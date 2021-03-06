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
#ifndef SERVER_LIBFCGI_HPP
#define SERVER_LIBFCGI_HPP

#include <fcgio.h>
#include "Request.hpp"
#include "Server.hpp"

namespace hermod {

class ServerLibFcgi : public Server
{
public:
	ServerLibFcgi();
	~ServerLibFcgi();
	void processFd(int fd = -1);
	void send     (const char *data, int len);
	void setPort(int num);
	void start(void);
	void stop (void);
protected:
	void loadHttpBody      (Request *req, FCGX_Request *fcgi);
	void loadHttpParameters(Request *req, FCGX_Request *fcgi);
private:
	int mPort;
	int mSocketFd;
	FCGX_Request *mFCGX;
};

} // namespace hermod
#endif
