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
#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <sstream>
#include <streambuf>
#include "ResponseHeader.hpp"
#include "Content.hpp"
#include "Server.hpp"

namespace hermod {

class Request;

/**
 * @class Response
 * @brief Class used to build and send CGI responses
 *
 */
class Response {
public:
	explicit Response(Request *request = NULL);
	~Response();
	Content        *content();
	ResponseHeader *header();
	void catchCout  (void);
	void releaseCout(void);
	void send(void);
	void setContent(Content *content);
	void setRequest(Request *request);
	void setServer (Server  *server);
private:
	Request          *mRequest;
	ResponseHeader    mHeader;
	Server           *mServer;
	Content          *mContent;
	std::streambuf   *mCoutBackup;
	std::stringstream mCoutBuffer;
};

} // namespace hermod
#endif
