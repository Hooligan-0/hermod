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

namespace hermod {

/**
 * @brief Default constructor
 *
 * @param request Pointer to a Request associated with this response
 */
Response::Response(Request *request)
{
	mContent     = 0;
	mCoutBackup  = NULL;
	mRequest     = NULL;
	if (request)
		setRequest(request);
}

/**
 * @brief Defaut destructor
 *
 */
Response::~Response()
{
	if (mContent)
	{
		delete mContent;
		mContent = 0;
	}
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
 * @brief Get access to the response content
 *
 * @return ResponseHeader Pointer to the current header object
 */
Content *Response::content(void)
{
	return mContent;
}

/**
 * @brief Get access to the response header
 *
 * @return ResponseHeader Pointer to the current header object
 */
ResponseHeader *Response::header(void)
{
	return &mHeader;
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
	// Send Header
	FCGX_PutS(mHeader.getHeader().data(), fout);
	// Send Content
	if (mContent)
	{
		const char *ptrContent = mContent->getCBuffer();
		FCGX_PutStr(ptrContent, mContent->size(), fout);
	}
	// Send cout buffer
	FCGX_PutS(mCoutBuffer.str().c_str(), fout);
}

/**
 * @brief Set a content for this response
 *
 * This method allow to associate with this response an object that holds a
 * buffer of datas. The life cycle of a response is longer than page or other
 * objects that produce datas. To avoid data loss, response take control of the
 * content after call of this method (must not be deleted elsewhere) !!
 *
 * @param content Pointer to the Content
 */
void Response::setContent(Content *content)
{
	mContent = content;
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
			mHeader.addHeader("Access-Control-Allow-Origin", o);
	}

	// Allow credentials control
	mHeader.addHeader("Access-Control-Allow-Credentials", "true");
}

} // namespace hermod
/* EOF */
