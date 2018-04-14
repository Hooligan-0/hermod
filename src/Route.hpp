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
#ifndef ROUTE_HPP
#define ROUTE_HPP

#include <vector>
#include "RouteTarget.hpp"
#include "String.hpp"

namespace hermod {

class Request;

/**
 * @class Route
 * @brief The route class define how to handle a specific URI
 *
 */
class Route
{
public:
	Route (void);
	~Route();
	RouteTarget *getTarget(void);
	String      &getUri(void);
	void    setTarget(RouteTarget *target);
	void    setUri   (const String &uri);
public:
	friend bool operator==(Route *, const String&);

private:
	RouteTarget *mTarget;
	String       mUri;
};

} // namespace hermod
#endif
