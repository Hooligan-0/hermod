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
#ifndef APP_HPP
#define APP_HPP
#include "ModuleCache.hpp"
#include "Router.hpp"
#include "Server.hpp"

namespace hermod {

/**
 * @class App
 * @brief The App class manage all resources of an application.
 *
 * An hermod server is mainly a framework to create web services. There is many
 * resources and layers available to cover the standard requirements (a router,
 * a session manager, ...) Each server has one App object that manage everything
 *
 */
class App
{
public:
	static void destroy();
	App * exec (void);
	App * init (void);
	static App* getInstance();
public:
	static void sigInt(void);
private:
	App();
	~App();
private:
    	static App*  mAppInstance;
	bool         mRunning;
	Server      *mServer;
	Router      *mRouter;
	ModuleCache  mModuleCache;
};

} // namespace hermod
#endif