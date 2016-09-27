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
#include <string>
#include "PageHelloJson.hpp"
#include "ContentJson/JsonObject.hpp"
#include "ContentJson/JsonString.hpp"

namespace hermod {
	namespace Dummy {

/**
 * @brief Constructor of the page
 *
 */
PageHelloJson::PageHelloJson(void)
    : Page()
{
	// Nothing to do
}

/**
 * @brief This method is called when a request for this page is received
 *
 */
int PageHelloJson::process(void)
{
	// Use the Page layer to init this document as JSON
	ContentJson *content = initContentJson();
	
	// Create the "Hello World" message
	std::string msg("Hello ");
	if (getArgCount() == 0)
		msg += "World";
	else
		msg += getArg(1).toStdStr();
	msg += " !";
	
	contentJson::JsonObject *obj = new contentJson::JsonObject;
	content->setRoot(obj);

	contentJson::JsonString *str = new contentJson::JsonString(msg);
	obj->add("message", str);

	if (getArgCount() > 0)
	{
		contentJson::JsonString *strName = new contentJson::JsonString(getArg(1));
		obj->add("your_name", strName);
	}
	
	return(0);
}

	} // namespace Dummy
} // namespace hermod
/* EOF */
