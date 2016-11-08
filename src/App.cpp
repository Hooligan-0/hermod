/*
 * Hermod - Modular application framework
 *
 * Copyright (c) 2016 Cowlab
 *
 * Hermod is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License 
 * version 3 as published by the Free Software Foundation. You
 * should have received a copy of the GNU Lesser General Public
 * License along with this program, see LICENSE file for more details.
 * This program is distributed WITHOUT ANY WARRANTY see README file.
 *
 * Authors: Saint-Genest Gwenael <gwen@hooligan0.net>
 */
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <string>
#include <fcgios.h> // OS_IpcClose()

#include "App.hpp"
#include "Config.hpp"
#include "Log.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Router.hpp"
#include "SessionCache.hpp"

// Call to OS_LibShutdown() is needed to avoid fcgi leak
extern "C"
{
void OS_LibShutdown(void);
}

namespace hermod {

App*  App::mAppInstance = NULL;

/**
 * @brief Default constructor
 *
 */
App::App()
{
	mRunning  = false;
	mFcgxSock = -1;
	mRouter   = NULL;
}

/**
 * @brief Default destructor
 *
 */
App::~App()
{
}

/**
 * @brief This method should be used to delete the App singleton
 *
 * The App class is designed to have only one instance (singleton), allocation
 * and desallocation are made by class itself using static methods. This destroy
 * method can be called to delete the global App object (and clean all
 * associated resources)
 */
void App::destroy(void)
{
	if ( ! mAppInstance)
		return;

	// Unload modules
	mAppInstance->mModuleCache.clear();

	// Clear Log layer
	Log::destroy();
	
	// Close FCGI socket
	OS_IpcClose(mAppInstance->mFcgxSock);
	mAppInstance->mFcgxSock = -1;
	// Free fcgi
	OS_LibShutdown();

	// Delete singleton object
	delete mAppInstance;
	mAppInstance = NULL;
}

/**
 * @brief Get access to the global App object (singlaton) ... or create it
 *
 * @return App* Pointer to the global App object
 */
App* App::getInstance()
{
	// If App not already available, create it
	if ( ! mAppInstance)
		mAppInstance = new App;

	// Return the global App object
	return mAppInstance;
}

/**
 * @brief Entry point to start the App
 *
 * This method is the main place of an App. It must be called to start the
 * application, and the app is stopped when this method returns. An app works
 * using events on descriptors, a big loop wait and dispatch events.
 *
 * @return App* Pointer to the App object
 */
App* App::exec(void)
{
	try {
		mRunning = true;

		while(mRunning)
		{
			fd_set rfds;
			struct timeval tv;
			int retval;

			FD_ZERO(&rfds);
			FD_SET(mFcgxSock, &rfds);

			tv.tv_sec = 1;
			tv.tv_usec = 0;
			retval = select(mFcgxSock+1, &rfds, NULL, NULL, &tv);
			if (retval > 0)
			{
				if (FD_ISSET(mFcgxSock, &rfds) )
					processFcgi();
			}
			else if (retval == 0)
			{
				// Case of timeout reserved for future use
				continue;
			}

			// Flush Log
			Log::sync();
		} /* while */
	}
	catch(std::exception& e) {
		cout << "App::exception " << endl;
		cout << e.what() << endl;
	}
	
	try {
		// Clear the local Router
		delete mRouter;
		mRouter = NULL;
		// Clear the Session cache
		SessionCache::destroy();
		// Clear Config cache
		Config::destroy();
	} catch(std::exception& e) {
		// ToDo: handle error ? !
	}
	
	return this;
}

/**
 * @brief Initialize an application before starting it
 *
 * @return App* Pointer to the App object
 */
App* App::init(void)
{
	Config *cfg = Config::getInstance();
	
	try {
		String cfgFile;
		cfgFile = cfg->get("global", "log_file");
		Log::setFile(cfgFile);
	} catch (std::exception& e) {
		// ToDo: print some message ?
	}

	// Init random number generator
	std::srand(std::time(0));

	// Create a Router for this App
	mRouter = new Router;

	// Load external modules
	size_t pos = 0;
	while(1)
	{
		String name;
		name = cfg->get("plugins", "load", &pos);
		if (name.isEmpty())
			break;
		// Try to load the specified module
		Module *newModule = mModuleCache.load( name );
		if (newModule)
		{
			newModule->initRouter(mRouter);
			Log::info() << "Load module " << name << Log::endl;
		}
		pos++;
	}

	// Open FCGI socket
	try {
		std::string fcgiPort(":");
		// Initialize library
		FCGX_Init();
		// Define the TCP port to listen
		ConfigKey *keyPort = cfg->getKey("global", "port");
		if (keyPort)
			fcgiPort += keyPort->getValue();
		else
			fcgiPort += "9000";
		// Open the FCGI socket
		mFcgxSock = FCGX_OpenSocket(fcgiPort.c_str(), 4);
	} catch (...) {
		// ToDo: handle error
	}
	Log::info() << "Hermod (FCGI) started socket=" << mFcgxSock << Log::endl;

	Log::sync();
	return getInstance();
}

/**
 * @brief When a request is received on FCGI socket, this method process it
 *
 */
void App::processFcgi (void)
{
	FCGX_Request fcgiReq;
	Request  *req;
	Response *rsp;
	
	FCGX_InitRequest(&fcgiReq, mFcgxSock, 0);
	
	if (FCGX_Accept_r(&fcgiReq) != 0)
	{
		Log::info() << "FastCGI interrupted during accept." << Log::endl;
		Log::sync();
		return;
	}
	// Instanciate a new Request
	req = new Request( &fcgiReq );
	// Instanciate a new Response
	rsp = new Response( req );

	if (req->getMethod() == Request::Option)
	{
		ResponseHeader *rh = rsp->header();
		rh->addHeader("Allow", "HEAD,GET,PUT,DELETE,OPTIONS");
		rh->addHeader("Access-Control-Allow-Headers", "access-control-allow-origin,x-requested-with");
		std::string corsMethod = req->getParam("HTTP_CORS_METHOD");
		if ( ! corsMethod.empty() )
			rh->addHeader("Access-Control-Allow-Method", corsMethod);
	}
	else if ( (req->getMethod() == Request::Get) ||
	          (req->getMethod() == Request::Post) )
	{
		RouteTarget *route = mRouter->find(req);
		if ( ! route)
		{
			Log::info() << "Request an unknown URL: ";
			Log::info() << req->getUri(0) << Log::endl;
			route = mRouter->find(":404:");
		}
		if (route)
		{
			Log::info() << "App: Found a route for this request " << Log::endl;
			Page *page = route->newPage();
			if (page)
			{
				rsp->catchCout();

				try {
					page->setRequest( req );
					page->setReponse( rsp );
					page->initSession();
					page->process();
				} catch (std::exception &e) {
					Log::info() << "Request::process Exception " << e.what() << Log::endl;
				}

				rsp->releaseCout();

				route->freePage(page);
			}
			else
			{
				Log::info() << "App: Failed to load target page" << Log::endl;
				rsp->header()->setRetCode(404, "Not found");
			}
		}
		else
		{
			Log::info() << "No page in config for '404' error" << Log::endl;
			rsp->header()->setRetCode(404, "Not found");
		}
	}
	else
	{
		Log::info() << "App: Unknown method for this request :(" << Log::endl;
		rsp->header()->setRetCode(404, "Not found");
	}

	rsp->send();

	// Delete "Response" object at the end of the process
	delete rsp;
	rsp = NULL;
	// Delete "Request" object at the end of the process
	delete req;
	req = 0;
	
	FCGX_Finish_r(&fcgiReq);

	FCGX_Free(&fcgiReq, 0);
}

/**
 * @brief This static method handle OS based signals (mainly SIGINT)
 *
 */
void App::sigInt(void)
{
	if (mAppInstance)
		mAppInstance->mRunning = false;
	
	FCGX_ShutdownPending();
}

} // namespace hermod
/* EOF */
