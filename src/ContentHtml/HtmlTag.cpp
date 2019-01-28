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
#include <string>
#include <sstream>
#include "HtmlTag.hpp"

namespace hermod {
	namespace contentHtml {

/**
 * @brief Constructor
 *
 * @param tag Name of the HTML tag
 */
HtmlTag::HtmlTag(const std::string &tag) : mTagName(tag)
{
	// Nothing to do here
}

/**
 * @brief Render the "head" of the tag
 *
 */
void HtmlTag::renderHead(void)
{
	std::ostringstream ss;
	ss << "<" << mTagName;
	if (mAttributes.size() > 0)
	{
		std::vector<HtmlAttribute *>::iterator it;
		for (it = mAttributes.begin(); it != mAttributes.end(); ++it)
		{
			// Insert a space separator before attribute
			ss << " ";
			// Insert attribute value "key=value"
			ss << (*it)->getName();
			ss << "=\"";
			ss << (*it)->getValue();
			ss << "\"";
		}
	}
	ss << ">";
	
	renderAppend(ss.str());
}

/**
 * @brief Render the "tail" of the tag
 *
 */
void HtmlTag::renderTail(void)
{
	std::ostringstream ss;
	ss << "</" << mTagName << ">";
	
	renderAppend(ss.str());
}

	} // namespace ContentHtml
} // namespace hermod
/* EOF */
