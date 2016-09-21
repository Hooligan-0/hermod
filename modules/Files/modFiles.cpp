/*
 * modFiles - An Hermod module to access static files
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
#include "modFiles.hpp"
#include "PageFile.hpp"

using namespace hermod;

extern "C" Module* create_object()
{
	hermod::Files::ModFiles *module;
	module = new hermod::Files::ModFiles;
	
	return module;
}

extern "C" void destroy_object(Module *module)
{
	delete module;
}

namespace hermod {
	namespace Files {

ModFiles::ModFiles()
  : Module()
{
	// Set a name for this module
	setName("Files");
}

void ModFiles::freePage(Page *page)
{
	if (page == NULL)
		return;
	delete page;
}

void ModFiles::initRouter(Router *router)
{
	RouteTarget *tgt;
	// Create a target for File page
	tgt = router->createTarget(this);
	tgt->setName("file");
	tgt->enable();
}

Page *ModFiles::newPage(const std::string &name)
{
	Page *page = NULL;
	
	if (name.compare("file") == 0)
		page = new PageFile();

	return page;
}
	} // namespace Files
} // namespace hermod
/* EOF */
