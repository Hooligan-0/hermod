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
#include <stdexcept>
#include "Request.hpp"
#include "Log.hpp"
#include "String.hpp"

using namespace std;

namespace hermod {

/**
 * @brief Default constructor
 *
 */
Request::Request(Server *server)
{
	mBody   = 0;
	mServer = server;
	mMethod = Undef;

	mUri.clear();
}

/**
 * @brief Default destructor
 *
 */
Request::~Request()
{
	if (mBody)
	{
		delete mBody;
		mBody = 0;
	}
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

/**
 * @brief Get the value of a cookie
 *
 * @param name Name of the requested cookie
 * @param allowEmpty Boolean to aloow (or not) empty result
 * @return string Value of the cookie
 */
String Request::getCookieByName(const String &name, bool allowEmpty = false)
{
	String value;

	try {
		String hc = getParam("HTTP_COOKIE");
		if (hc.isEmpty())
			throw runtime_error("No Cookie available");

		int pos = 0;
		while(pos >= 0)
		{
			String cookie;

			int startPos = pos;
			// Find the next cookie separator (if any)
			pos = hc.indexOf(';', pos);
			// Extract one cookie from list
			if (pos >= 0)
				cookie = hc.mid(startPos, pos - startPos);
			else
				cookie = hc.right(hc.length() - startPos);

			String cName;
			String cValue;

			// Find the name/value separator
			int vSep = cookie.indexOf('=');
			if (vSep >= 0)
			{
				// Extract cookie name
				cName  = cookie.left(vSep);
				if (cName[0] == ' ')
					cName.remove(0, 1);
				// Extract cookie value
				vSep++;
				cValue = cookie.right(cookie.length() - vSep);
			}

			if (cName == name)
			{
				value = cValue;
				break;
			}

			if (pos >= 0)
				pos++;
		}
	} catch (std::exception &e) {
		Log::debug() << "Request::getCookieByName " << name;
		Log::debug() << " : " << e.what() << Log::endl;
	}

	if ( value.isEmpty() && (allowEmpty == false) )
		throw runtime_error("Not Found");

	return value;
}

/**
 * @brief Get the content type of the request
 *
 * @return String Mime type of the request
 */
String Request::getContentType(void)
{
	String result;
	result = getParam("CONTENT_TYPE");

	// Search the parameter separator (if any)
	int sep = result.indexOf(';');
	// If a separator exists, cut the string to keep only typs/subtype
	if (sep)
		result.truncate(sep);

	return result;
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
	String value;

	std::map<String, String>::iterator it;
	it = mHeaderParameters.find(name);

	if (it != mHeaderParameters.end())
		value = mHeaderParameters[name];

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

/**
 * @brief Get page URI or optional argument
 *
 * @param n Position of the requested argument (0 for URI hitself)
 * @return string Value of the argument, or the URI
 */
String Request::getUri(unsigned int n = 0)
{
	String uri;
	
	if (n < mUri.size())
		uri = mUri.at(n);
	
	return uri;
}

/**
 * @brief Test if a FORM field with specified name has been received
 *
 * @param name Name of the searched field
 * @return boolean True if a field with given name exists
 **/
bool Request::hasFormValue(const String &name)
{
	// If the list of received variables is empty, refresh it
	if (mFormParameters.empty())
		loadFormInputs();

	std::map<String, String>::iterator it;
	it = mFormParameters.find(name);

	if (it != mFormParameters.end())
		return true;

	return false;
}

/**
 * @brief Test if a specific mime-type is delared into accept header
 *
 * @param  type String that contains the name of tested mime type
 * @return boolean True if the specified mime-type is found into Accept header
 */
bool Request::isAccept(const String &type)
{
	String acceptTypes = getParam("ACCEPT");
	if (acceptTypes.isEmpty())
		return false;

	// Find the separator for parameters
	int pSep = acceptTypes.indexOf(';');
	// If found, cut the string to ignore additional parameters
	if (pSep >= 0)
		acceptTypes.truncate(pSep);

	// Split Accept string and test each types into
	int idxStart = 0;
	while (idxStart >= 0)
	{
		String item;
		// Search next separator character
		int idxSep = acceptTypes.indexOf(',', idxStart);
		// If a separator has been found
		if (idxSep > 0)
		{
			item = acceptTypes.mid(idxStart, idxSep - idxStart);
			// ToDo : remove spaces at the begining/end of string
			idxStart = (idxSep + 1);
		}
		// Else, it is te last item
		else
		{
			item = acceptTypes.right(acceptTypes.length() - idxStart);
			idxStart = -1;
		}
		// If this item is equal to searched type, good news :)
		if (item == type)
			return true;
	}
	return false;
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
		if (contentLength.isEmpty())
		{
			Log::error() << "Failed to load POSTed form values : CONTENT_LENGTH not found" << Log::endl;
			return;
		}
		// Convert content length to a numeric value
		int len = contentLength.toInt();
		if (len == 0)
			return;
		// Arbitraty upper limit ... must be improved in future
		if (len > (1024 * 1024))
			return;
		// Get datas
		if ( (mBody == 0) || mBody->isEmpty())
			return;
		char *pnt   = mBody->data();
		char *token = mBody->data();

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

/**
 * @brief Set the Request body
 *
 * @param body Pointer to a buffer (String) that contain body content
 */
void Request::setBody(String *body)
{
	// If a body has already been set ... delete it
	if (mBody)
		delete mBody;

	// Copy pointer to body buffer (and get ownership)
	mBody = body;
}

/**
 * @brief Insert (or update) an HTTP header parameter
 *
 * @param name  String that contain parameter name
 * @param value String that contain parameter value
 */
void Request::setHeaderParameter(const String &name, const String &value)
{
	mHeaderParameters[ name ] = value;

	if (name == "QUERY_STRING")
		mUri.push_back(value);
}

/**
 * @brief Update the URI / arguments split position according to the route
 *
 * @param route Reference to the route URI
 */
void Request::setUri(const String &route)
{
	if (mUri.size() == 0)
		return;
	
	try {
		// Get full URI string
		String args = getParam("QUERY_STRING");
		if (args.isEmpty())
			throw runtime_error("Query string empty (or not found)");
		// Keep only the tail (where args are)
		args.remove(0, route.length());
		if (args.length())
		{
			if (args[0] == '/')
				args.remove(0, 1);
		}

		// Clear the current URI argument array
		mUri.clear();
		// Set the requested route as arg(0)
		mUri.push_back(route);

		// If the requested URI contains args after the route hitself
		if (args.length() > 0)
		{
			// Split arg string into an array of args
			for (int pos = 0; pos >= 0; )
			{
				String token;
				int start = pos;
				// Find the next token separator
				pos = args.indexOf('/', pos);
				if (pos >= 0)
				{
					token = args.mid(start, pos - start);
					pos++;
				}
				else
				{
					token = args.right(args.length() - start);
				}
				// Save the item
				mUri.push_back(token);
			}
		}

	} catch (std::exception &e) {
		Log::error() << "setUri failed "
		             << e.what() << Log::endl;
	}
}

} // namespace hermod
/* EOF */
