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
#include "Response.hpp"
#include "Router.hpp"
#include "ServerFastcgi.hpp"
#include "String.hpp"

namespace hermod {

#define FCGI_BEGIN_REQUEST       1
#define FCGI_ABORT_REQUEST       2
#define FCGI_END_REQUEST         3
#define FCGI_PARAMS              4
#define FCGI_STDIN               5
#define FCGI_STDOUT              6
#define FCGI_STDERR              7
#define FCGI_DATA                8
#define FCGI_GET_VALUES          9
#define FCGI_GET_VALUES_RESULT  10
#define FCGI_UNKNOWN_TYPE       11
#define FCGI_MAXTYPE (FCGI_UNKNOWN_TYPE)

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

	mState = 0;

	mClients.clear();
	mRxBuffer = 0;
	mRxHeaderLength = 0;
	mBody = 0;

	mRequest  = 0;
	mResponse = 0;
}

/**
 * @brief Default destructor
 *
 */
ServerFastcgi::~ServerFastcgi()
{
	// Clean the client objects (if any)
	while(mClients.size())
	{
		ServerFastcgi *c = mClients.back();
		mClients.pop_back();
		delete c;
	}

	// Close the client or the server socket (if open)
	if (mFd >= 0)
	{
		close(mFd);
		mFd = -1;
	}

	// If a Request has been allocated, delete it
	if (mRequest)
	{
		delete mRequest;
		mRequest = 0;
	}
	// If a Response has been allocated, delete it
	if (mResponse)
	{
		delete mResponse;
		mResponse = 0;
	}
	// If a Body has been allocated and not used by Request, delete it
	if (mBody)
	{
		delete mBody;
		mBody = 0;
	}
}

/**
 * @brief Decode a FCGI packet with parameters
 *
 * @param len Length of the received data packet (withour padding)
 */
void ServerFastcgi::clientDecodeParam(unsigned int len)
{
	char *ptr;

	// Sanity check
	if (mRxBuffer == 0)
		return;

	ptr = (char *)mRxBuffer;

	// A packet may contains multiple parameters, decode each
	for (unsigned int i = 0; i < len; )
	{
		char argName[64];
		char argValue[64];

		// First two bytes contains fields lengths
		int nameLen  = ptr[0];
		int valueLen = ptr[1];

		ptr += 2;
		i   += 2;

		// Extract the parameter name
		bzero(argName, 64);
		strncpy(argName, (char *)ptr, nameLen);
		ptr += nameLen;
		i   += nameLen;
		// Extract the parameter value
		bzero(argValue, 64);
		strncpy(argValue, (char *)ptr, valueLen);
		ptr += valueLen;
		i   += valueLen;

		// Add this HTTP parameter into Request
		mRequest->setHeaderParameter(argName, argValue);
	}
}

void ServerFastcgi::clientEvent(void)
{
	int len;

	try {
		FCGI_Record   *rec;
		unsigned int   recLen;

		// Sanity check
		if (mRouter == 0)
			throw -3;

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
		if (rec->type == FCGI_BEGIN_REQUEST)
		{
			// Save the request ID
			mRecId = (rec->requestIdB1 << 8) | rec->requestIdB0;

			// Instanciate a Request
			mRequest = new Request(this);
			// Instanciate a Response for this request
			mResponse = new Response(mRequest);
			mResponse->setServer(this);

			mState = 1;
		}
		else if (rec->type == FCGI_PARAMS)
		{
			if (recLen)
				clientDecodeParam(recLen);
			if (recLen == 0)
				mState = 2;
		}
		else if (rec->type == FCGI_STDIN)
		{
			if (recLen)
			{
				int bodyLength = 0;
				// Search body (content) length into HTTP headers
				String contentLength = mRequest->getParam("CONTENT_LENGTH");
				if ( ! contentLength.isEmpty())
					bodyLength = contentLength.toInt();
				if (bodyLength > 0)
				{
					unsigned int j;
					char *pIn, *pOut;
					// Allocate a String to hold body, and get it
					mBody = new String();
					mBody->reserve(len);
					pIn  = (char *)mRxBuffer;
					pOut = mBody->data();
					for (j = 0; j < recLen; j++)
					{
						*pOut++ = *pIn++;
					}
				}
			}
			if (recLen == 0)
			{
				// Set this body into Request (give owership of buffer)
				mRequest->setBody(mBody);
				mBody = 0;

				RouteTarget *route = mRouter->find(mRequest);
				if (route)
				{
					Page *page = route->newPage();
					if (page)
					{
						mResponse->catchCout();
						try {
							page->setRequest(mRequest);
							page->setReponse(mResponse);
							page->initSession();
							page->process();
						} catch (std::exception &e) {
							Log::info() << "Request::process Exception " << e.what() << Log::endl;
						}
						mResponse->releaseCout();
						route->freePage(page);
					}
					else
					{
						mResponse->header()->setRetCode(404, "Not found");
					}
				}

				mResponse->send();
				sendEndRequest();
				close(mFd);
				mFd = -1;
			}
		}

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
		if (ecode == -3)
			throw std::runtime_error("ServerFastcgi: missing Router");
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

			// If a client has been found
			if (client)
			{
				client->processFd();
				// If the client socket has ben closed
				if (client->getFd() < 0)
				{
					// Search the client into local cache
					std::vector<ServerFastcgi *>::iterator it;
					for (it = mClients.begin(); it != mClients.end(); ++it)
					{
						if (client != (*it))
							continue;
						mClients.erase(it);
						break;
					}
				}
			}
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
	FCGI_Record rec;

	// Create a FCGI record header
	rec.version = 1;
	rec.type    = FCGI_STDOUT;
	rec.requestIdB1     = (mRecId <<   8);
	rec.requestIdB0     = (mRecId & 0xFF);
	rec.contentLengthB1 = ( len   >>   8);
	rec.contentLengthB0 = ( len   & 0xFF);
	rec.paddingLength   = 0;
	rec.reserved        = 0;
	// Send it
	write(mFd, &rec, 8);

	if (len > 0)
		write(mFd, data, len);

	return;
}

/**
 * @brief Finish a FCGI transaction by sending END_REQUEST
 *
 */
void ServerFastcgi::sendEndRequest(void)
{
	FCGI_Record rec;
	char buffer[8];

	// Send a STDOUT record without content to finish STDOUT step
	send(0, 0);

	// Create a FCGI record header
	rec.version = 1;
	rec.type    = FCGI_END_REQUEST;
	rec.requestIdB1     = (mRecId <<   8);
	rec.requestIdB0     = (mRecId & 0xFF);
	rec.contentLengthB1 = 0;
	rec.contentLengthB0 = 8;
	rec.paddingLength   = 0;
	rec.reserved        = 0;
	// Send it
	write(mFd, &rec, 8);

	// Set appStatus value
	buffer[0] = 0;
	buffer[1] = 0;
	buffer[2] = 0;
	buffer[3] = 0;
	// Set protocolStatus value
	buffer[4] = 0; // FCGI_REQUEST_COMPLETE
	// Reserved
	buffer[5] = 0;
	buffer[6] = 0;
	buffer[7] = 0;
	// Send it
	write(mFd, buffer, 8);
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
		client->setRouter(mRouter);

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
