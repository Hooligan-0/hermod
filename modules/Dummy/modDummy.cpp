/*
 * modDummy - An Hermod module for test and demo
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
#include "Module.hpp"
#include "Page.hpp"
#include "modDummy.hpp"
#include "Page404.hpp"
#include "PageHello.hpp"
#include "PageHelloJson.hpp"

using namespace hermod;

extern "C" Module* create_object()
{
	hermod::Dummy::ModDummy *module;
	module = new hermod::Dummy::ModDummy;
	
	return module;
}

extern "C" void destroy_object(Module *module)
{
	delete module;
}

namespace hermod {
	namespace Dummy {

ModDummy::ModDummy()
  : Module()
{
	// Set a name for this module
	setName("Dummy");
}

void ModDummy::freePage(Page *page)
{
	if (page == NULL)
		return;
	delete page;
}

void ModDummy::initRouter(Router *router)
{
	RouteTarget *tgt;
	// Create a target for the "Hello World" page
	tgt = router->createTarget(this);
	tgt->setName("hello");
	tgt->enable();
	// Create a target for the Json version of "Hello World" page
	tgt = router->createTarget(this);
	tgt->setName("hello_json");
	tgt->enable();
	// Create a target for the error 404 page
	tgt = router->createTarget(this);
	tgt->setName("err_404");
	tgt->enable();
}

Page *ModDummy::newPage(const std::string &name)
{
	Page *page = NULL;
	
	if (name.compare("err_404") == 0)
		page = new Page404();
	else if (name.compare("hello") == 0)
		page = new PageHello();
	else if (name.compare("hello_json") == 0)
		page = new PageHelloJson();
	
	return page;
}

	} // namespace Dummy
} // namespace hermod
/* EOF */
