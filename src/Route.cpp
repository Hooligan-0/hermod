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
#include "Log.hpp"
#include "Route.hpp"

namespace hermod {

/**
 * @brief Default constructor for a Route
 *
 */
Route::Route(void)
{
	mTarget = 0;
	mUri.clear();
}

/**
 * @brief Default destructor for a "Route" object
 *
 */
Route::~Route()
{
	// Nothing to do :)
}

RouteTarget *Route::getTarget(void)
{
	return mTarget;
}

String &Route::getUri(void)
{
	return mUri;
}

void Route::setTarget(RouteTarget *target)
{
	mTarget = target;
}

void Route::setUri(const String &uri)
{
	mUri = uri;
}

// ------------------------- Friend operators -------------------------

/**
 * @brief Test if a Route match with an URL given into a String
 *
 * @param src Reference to a Route object  (left member of "==")
 * @param str Reference to a String object (right member of "==")
 * @return boolean True if the two strings are equal
 */
bool operator==(Route *route, const String &uri)
{
	int len = route->getUri().length();
	
	if (route->getUri() == uri.left(len))
		return true;

	return false;
}

} // namespace hermod
/* EOF */
