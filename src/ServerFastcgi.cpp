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
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include "Config.hpp"
#include "Log.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Router.hpp"
#include "ServerFastcgi.hpp"
#include "String.hpp"

namespace hermod {

typedef struct
{
	unsigned char version;
	unsigned char type;
	unsigned char requestIdB1;
	unsigned char requestIdB0;
	unsigned char contentLengthB1;
	unsigned char contentLengthB0;
	unsigned char paddingLength;
	unsigned char reserved;
} FCGI_Record;


/**
 * @brief Default constructor
 *
 */
ServerFastcgi::ServerFastcgi()
  : Server()
{
	mMode = 0; // Define as server
	mPort = 9000;

	mClients.clear();
	mRxBuffer = 0;
	mRxHeaderLength = 0;
}

/**
 * @brief Default destructor
 *
 */
ServerFastcgi::~ServerFastcgi()
{
	if (mFd >= 0)
	{
		close(mFd);
		mFd = -1;
	}
}

void ServerFastcgi::clientEvent(void)
{
	int len;

	try {
		FCGI_Record   *rec;
		unsigned int   recLen;

		//
		if (mRxHeaderLength < 8)
		{
			len = 8 - mRxHeaderLength;

			len = read(mFd, mRxHeader + mRxHeaderLength, len);
			// If read length is 0, socket has been closed
			if (len == 0)
				throw -1;
			// A negative value is returned in case of error
			if (len < 0)
				throw runtime_error("ERROR reading from socket");

			mRxHeaderLength += len;

			if (mRxHeaderLength < 8)
				return;

			rec = (FCGI_Record *)mRxHeader;
			recLen = (rec->contentLengthB0) | (rec->contentLengthB1 << 8);
			recLen += rec->paddingLength;
			mRxBuffer = (unsigned char *)malloc(recLen);
			if (mRxBuffer == 0)
				throw std::bad_alloc();
			mRxLength = 0;

			if (recLen != 0)
				return;
		}

		//
		rec = (FCGI_Record *)mRxHeader;
		recLen = (rec->contentLengthB0) | (rec->contentLengthB1 << 8);

		if (mRxLength < recLen)
		{
			// Compute length of data to get
			len = (recLen + rec->paddingLength) - mRxLength;
			// Get datas from socket
			len = read(mFd, mRxBuffer + mRxLength, len);
			// If read length is 0, socket has been closed
			if (len == 0)
				throw -1;
			// A negative value is returned in case of error
			if (len < 0)
				throw runtime_error("ERROR reading from socket");

			// Update length of received datas
			mRxLength += len;
			if (mRxLength < (recLen + rec->paddingLength))
				return;
		}
		Log::debug() << "FCGI Record type " << rec->type
		             << " len=" << recLen
		             << " buffer_len=" << mRxLength << Log::endl;
		Log::sync();

		// ToDo : Process packet

		// Processing complete, discard packet
		free(mRxBuffer);
		mRxBuffer = 0;
		mRxLength = 0;
		mRxHeaderLength = 0;

	} catch(int ecode) {
		Log::error() << "Server: clientEvent " << mFd
		             << " ecode=" << ecode << Log::endl;
		if (ecode == -1)
		{
			close (mFd);
			mFd = -1;
		}
	} catch(...) {
		Log::error() << "Server: Fastcgi::clientEvent" << Log::endl;
	}
}

int ServerFastcgi::getFd(unsigned int index)
{
	int fd;

	if (index == 0)
		fd = mFd;
	else
	{
		ServerFastcgi *client = 0;

		if (index <= mClients.size())
		{
			client = mClients.at(index - 1);
			fd = client->getFd();
		}
		else
			fd = -1;
	}

	return(fd);
}

/**
 * @brief Handler called when an event is detected on server socket
 *
 */
void ServerFastcgi::processFd(int fd)
{
	if (mMode == 1)
	{
		clientEvent();
	}
	else
	{
		if ( (fd == mFd) || (fd == -1) )
			serverEvent();
		else
		{
			ServerFastcgi *client;

			// Search the client associated with requested fd
			std::vector<ServerFastcgi *>::iterator it;
			for (it = mClients.begin(); it != mClients.end(); ++it)
			{
				if (fd == (*it)->getFd())
				{
					client = *it;
					break;
				}
			}

			//
			if (client)
				client->processFd();
		}
	}
}

/**
 * @brief Send data as response of a request
 *
 * @param data Pointer to a buffer with datas to send
 * @param len  Length of the buffer
 */
void ServerFastcgi::send(const char *data, int len)
{
	(void)data;
	(void)len;
	return;
}

void ServerFastcgi::serverEvent(void)
{
	ServerFastcgi *client = 0;
	int fd = -1;

	if (mRouter == 0)
	{
		Log::error() << "Server-FastCGI: Failed to process FD (no router)" << Log::endl;
		Log::sync();
		// Stop the server to avoid infinite error loop
		stop();
		return;
	}

	try {
		struct sockaddr_in client_addr;
		socklen_t clilen;

		clilen = sizeof(client_addr);
		fd = accept(mFd, (struct sockaddr *)&client_addr, &clilen);
		if (fd < 0) 
			throw runtime_error("ERROR on accept");

		// Create a new object to handle client connection
		client = new ServerFastcgi();
		client->setClient(fd);

		mClients.push_back(client);

		Log::info() << "FastCGI accept client. fd=" << fd << Log::endl;
		Log::sync();
	} catch(...) {
		Log::error() << "Server-Fastcgi: Failed to accept incoming connection" << Log::endl;
		// Delete/clean the client object (if any)
		if (client)
		{
			delete client;
			client = 0;
		}
		// Close the socket (if already opened)
		if (fd >= 0)
			close(fd);

		throw;
	}
	Log::info() << "FastCGI Accepted connection fd=" << fd << Log::endl;
	Log::sync();
}

void ServerFastcgi::setClient(int fd)
{
	// Define this object as client
	mMode = 1;

	// Save the specified socket
	mFd = fd;
}

/**
 * @brief Set the (tcp) port number where server must listen
 *
 * @param num Port number to use
 */
void ServerFastcgi::setPort(int num)
{
	mPort = num;
}

/**
 * @brief Start the FCGI server
 *
 * This method allow to start FCGI server.
 */
void ServerFastcgi::start(void)
{
	Config *cfg = Config::getInstance();

	// If a server socket already defined
	if (mFd > 0)
		// Nothing to do, server is started
		return;

	try {
		// Define the TCP port to listen
		ConfigKey *keyPort = cfg->getKey("global", "port");
		if (keyPort)
			mPort = keyPort->getInteger();
	} catch(...) {
		// In case of error, use default config
		mPort = 9000;
	}

	try {
		struct sockaddr_in serv_addr;

		// First, create a socket for input connections
		mFd = socket(AF_INET, SOCK_STREAM, 0);
		if (mFd < 0)
			throw std::runtime_error("Failed to create socket");

		// Set the socket configuration
		memset((char *)&serv_addr, 0, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(mPort);
		// Start listening on server socket
		if (bind(mFd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
			throw std::runtime_error("Failed to bind socket");
		listen(mFd, 5);

	} catch (std::exception &e) {
		Log::error() << "Server: Server NOT started: "
		             << e.what() << Log::endl;
	} catch (...) {
		Log::error() << "Server: Server NOT started: "
		             << "Unknown reason" << Log::endl;
		if (mFd >= 0)
		{
			close(mFd);
			mFd = -1;
		};
	}
}

/**
 * @brief Stop the FastCGI server.
 *
 * This method allow to stop FastCGI server without deleting it.
 */
void ServerFastcgi::stop(void)
{
	if (mFd >= 0)
	{
		// Close FastCGI server socket
		close(mFd);
		mFd = -1;
	}
}

} // namespace hermod
/* EOF */
