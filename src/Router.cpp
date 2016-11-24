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
#include "Config.hpp"
#include "Log.hpp"
#include "Router.hpp"
#include "Request.hpp"

namespace hermod {

/**
 * @brief Default constructor for a Router
 *
 */
Router::Router(void)
{
	mTargets.clear();
}

/**
 * @brief Default destructor for a "Router" object
 *
 */
Router::~Router()
{
	// Clear the target cache
	while(mTargets.size())
	{
		RouteTarget *target = mTargets.back();
		mTargets.pop_back();
		delete target;
	}
}

/**
 * @brief This method create a new RouteTarget and register it into router
 *
 * @param module Pointer to the module that request a new route target
 * @return RouteTarget* Pointer the the newly allocated target.
 */
RouteTarget *Router::createTarget(Module *module)
{
	RouteTarget *target = new RouteTarget;
	target->setModule(module);
	mTargets.push_back(target);
	
	return target;
}

/**
 * @brief Find a route based on an URI or a system name
 *
 * @param uri Reference to the URI (or name) to find
 * @return RouteTarget* Pointer to a route if URI is found
 */
RouteTarget *Router::find(const String &uri)
{
	RouteTarget *target = 0;

	try {
		ConfigKey *cfgRoute = findConfigRoute(uri);
		// If no valid route found :( Nothing more to do
		if (cfgRoute == 0)
			return NULL;

		// Find a target for this route
		target = this->findTarget(cfgRoute->getValue());
		if (target == NULL)
		{
			Log::warning() << "Router: No loaded module or page match route ";
			Log::warning() << cfgRoute->getValue() << Log::endl;
		}
	} catch (std::exception &e) {
		Log::error() << "Router error: " << e.what() << Log::endl;
		return NULL;
	}

	return target;
}

/**
 * @brief Find a route based on a Request
 *
 * @param r Pointer to the source Request
 * @return RouteTarget* Pointer to a route if URI is found
 */
RouteTarget *Router::find(Request *r)
{
	String uri = r->getUri(0);
	if (uri.isEmpty())
		uri = ":index:";
	
	RouteTarget *target;

	try {
		// Find the route into config
		ConfigKey *cfgRoute = findConfigRoute(uri);
		// If no route found, end
		if (cfgRoute == 0)
			return 0;

		// Find a target for this route
		target = findTarget(cfgRoute->getValue());
		// If a valid target has been found
		if (target)
			// Update Request URI and args
			r->setUri( cfgRoute->getName() );
		else
		{
			Log::warning() << "Router: No loaded module or page match route ";
			Log::warning() << cfgRoute->getValue() << Log::endl;
		}

	} catch (std::exception &e) {
		Log::error() << "Router error: " << e.what() << Log::endl;
		return NULL;
	}

	return target;
}

/**
 * @brief Find into config a route that match an URI or a system name
 *
 * @param uri String of the requested URI or system name
 * @return ConfigKey* Pointer to the config key (or NULL)
 */
ConfigKey *Router::findConfigRoute(const String &uri)
{
	Config    *cfg = Config::getInstance();
	ConfigKey *key = 0;

	try {
		for(int i = 0; ;++i)
		{
			key = cfg->getKey("route", i);
			if (key == 0)
				break;
			
			String routeUri = key->getName();
			if (routeUri == uri.left(routeUri.length()) )
				break;
		}
	} catch (std::exception &e) {
		Log::info() << "Router error: " << e.what() << Log::endl;
		return NULL;
	}
	return key;
}

/**
 * @brief Find a target for a "page" into a "module"
 *
 * @param module String of the module name
 * @param page   String of the page name
 * @return RouteTarget* Pointer to the target that define the requested page
 */
RouteTarget *Router::findTarget(const String &module,
                                const String &page)
{
	RouteTarget *target = 0;
	try {
		// Parse all entries of RouteTarget cache
		std::vector<RouteTarget *>::iterator it;
		for (it = mTargets.begin(); it != mTargets.end(); ++it)
		{
			// Test if the target is valid/enabled
			if ( ! (*it)->isEnabled() )
				continue;
			// Test if Module name match
			if (module != (*it)->getModule()->getName())
				continue;
			// Test if page name match
			if (page != (*it)->getName())
				continue;
			// Yes :) A valid RouteTarget has been found
			target = *it;
			break;
		}
	} catch (std::exception &e) {
		Log::info() << "Router error: " << e.what() << Log::endl;
		return NULL;
	}

	return target;
}

/**
 * @brief Find a target for a "module:page" encoded name
 *
 * @param pair String of the "module:page" request
 * @return RouteTarget* Pointer to the target that define the requested page
 */
RouteTarget *Router::findTarget(const String &pair)
{
	RouteTarget *target = 0;
	try {
		// Split rule string to find module name and page name
		int sepPos = pair.indexOf(':');
		if (sepPos < 0)
			throw std::runtime_error("Malformed target pair name");
		// Extract name of the module from rule
		String modName  = pair.left(sepPos);
		// Extract name of the page from rule
		String pageName = pair.right(pair.length() - sepPos - 1);

		target = this->findTarget(modName, pageName);

	} catch (std::exception &e) {
		Log::info() << "Router error: " << e.what() << Log::endl;
		return NULL;
	}

	return target;
}

/**
 * @brief This method remove a route-target from the router cache
 *
 * @param target Pointer to the target to remove
 */
void Router::removeTarget(RouteTarget *target)
{
	std::vector<RouteTarget *>::iterator it;
	for (it = mTargets.begin(); it != mTargets.end(); ++it)
	{
		if ( (*it) != target )
			continue;
		RouteTarget *target = *it;
		// Remove the selected element from the list ...
		mTargets.erase(it);
		// ... then, delete it
		delete target;
		
		// Element has been removed, nothing more to do
		break;
	}
}

} // namespace hermod
/* EOF */
