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
#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <vector>
#include "Config.hpp"
#include "RouteTarget.hpp"
#include "String.hpp"

namespace hermod {

class Module;
class Request;

/**
 * @class Router
 * @brief The router class is used to amange a collection of URI <-> page pair
 *
 */
class Router
{
public:
	Router (void);
	~Router();
	RouteTarget *createTarget(Module *module);
	RouteTarget *createTarget(Module *module, const String &name, bool en = true);
	void removeTarget(RouteTarget *target);
	RouteTarget *find(const String &uri);
	RouteTarget *find(Request *r);
protected:
	ConfigKey   *findConfigRoute(const String &uri);
	RouteTarget *findTarget(const String &module, const String &page);
	RouteTarget *findTarget(const String &pair);
private:
	std::vector<RouteTarget *> mTargets;
};

} // namespace hermod
#endif
