/*
 * Hermod - Modular application framework
 *
 * Copyright (c) 2016-2019 Cowlab
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
#include <map>
#include <string>
#include <sstream>
#include "ResponseHeader.hpp"

using namespace std;

namespace hermod {

/**
 * @brief Default constructor
 *
 */
ResponseHeader::ResponseHeader()
    : mContentType("text/plain")
{
	mRetCode = 200;
}

/**
 * @brief Insert a new item into the header
 *
 * @param key Reference to the name of the parameter to add
 * @param value The value for the parameter
 */
void ResponseHeader::addHeader(const String &key, String value)
{
	std::map<String, String>::iterator it;
	
	it = mHeaders.find(key.data());
	if (it != mHeaders.end())
		mHeaders.erase (it);
	mHeaders[key] = value;
}

/**
 * @brief Set the HTTP content-type of the Response
 *
 * @param type Reference to a string that contains the mime-type
 */
void ResponseHeader::setContentType(const String &type)
{
	mContentType = type;
}

/**
 * @brief Set the status code returned with the Response (default 200)
 *
 * @param code The status code to return with Response
 * @param reason Reference to a String with status information
 */
void ResponseHeader::setRetCode(int code, const String &reason)
{
	mRetCode   = code;
	mRetReason = reason;
}

/**
 * @brief Set the status code returned with the Response
 *
 * @param code The status code to return with Response
 */
void ResponseHeader::setRetCode(int code)
{
	mRetCode = code;

	switch (mRetCode)
	{
		// Informational 1xx
		case 100: mRetReason = "Continue";    break;
		case 101: mRetReason = "Switching Protocols"; break;
		// Successful 2xx
		case 200: mRetReason = "OK";          break;
		case 201: mRetReason = "Created";     break;
		case 204: mRetReason = "No Content";  break;
		case 202: mRetReason = "Accepted";    break;
		// Redirection 3xx
		case 301: mRetReason = "Moved Permanently";   break;
		case 302: mRetReason = "Found";       break;
		// Client Error 4xx
		case 400: mRetReason = "Bad Request"; break;
		case 403: mRetReason = "Forbidden";   break;
		case 404: mRetReason = "Not Found";   break;
		case 405: mRetReason = "Method Not Allowed";  break;
		// Server Error 5xx
		case 500: mRetReason = "Internal Server Error"; break;
		case 501: mRetReason = "Not Implemented";       break;
		case 502: mRetReason = "Bad Gateway";           break;
		case 503: mRetReason = "Service Unavailable";   break;
		case 504: mRetReason = "Gateway Timeout";       break;
		case 505: mRetReason = "Version Not Supported"; break;
	}
}

/**
 * @brief Get the full header
 *
 * @return String The full header text, with status and all parameters
 */
String ResponseHeader::getHeader(void)
{
	String h;
	
	if (mRetCode != 200)
	{
		h.append("Status: ");
		h += String::number(mRetCode) + " ";
		h += mRetReason + "\n";
	}
	
	if (mContentType != "")
		h += "Content-type: " + mContentType + "\n";
	
	std::map<String, String>::iterator it;
	for (it = mHeaders.begin(); it != mHeaders.end(); ++it)
	{
		h += it->first +": " + it->second + "\n";
	}
	
	// Add an empty line for the end of header
	h += "\n";
	
	return h;
}

} // namespace hermod
/* EOF */
