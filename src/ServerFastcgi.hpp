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
#include "Request.hpp"
#include "Response.hpp"
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
	void clientDecodeParam(unsigned int len);
	void clientEvent(void);
	void serverEvent(void);
	void sendEndRequest(void);
private:
	int mMode;
	int mPort;
	int mState;
	std::vector <ServerFastcgi *> mClients;
	unsigned char  mRxHeader[8];
	unsigned int   mRxHeaderLength;
	unsigned char *mRxBuffer;
	unsigned int   mRxLength;
private:
	unsigned short mRecId;
	String   *mBody;
	Request  *mRequest;
	Response *mResponse;
};

} // namespace hermod
#endif
