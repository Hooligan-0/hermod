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
#include "PageHello.hpp"
#include "ContentHtml/HtmlHtml.hpp"
#include "ContentHtml/HtmlH.hpp"
#include "ContentHtml/HtmlTag.hpp"

namespace hermod {
	namespace Dummy {

/**
 * @brief Constructor of the page
 *
 */
PageHello::PageHello(void)
    : Page()
{
	// Nothing to do
}

/**
 * @brief This method is called when a request for this page is received
 *
 */
int PageHello::process(void)
{
	// Use the Page layer to init this document as HTML page
	ContentHtml *content = initContentHtml();
	
	// Create the "Hello World" message
	std::string msg("Hello ");
	if (getArgCount() == 0)
		msg += "World";
	else
		msg += getArg(1).toStdStr();
	msg += " !";
	
	// Create an HTML "h1" tag
	contentHtml::HtmlH *h1 = new contentHtml::HtmlH(1);
	// Insert the Hello message to the tag
	h1->add(msg);
	// Insert the h1 tag to the response body
	content->root()->add(h1);
	
	return(0);
}

	} // namespace Dummy
} // namespace hermod
/* EOF */
