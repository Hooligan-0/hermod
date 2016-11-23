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

RouteTarget::RouteTarget(void)
{
	mValid  = false;
	mName.clear();
	mModule = NULL;
}

void RouteTarget::disable(void)
{
	mValid = false;
}

void RouteTarget::enable(void)
{
	mValid = true;
}

void RouteTarget::freePage(Page *page)
{
	// Sanity check
	if ( (page == NULL) || (mModule == NULL) )
		return;
	
	mModule->freePage(page);
}

Module *RouteTarget::getModule(void)
{
	return mModule;
}

const String &RouteTarget::getName(void)
{
	return mName;
}

bool RouteTarget::isEnabled(void)
{
	return mValid;
}

Page *RouteTarget::newPage(void)
{
	// Sanity check
	if (mModule == NULL)
		return NULL;
	if (mName.isEmpty())
		return NULL;
	
	return mModule->newPage(mName);
}

void RouteTarget::setModule(Module *module)
{
	mModule = module;
}

void RouteTarget::setName(const String &name)
{
	mName = name;
}

} // namespace hermod
/* EOF */
