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
#include <stdexcept>
#include "Request.hpp"
#include "Log.hpp"
#include "String.hpp"

using namespace std;

/**
 * @brief Default constructor
 *
 */
Request::Request(FCGX_Request *req)
{
	mFcgiRequest = req;
	mModuleCache = 0;
	mMethod      = Undef;

	const char *u = FCGX_GetParam("QUERY_STRING", req->envp);
	if (u)
	{
		std::string qs(u);
		mUri.push_back( qs );
	}
}

/**
 * @brief Default destructor
 *
 */
Request::~Request()
{
	// Nothing to do
}

/**
 * @brief Get the number of arguments into requested URI
 *
 * @return integer Number of known arguments
 */
unsigned int Request::countUriArgs(void)
{
	if (mUri.empty())
		return 0;
	
	// Get the number of elements into Uri
	unsigned int count = mUri.size();
	// Decrement count to ignore arg(0) (route itself)
	count--;
	
	return count;
}

std::string Request::getCookieByName(const std::string &name, bool allowEmpty = false)
{
	std::string value("");

	try {
		const char *c = FCGX_GetParam("HTTP_COOKIE", mFcgiRequest->envp);
		if (c == 0)
			throw runtime_error("No Cookie available");
		std::string qs(c);

		std::size_t pos = 0;
		while(1)
		{
			pos = qs.find(";", pos);
			std::string cookie = qs.substr(0, pos);

			std::size_t sep = cookie.find("=");
			if (sep == std::string::npos)
				continue;
			std::string cName  = cookie.substr(0, sep);
			std::string cValue = cookie.substr(sep + 1, std::string::npos);

			if ( ! cName.compare(name) )
			{
				value = cValue;
				break;
			}

			if (pos == std::string::npos)
				break;
		}
	} catch (std::exception &e) {
		Log::debug() << "Request::getCookieByName " << name;
		Log::debug() << " : " << e.what() << Log::endl;
	}

	if ( value.empty() && (allowEmpty == false) )
		throw runtime_error("Not Found");

	return value;
}

FCGX_Request *Request::getFCGX(void)
{
	return mFcgiRequest;
}

/**
 * @brief Get the HTTP method of this request
 *
 * @return Method The requested method
 */
Request::Method Request::getMethod(void)
{
	if (mMethod != Undef)
		return mMethod;

	String method = getParam("REQUEST_METHOD");
	if (method == "OPTIONS")
		mMethod = Option;
	else if (method == "GET")
		mMethod = Get;
	else if (method == "POST")
		mMethod = Post;

	return mMethod;
}

/**
 * @brief Read the value of an environment variable
 *
 * @param name Name of the environment variable
 * @return String Value for this variable
 */
String Request::getParam (const String &name)
{
	String value = FCGX_GetParam(name.data(), mFcgiRequest->envp);

	return value;
}

/**
 * @brief Get the value of a posted variable
 *
 * @param name Name of the variable
 * @return String Value of the variable
 */
String Request::getFormValue(const String &name)
{
	// If the list of received variables is empty, refresh it
	if (mFormParameters.empty())
		loadFormInputs();

	// Get the value from cache, and return it
	return mFormParameters[ name ];
}

std::string Request::getUri(unsigned int n = 0)
{
	std::string uri;
	
	if (n < mUri.size())
	{
		uri = mUri.at(n);
	}
	
	return uri;
}

/**
 * @brief Read the posted content and insert variables into form cache
 *
 */
void Request::loadFormInputs(void)
{
	Method method = getMethod();
	
	if (method == Post)
	{
		String contentLength = getParam("CONTENT_LENGTH");
		// Convert content length to a numeric value
		int len = contentLength.toInt();
		if (len == 0)
			return;
		// Arbitraty upper limit ... must be improved in future
		if (len > (1024 * 1024))
			return;
		// Get datas
		String buffer;
		buffer.reserve(len);
		FCGX_GetStr(buffer, len, mFcgiRequest->in);
		char *pnt = buffer;
		char *token = buffer;
		String paramName;
		String paramValue;
		while(1)
		{
			if ((*pnt == 0) || (*pnt == '&'))
			{
				bool isLast = (*pnt == 0);
				*pnt = 0;
				paramValue = token;
				mFormParameters[ paramName ] = paramValue;
				if (isLast)
					break;
				*pnt = '&';
				token = (pnt + 1);
			}
			else if (*pnt == '=')
			{
				*pnt = 0;
				paramName = token;
				token = (pnt + 1);
				*pnt = '=';
			}
			else if (*pnt == '&')
			{
				paramName.clear();
				token = (pnt + 1);
			}
			pnt ++;
		}
	}
}

void Request::setModules(ModuleCache *cache)
{
	mModuleCache = cache;
}

void Request::setUri(const std::string &route)
{
	if (mUri.size() == 0)
		return;
	
	// Get full URI string
	std::string args = getParam("QUERY_STRING");
	// Keep only the tail (where args are)
	args.erase(0, route.length());
	if (args.length())
	{
		if (args[0] == '/')
			args.erase(0, 1);
	}
	
	mUri.clear();
	mUri.push_back(route);
	
	std::istringstream qs( args );
	for(std::string token; getline(qs, token, '/'); )
		mUri.push_back(token);
	
	Log::info() << "setUri " << mUri.at(0);
	Log::info() << "  route " << route;
	Log::info() << "  args count  " << mUri.size();
	Log::info() << "  args  " << args << Log::endl;
}
/* EOF */
