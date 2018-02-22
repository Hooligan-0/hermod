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
#ifndef SERVER_HPP
#define SERVER_HPP

#include "Router.hpp"

namespace hermod {

/**
 * @class Server
 * @brief This class define a generic server skeleton
 *
 * Hermod is designed to serve requests from different sources. A link or an
 * endpoint that can handle requests from outside world is named a server.
 * This class must be used as a parent class for each specific server
 * implementations.
 */
class Server
{
public:
	Server();
	virtual ~Server();

	virtual int  getFd    (unsigned int index = 0);
	virtual void processFd(int fd = -1);
	virtual void send     (const String &content);
	virtual void send     (const char *data, int len) = 0;
	virtual void setRouter(Router *router);
	virtual void start(void) = 0;
	virtual void stop (void) = 0;

protected:
	int mFd;
	Router *mRouter;
};

} // namespace hermod
#endif
