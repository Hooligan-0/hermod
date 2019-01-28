/*
 * Hermod - Modular application framework
 *
 * Copyright (c) 2016-2019 Cowlab
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
#include "HtmlAttribute.hpp"

namespace hermod {
	namespace contentHtml {

/**
 * @brief Default constructor
 *
 */
HtmlAttribute::HtmlAttribute()
{
	mName.clear();
	mValue.clear();
}

/**
 * @brief Constructor with name definition
 *
 * @param name Name of the attribute
 */
HtmlAttribute::HtmlAttribute(const String &name)
{
	mName = name;
}

/**
 * @brief Get the current name of the attribute
 *
 * @return String Attribute name
 */
String &HtmlAttribute::getName()
{
	return mName;
}

/**
 * @brief Gte the current value of the attribute
 *
 * @return String Attribute value
 */
String &HtmlAttribute::getValue()
{
	return mValue;
}

/**
 * @brief Set the name of the attribute
 *
 * @param name New name to set for the attribute
 */
void HtmlAttribute::setName(const String &name)
{
	mName = name;
}

/**
 * @brief Set the value of the attribute
 *
 * @param value New value to set for the attribute
 */
void HtmlAttribute::setValue(const String &value)
{
	mValue = value;
}

	} // namespace ContentHtml
} // namespace hermod
/* EOF */
