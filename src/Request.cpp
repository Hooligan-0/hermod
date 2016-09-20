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

using namespace std;

Request::Request(FCGX_Request *req)
{
	mFcgiRequest = req;
	mModuleCache = 0;

	const char *u = FCGX_GetParam("QUERY_STRING", req->envp);
	if (u)
	{
		std::string qs(u);
		mUri.push_back( qs );
	}
}

Request::~Request()
{
	// Nothing to do
}

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

Request::Method Request::getMethod(void)
{
	std::string method = getParam("REQUEST_METHOD");
	if ( method.compare("OPTIONS") == 0 )
		return Option;
	else if ( method.compare("GET") == 0 )
		return Get;
	else if ( method.compare("POST") == 0 )
		return Post;
	else
		return Undef;
}

std::string Request::getParam (const std::string &name)
{
	std::string value;
	
	const char *p = FCGX_GetParam(name.c_str(), mFcgiRequest->envp);
	if (p)
		value = p;
	
	return value;
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
