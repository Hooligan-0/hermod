/*
 * Hermod - Modular application framework
 *
 * Copyright (c) 2016-2018 Cowlab
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
#include <string>
#include <fcgio.h>
#include <fcgios.h> // For OS_* functions
#include "Config.hpp"
#include "Log.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Router.hpp"
#include "ServerLibFcgi.hpp"
#include "String.hpp"

namespace hermod {

/**
 * @brief Default constructor
 *
 */
ServerLibFcgi::ServerLibFcgi()
  : Server()
{
	mPort = 9000;
}

/**
 * @brief Default destructor
 *
 */
ServerLibFcgi::~ServerLibFcgi()
{
	// Free fcgi
	OS_LibShutdown();
}

/**
 * @brief Handler called when an event is detected on server socket
 *
 * This is th main processing method of this server. When an event is
 * detected on FCGI socket (mainly new connection) this method call
 * libfcgi to appept and decode it. Then, router is used to call a
 * page that handle the URI/method.
 */
void ServerLibFcgi::processFd(void)
{
	FCGX_Request fcgiReq;
	Request  *req;
	Response *rsp;

	if (mRouter == 0)
	{
		Log::error() << "Server:FastCGI: Failed to process FD (no router)" << Log::endl;
		Log::sync();
		return;
	}

	FCGX_InitRequest(&fcgiReq, mFd, 0);

	if (FCGX_Accept_r(&fcgiReq) != 0)
	{
		Log::info() << "FastCGI interrupted during accept." << Log::endl;
		Log::sync();
		return;
	}
	Log::info() << "FastCGI Accepted connection." << Log::endl;
	Log::sync();
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
 * @brief Set the (tcp) port number where server must listen
 *
 * @param num Port number to use
 */
void ServerLibFcgi::setPort(int num)
{
	mPort = num;
}

/**
 * @brief Start the FCGI server
 *
 * This method allow to start FCGI server.
 */
void ServerLibFcgi::start(void)
{
	Config *cfg = Config::getInstance();

	// Define the TCP port to listen
	ConfigKey *keyPort = cfg->getKey("global", "port");
	if (keyPort)
		mPort = keyPort->getInteger();

	// Initialize library
	FCGX_Init();

	std::string fcgiPort(":");
	fcgiPort += String::number(mPort).toStdStr();

	// Open the FCGI socket
	mFd = FCGX_OpenSocket(fcgiPort.c_str(), 4);
}

/**
 * @brief Stop the FCGI server.
 *
 * This method allow to stop FCGI server without deleting it.
 */
void ServerLibFcgi::stop(void)
{
	if (mFd >= 0)
	{
		// Close FCGI socket
		OS_IpcClose(mFd);
		mFd = -1;
	}
}

} // namespace hermod
/* EOF */