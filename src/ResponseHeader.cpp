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
