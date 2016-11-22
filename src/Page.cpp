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
#include <string>
#include "Config.hpp"
#include "Log.hpp"
#include "Page.hpp"
#include "Request.hpp"
#include "Session.hpp"
#include "SessionCache.hpp"

namespace hermod {

/**
 * @brief Default constructor
 *
 */
Page::Page(void)
{
	mRequest  = NULL;
	mResponse = NULL;
	mSession  = NULL;
	mUseSession = false;
}

/**
 * @brief Default destructor
 *
 */
Page::~Page(void)
{
	// Nothing to do but needed for derivated classes
}

/**
 * @brief Get Uri (or argument) of the request
 *
 * This method allow to get a part of the original URI, after analysis by
 * router. The first item (n=0) contains the route, and other items (n>0)
 * contains other words of the URI separated by '/'. This method is just a
 * wrapper to Request object, and can be overloaded by specific page.
 *
 * @param n Position of the requested element (0 for uri)
 * @return String Value of the requested element
 */
String Page::getArg(int n)
{
	return mRequest->getUri(n);
}

/**
 * @brief Get the number of arguments found into the requested URI
 *
 * This method is just a wrapper to countUriArs() of Request object. Using this
 * method instead of original is usefull when overload by a specific page.
 *
 * @return integer Number of arguments into URI
 */
int Page::getArgCount(void)
{
	return mRequest->countUriArgs();
}

/**
 * @brief Create a generic Content and insert it into Page Response
 *
 * @return Content* Pointer to the newly allocated Content
 */
Content *Page::initContent(void)
{
	if ( ! mResponse)
		return NULL;
	
	Content *content = new Content();
	if ( ! content)
		throw runtime_error("Failed to allocate Content");
	
	mResponse->setContent(content);
	
	return content;
}

/**
 * @brief Allocate an HTML content and define it as response content
 *
 * @return ContentHtml* Pointer to the newly allocated HTML content
 */
ContentHtml *Page::initContentHtml(void)
{
	if ( ! mResponse)
		return NULL;
	
	ContentHtml *content = new ContentHtml();
	if ( ! content)
		throw runtime_error("Failed to allocate ContentHtml");
	
	mResponse->setContent(content);
	mResponse->header()->setContentType("text/html");
	
	return content;
}

/**
 * @brief Allocate a JSON content and define it as response content
 *
 * @return ContentJson* Pointer to the newly allocated JSON content
 */
ContentJson *Page::initContentJson(void)
{
	if ( ! mResponse)
		return NULL;
	
	ContentJson *content = new ContentJson();
	if ( ! content)
		throw runtime_error("Failed to allocate ContentJson");
	
	mResponse->setContent(content);
	mResponse->header()->setContentType("application/json");
	
	return content;
}

/**
 * @brief Start the session layer for the current request
 *
 */
void Page::initSession(int mode)
{
	// Test if this page require session
	if ((mode == 0) && ( ! mUseSession))
		return;
	// Test if session already init
	if (mSession)
		return;
	
	Config       *cfg  = Config::getInstance();
	SessionCache *sc   = SessionCache::getInstance();
	Session      *sess = NULL;
	std::string   sessId;
	std::string   cookieName;
	
	if (mode == 0)
	{
		String cfgSessionMode( cfg->get("global", "session_mode") );
		if (cfgSessionMode == "cookie")
			mode = 1;
		else if (cfgSessionMode == "token")
			mode = 2;
		else
		{
			Log::info() << "Page: Could not load session, unknown mode ";
			Log::info() << cfgSessionMode << Log::endl;
			return;
		}
		Log::debug() << "Page::initSession config mode " << mode << Log::endl;
	}

	loadSession(mode);

	if ( ! mSession)
	{
		try {
			sess = sc->create();
			if (sess == NULL)
				throw runtime_error("Failed to create a new session");
			Log::debug() << "Page::initSession create session " << sess->getId() << Log::endl;
			if (mode == 1)
			{
				std::string cookie( cookieName );
				cookie += "=" + sess->getId();
				mResponse->header()->addHeader("Set-Cookie", cookie);
				Log::debug() << "Page: create session " << sess->getId() << Log::endl;
			}
			mSession = sess;
		} catch (...) {
		}
	}
}

/**
 * @brief Load the session layer for the current request
 *
 */
void Page::loadSession(int mode)
{
	Config       *cfg  = Config::getInstance();
	SessionCache *sc   = SessionCache::getInstance();
	Session      *sess = NULL;

	if (mode == 1)
	{
		// Find Session ID using cookie
		try {
			// Try to get the cookie name from config
			String cookieName( cfg->get("global", "session_cookie") );
			// If this parameter is absent
			if (cookieName.isEmpty())
				cookieName = "HERMOD_SESSION";
			String sessId = mRequest->getCookieByName(cookieName, false);
			sess = sc->getById(sessId.toStdStr());
			if (sess == 0)
			{
				Log::debug() << "Page: try to load an unknown session " << sessId << Log::endl;
				throw -1;
			}
			Log::debug() << "Page: load session " << sessId << Log::endl;
			mSession = sess;
		} catch (std::exception &e) {
			Log::error() << "Page: Session error : " << e.what() << Log::endl;
		} catch (...) {
			Log::error() << "Page: Session unknown error" << Log::endl;
		}
	}
	// Load Session using token
	else if (mode == 2)
	{
		try {
			String token( request()->getFormValue("token") );
			if (token.isEmpty())
				throw -2;
			sess = sc->getById(token);
			if (sess == 0)
			{
				Log::debug() << "Page: try to load an unknown session " << token << Log::endl;
				throw -1;
			}
			mSession = sess;
		} catch (...) {
		}
	}
}

/**
 * @brief Get access to the request
 *
 * @return Request* Pointer to the Request
 */
Request *Page::request(void)
{
	if (mRequest == 0)
		throw -1;

	return mRequest;
}

/**
 * @brief Get access to the Response object
 *
 * @return Response* Pointer to the Response object of this page
 */
Response *Page::response(void)
{
	if (mResponse == 0)
		throw -1;

	return mResponse;
}

/**
 * @brief Get access to the session
 *
 * When sessions are used by a page, this method should be used by inherited
 * page classes to access it. If the Session is not already loaded this method
 * call initSession to load it ! Errors of initSession are not catch, so this
 * method can fail with an exception.
 *
 * @return Session* Pointer to the session
 */
Session *Page::session(void)
{
	if (mSession == 0)
	{
		initSession();
		if (mSession == 0)
			throw runtime_error("no session");
	}

	return mSession;
}

/**
 * @brief Set the request associated with this page
 *
 * @param obj Pointer to the source request
 */
void Page::setRequest(Request *obj)
{
	mRequest = obj;
}

/**
 * @brief Set the response object that will contains the page result
 *
 * @param obj Pointer to a response object
 */
void Page::setReponse(Response *obj)
{
	mResponse = obj;
}

/**
 * @brief Test the session flag to check if this page must use a session
 *
 * @return boolean True if a Session if needed by this page
 */
bool Page::useSession(void)
{
	return mUseSession;
}

/**
 * @brief Set the session flag to define if this page need a session
 *
 * @param use Boolean value, true if a Seesion is needed
 */
void Page::useSession(bool use)
{
	mUseSession = use;
}

} // namespace hermod
/* EOF */
