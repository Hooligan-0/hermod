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
#include <iostream>
#include <fcgio.h>
#include "Response.hpp"
#include "Request.hpp"

/**
 * @brief Default constructor
 *
 * @param request Pointer to a Request associated with this response
 */
Response::Response(Request *request)
{
	mCoutBackup  = NULL;
	mRequest     = NULL;
	if (request)
		setRequest(request);
}

/**
 * @brief Redirect standard cout to a local stream
 *
 */
void Response::catchCout(void)
{
	// Backup cout's streambuf
	mCoutBackup = std::cout.rdbuf();
	// Clear the intermediate buffer
	mCoutBuffer.str(std::string());
	// Redirect cout to intermediate buffer
	std::cout.rdbuf(mCoutBuffer.rdbuf());
}

/**
 * @brief Get access to the response header
 *
 * @return ResponseHeader Pointer to the current header object
 */
ResponseHeader *Response::header(void)
{
	return &mResponseHeader;
}

/**
 * @brief Restore cout that has previously been redirected by catchCout()
 *
 */
void Response::releaseCout(void)
{
	if (mCoutBackup == NULL)
		return;
	// Restore cout from the savec backup
	std::cout.rdbuf(mCoutBackup);
	mCoutBackup = NULL;
}

/**
 * @brief Send response to the remote FastCGI peer
 *
 * This method is called at the end of process to send header and http data to
 * the client.
 */
void Response::send(void)
{
	FCGX_Request *fcgi = mRequest->getFCGX();
	FCGX_Stream *fout = fcgi->out;
	FCGX_PutS(mResponseHeader.getHeader().c_str(), fout);
	FCGX_PutS(mCoutBuffer.str().c_str(), fout);
}

/**
 * @brief Set the request associated with this response into an HTTP transaction
 *
 * @param request Pointer to the Request
 */
void Response::setRequest(Request *request)
{
	mRequest = request;
	FCGX_Request *fcgi = mRequest->getFCGX();
	
	// Process "Origin" header
	const char *oh = FCGX_GetParam("HTTP_ORIGIN", fcgi->envp);
	if (oh)
	{
		std::string o( oh );
		if ( ! o.empty() )
			mResponseHeader.addHeader("Access-Control-Allow-Origin", o);
	}

	// Allow credentials control
	mResponseHeader.addHeader("Access-Control-Allow-Credentials", "true");
}
/* EOF */
