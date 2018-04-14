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

#include "App.hpp"
#include "Config.hpp"
#include "Log.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Router.hpp"
#include "SessionCache.hpp"
#include "ServerFastcgi.hpp"
#include "ServerLibFcgi.hpp"

namespace hermod {

App*  App::mAppInstance = NULL;

/**
 * @brief Default constructor
 *
 */
App::App()
{
	mRunning  = false;
	mRouter   = NULL;
	mServer   = NULL;
}

/**
 * @brief Default destructor
 *
 */
App::~App()
{
	if (mServer)
	{
		delete mServer;
		mServer = 0;
	}
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

	if (mAppInstance->mServer)
		mAppInstance->mServer->stop();

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
	if (mServer == 0)
		throw runtime_error("APP: Failed to start (no server)");

	try {
		mRunning = true;

		while(mRunning)
		{
			fd_set rfds;
			struct timeval tv;
			int retval;
			int maxFd  = 0;
			int i;

			FD_ZERO(&rfds);
			for (i = 0; /**/; i++)
			{
				// Get the next server fd
				int fd = mServer->getFd(i);
				// If this is the last fd (or no fd available)
				if (fd < 0)
					break;
				// Insert this fd into the "select" list
				FD_SET(fd, &rfds);
				// Update the highest fd number
				if (fd >= maxFd)
					maxFd = (fd + 1);
			}

			tv.tv_sec = 5;
			tv.tv_usec = 0;
			retval = select(maxFd, &rfds, NULL, NULL, &tv);
			if (retval > 0)
			{
				for (i = 0; i < maxFd; i++)
				{
					if ( ! FD_ISSET(i, &rfds))
						continue;
					mServer->processFd(i);
				}
			}
			else if (retval == 0)
			{
				// Case of timeout reserved for future use
				SessionCache::clean();
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
	String  cfgServer("libfcgi");
	
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

	// Load Routes (after loading modules)
	mRouter->reloadConfig();

	try {
		ConfigKey *cfgKey;
		// Get the server type from config
		cfgKey = cfg->getKey("global", "server");
		// If the config key exists, read it
		if (cfgKey)
			cfgServer = cfgKey->getValue();
	} catch (std::exception& e) {
		// ToDo: print some message ?
	}
	Log::info() << "App: server type = " << cfgServer << Log::endl;

	// Start FCGI server
	try {
		if (cfgServer == "fastcgi")
		{
			ServerFastcgi *server;
			// Create a new FastCGI server
			server = new ServerFastcgi();

			// Register the local router into server
			server->setRouter(mRouter);

			// Start server ! :)
			mServer = server;
		}
		else if (cfgServer == "libfcgi")
		{
			ServerLibFcgi *server;
			// Create the FCGI default interface
			server = new ServerLibFcgi();

			// Register the local router into server
			server->setRouter(mRouter);

			// Start server ! :)
			mServer = server;
		}
		else
			throw std::runtime_error("Unknown server type declared");

		mServer->start();

	} catch (std::exception& e) {
		Log::error() << "Failed to start Hermod server: "
		             << e.what() << Log::endl;
		throw;
	} catch (...) {
		// ToDo: handle error
	}
	Log::info() << "Hermod (FCGI) started" << Log::endl;
	Log::sync();

	return getInstance();
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
