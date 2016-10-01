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
#ifndef CONTENTHTML_TAG_HPP
#define CONTENTHTML_TAG_HPP
#include <vector>
#include "HtmlElement.hpp"

namespace hermod {
	namespace contentHtml {

/**
 * @class HtmlTag
 * @brief Class to handle a "generic" HTML tag
 *
 * Many HTML tags use the same format with a opening and a closing element. This
 * class can be used to create an arbitrary tag : < tag >(some datas)< /tag >
 */
class HtmlTag : public HtmlElement {
public:
	explicit HtmlTag(const std::string &tag);
	void renderHead(void);
	void renderTail(void);
protected:
	std::string mTagName;
};

	} // namespace ContentHtml
} // namespace hermod
#endif
