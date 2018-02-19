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
#include <cstdlib>
#include <unistd.h>
#include "Log.hpp"
#include "Server.hpp"

namespace hermod {

/**
 * @brief Default constructor
 *
 */
Server::Server()
{
	mFd = -1;
	mRouter = NULL;
}

/**
 * @brief Default destructor
 *
 */
Server::~Server(void)
{
	// Nothing to do but needed for derivated classes
}

/**
 * @brief Get the descriptor used by server to listen for events
 *
 * @param index Index to get (when server can handle multiple connection)
 * @return integer Descriptor ID
 */
int Server::getFd(unsigned int index)
{
	if (index != 0)
		return (-1);

	return mFd;
}

/**
 * @brief Default event handler - Must be overloaded
 *
 * This method is called when an event is received on server descriptor. The
 * specific implementation must overload this method to *really* process event.
 * Here this fallback method just close the descriptor because if not overloaded
 * nobody know how to process such event.
 */
void Server::processFd(int fd)
{
	Log::warning() << "Server: Event on descriptor " << fd
	               << " but no processing function available !" << Log::endl;
	// Close server FD to avoid further error
	close(mFd);
	mFd = -1;
}

/**
 * @brief Set the router associated with this server
 *
 * @param Router* Pointer to the router to use.
 */
void Server::setRouter(Router *router)
{
	mRouter = router;
}

} // namespace hermod
/* EOF */
