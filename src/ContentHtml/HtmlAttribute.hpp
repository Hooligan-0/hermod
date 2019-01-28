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
#ifndef CONTENTHTML_ATTRIBUTE_HPP
#define CONTENTHTML_ATTRIBUTE_HPP
#include <vector>
#include "../String.hpp"

namespace hermod {
	namespace contentHtml {

/**
 * @class HtmlAttribute
 *
 * @brief This class is used to define an attribute for an Html element
 */
class HtmlAttribute {
public:
	HtmlAttribute();
	HtmlAttribute(const String &name);
	String &getName();
	String &getValue();
	void    setName(const String &name);
	void    setValue(const String &value);
protected:
	String mName;
	String mValue;
};

	} // namespace ContentHtml
} // namespace hermod
#endif
