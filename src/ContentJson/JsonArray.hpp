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
#ifndef CONTENTJSON_ARRAY_HPP
#define CONTENTJSON_ARRAY_HPP
#include <vector>
#include "JsonElement.hpp"

namespace hermod {
	namespace contentJson {

/**
 * @class JsonArray
 * @brief Class to handle a JSON array
 *
 */
class JsonArray : public JsonElement {
public:
	JsonArray();
	~JsonArray();
	void add(JsonElement *item);
	void add(const std::string &value);
	void renderBody(void);
	void renderHead(void);
	void renderTail(void);
private:
	std::vector<JsonElement *>  mChildren;
};

	} // namespace contentJson
} // namespace hermod
#endif
