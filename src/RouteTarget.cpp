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
#include "RouteTarget.hpp"
#include "Module.hpp"
#include "Page.hpp"
#include "String.hpp"

namespace hermod {

/**
 * @brief Default constructor
 *
 */
RouteTarget::RouteTarget(void)
{
	mValid  = false;
	mModule = (Module *)0x00;
	mName.clear();
}

/**
 * @brief Set the current state of the target to "disabled"
 *
 * The 'disable' state is used to announce that the module that owns the target
 * page is currently not able (or doesn't want) to process the page.
 */
void RouteTarget::disable(void)
{
	mValid = false;
}

/**
 * @brief Set the current state to the target to "enable"
 *
 * When a target is enabled, the router can use it. After enabling, when a
 * request match a route, the target page can be called.
 */
void RouteTarget::enable(void)
{
	mValid = true;
}

/**
 * @brief Get the Module that contains the page of this target
 *
 * @return Module* Pointer to the module
 */
Module *RouteTarget::getModule(void)
{
	return mModule;
}

/**
 * @brief Get the name of the target page
 *
 * @return String Name of the page into the module
 */
const String &RouteTarget::getName(void)
{
	return mName;
}

/**
 * @brief Test the current state of the target (enabled ... or not)
 *
 * @return boolean True if the target is enabled
 */
bool RouteTarget::isEnabled(void)
{
	return mValid;
}

/**
 * @brief Set the Module associated with this target
 *
 * @param Module* Pointer to the Module to use
 */
void RouteTarget::setModule(Module *module)
{
	mModule = module;
}

/**
 * @brief Set the name of the page for this target
 *
 * @param name Name of the page into the module
 */
void RouteTarget::setName(const String &name)
{
	mName = name;
}

} // namespace hermod
/* EOF */
