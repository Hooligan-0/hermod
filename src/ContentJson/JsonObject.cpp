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
#include "JsonObject.hpp"
#include "JsonString.hpp"

namespace hermod {
	namespace contentJson {

/**
 * @brief Constructor
 *
 */
JsonObject::JsonObject()
{
	mChildren.clear();
}

/**
 * @brief Default destructor
 *
 */
JsonObject::~JsonObject()
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
 * @brief Insert a JSON element as child of this one
 *
 * @param name Name of this object member
 * @param item Pointer to the element to add into child list
 */
void JsonObject::add(const std::string &name, JsonElement *item)
{
	item->setName( name );
	mChildren.push_back( item );
}

/**
 * @brief Insert a string key/value as child of this json-object
 *
 * @param name Name of the new string member
 * @param value Value of the string member
 */
void JsonObject::add(const std::string &name, const std::string &value)
{
	JsonString *newItem = new contentJson::JsonString(value);
	newItem->setName( name );
	mChildren.push_back( newItem );
}

/**
 * @brief Render members of JSON object
 *
 */
void JsonObject::renderBody(void)
{
	std::vector<JsonElement *>::iterator it;
	for (it = mChildren.begin(); it != mChildren.end(); ++it)
	{
		std::ostringstream ss;
		
		if (it != mChildren.begin())
			ss << ", ";
		
		// Insert object member name
		ss << "\"" << (*it)->getName() << "\":";
		renderAppend( ss.str() );
		
		// Insert object member value
		(*it)->setRenderBuffer(mRenderBuffer);
		(*it)->render();
	}
}

/**
 * @brief Render the "head" of the JSON object tag
 *
 */
void JsonObject::renderHead(void)
{
	std::string head("{");
	
	renderAppend(head);
}

/**
 * @brief Render the "tail" of the JSON object tag
 *
 */
void JsonObject::renderTail(void)
{
	std::string tail("}");
	
	renderAppend(tail);
}

	} // namespace contentJson
} // namespace hermod
/* EOF */
