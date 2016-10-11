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
#include "JsonArray.hpp"
#include "JsonString.hpp"

namespace hermod {
	namespace contentJson {

/**
 * @brief Constructor
 *
 */
JsonArray::JsonArray()
{
	mChildren.clear();
}

/**
 * @brief Default destructor
 *
 */
JsonArray::~JsonArray()
{
	// Delete all child elements (if any)
	while( mChildren.size() )
	{
		JsonElement *e = mChildren.back();
		mChildren.pop_back();
		delete e;
	}
}

/**
 * @brief Insert a JSON element as an item of the array
 *
 * @param item Pointer to the element to add into child list
 */
void JsonArray::add(JsonElement *item)
{
	mChildren.push_back( item );
}

/**
 * @brief Insert a string value as an item of this array
 *
 * @param value String to add as a new array item
 */
void JsonArray::add(const std::string &value)
{
	JsonString *newItem = new contentJson::JsonString(value);
	mChildren.push_back( newItem );
}

/**
 * @brief Render members of JSON array
 *
 */
void JsonArray::renderBody(void)
{
	std::vector<JsonElement *>::iterator it;
	for (it = mChildren.begin(); it != mChildren.end(); ++it)
	{
		if (it != mChildren.begin())
			renderAppend(", ");
		
		// Insert object member value
		(*it)->setRenderBuffer(mRenderBuffer);
		(*it)->render();
	}
}

/**
 * @brief Render the "head" of the JSON array tag
 *
 */
void JsonArray::renderHead(void)
{
	std::string head("[");
	
	renderAppend(head);
}

/**
 * @brief Render the "tail" of the JSON array tag
 *
 */
void JsonArray::renderTail(void)
{
	std::string tail("]");
	
	renderAppend(tail);
}

	} // namespace contentJson
} // namespace hermod
/* EOF */
