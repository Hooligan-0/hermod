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
#ifndef SERVER_FASTCGI_HPP
#define SERVER_FASTCGI_HPP

#include <vector>
#include "Server.hpp"

namespace hermod {

class ServerFastcgi : public Server
{
public:
	ServerFastcgi();
	~ServerFastcgi();
	int  getFd    (unsigned int index = 0);
	void processFd(int fd = -1);
	void send     (const char *data, int len);
	void setClient(int fd);
	void setPort(int num);
	void start(void);
	void stop (void);
protected:
	void clientEvent(void);
	void serverEvent(void);
private:
	int mMode;
	int mPort;
	std::vector <ServerFastcgi *> mClients;
	unsigned char  mRxHeader[8];
	unsigned int   mRxHeaderLength;
	unsigned char *mRxBuffer;
	unsigned int   mRxLength;
};

} // namespace hermod
#endif
