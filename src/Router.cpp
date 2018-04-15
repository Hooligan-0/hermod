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
	mModules = 0;
	mRoutes.clear();
	mTargets.clear();
}

/**
 * @brief Default destructor for a "Router" object
 *
 */
Router::~Router()
{
	clean();
}

/**
 * @brief Remove all the routes and targets loaded into the Router
 *
 */
void Router::clean(void)
{
	// Clear the route(s) cache
	while(mRoutes.size())
	{
		Route *r = mRoutes.back();
		mRoutes.pop_back();
		delete r;
	}
	// Clear the target cache
	while(mTargets.size())
	{
		RouteTarget *target = mTargets.back();
		mTargets.pop_back();
		delete target;
	}
}

/**
 * @brief This method create a new Route and register it into router
 *
 * @param  uri    String that contains the URI
 * @param  target Pointer to the RouteTarget to use for requests on this route
 * @return Route* Pointer the the newly allocated Route.
 */
Route *Router::createRoute(String &uri, RouteTarget *target)
{
	Route *route;

	// Instanciate a ne Route object
	route = new Route();
	// Configure it
	route->setUri(uri);
	route->setTarget(target);

	// Register this Route into local Router cache
	mRoutes.push_back(route);

	return route;
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
 * @brief This method create a new named RouteTarget and register it into router
 *
 * @param module Pointer to the module that request a new route target
 * @param name   Reference to a String that contains the name of the new target
 * @param en     Define if the new target is enabled (or not)
 * @return RouteTarget* Pointer the the newly allocated target.
 */
RouteTarget *Router::createTarget(Module *module, const String &name, bool en)
{
	RouteTarget *target = new RouteTarget;
	target->setModule(module);
	target->setName(name);
	if (en)
		target->enable();
	else
		target->disable();
	mTargets.push_back(target);

	return target;
}

/**
 * @brief Find a route based on an URI or a system name
 *
 * @param  uri Reference to the URI (or name) to find
 * @return Route* Pointer to a route if URI is found
 */
Route *Router::find(const String &uri)
{
	Route *route = 0;

	try {
		// Parse all entries of Route cache
		std::vector<Route *>::iterator it;
		for (it = mRoutes.begin(); it != mRoutes.end(); ++it)
		{
			if (! ((*it) == uri))
				continue;
			route = (*it);
			break;
		}
		if (route == NULL)
		{
			Log::warning() << "Router: No loaded module or page match route "
			               << uri << Log::endl;
		}
	} catch (std::exception &e) {
		Log::error() << "Router error: " << e.what() << Log::endl;
		return NULL;
	}

	return route;
}

/**
 * @brief Find a route based on a Request
 *
 * @param  r      Pointer to the source Request
 * @return Route* Pointer to a route if URI is found
 */
Route *Router::find(Request *r)
{
	String uri = r->getUri(0);
	if (uri.isEmpty())
		uri = ":index:";
	
	Route *route = 0;

	try {
		route = find(uri);

		// If a valid target has been found
		if (route)
			// Update Request URI and args
			r->setUri( route->getUri() );
	} catch (std::exception &e) {
		Log::error() << "Router error: " << e.what() << Log::endl;
		return NULL;
	}

	return route;
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

/**
 * @brief Load (or reload) list of available Route(s)
 *
 */
void Router::reload(void)
{
	// Clear the current router config (if any)
	clean();

	// First, reload routes and Target defined by Modules
	try {
		if (!mModules)
			throw std::runtime_error("Module cache not available");

		// Search all available modules
		for (int i = 0; i < mModules->count(); ++i)
		{
			Module *mod;
			// Get access to next module
			mod = mModules->get(i);
			if (!mod)
				continue;
			// Call initRouter, the module will reload his rules by itself
			mod->initRouter(this);
		}
	} catch (std::exception &e) {
		Log::info() << "Router: Failed to (re)load routes: " << e.what() << Log::endl;
	}

	// Second, reload routes defined into config file

	Config    *cfg = Config::getInstance();

	try {
		ConfigKey   *key    = 0;
		RouteTarget *target = 0;
		Route       *route  = 0;

		for(int i = 0; ;++i)
		{
			key = cfg->getKey("route", i);
			if (key == 0)
				break;

			String routeUri = key->getName();

			// Find a target for this route
			target = this->findTarget( key->getValue() );
			if (target == 0)
			{
				Log::warning() << "Router: Failed to (re)load config for URI "
				               << routeUri << Log::endl;
				continue;
			}

			route = createRoute(routeUri, target);
			if (route == 0)
				throw std::runtime_error("Failed to create route");
		}
	} catch (std::exception &e) {
		Log::info() << "Router error: " << e.what() << Log::endl;
	}
}

/**
 * @brief Define the ModuleCache to use when search/access Modules
 *
 * For some operations, Router need to access Modules directly. For example when
 * the list of routes is loaded/reloaded, Router will call each module to allow
 * them to insert they own rules. This method set the cache that contains the
 * list of available modules.
 *
 * @param mc Pointer to the ModuleCache to use
 */
void Router::setModuleCache(ModuleCache *mc)
{
	mModules = mc;
}

} // namespace hermod
/* EOF */
