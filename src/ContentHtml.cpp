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
#include "ContentHtml.hpp"
#include "ContentHtml/HtmlElement.hpp"
#include "ContentHtml/HtmlHtml.hpp"
#include "Log.hpp"

namespace hermod {

using namespace contentHtml;

/**
 * @brief Default constructor
 *
 */
ContentHtml::ContentHtml()
  : Content()
{
	mValidBuffer = false;
	mRootElement = new contentHtml::HtmlHtml();
	mTemplate    = 0;
}

/**
 * @brief Default destructor
 *
 */
ContentHtml::~ContentHtml()
{
	clean();
}

/**
 * @brief Clean the current content (remove root element and/or template)
 *
 */
void ContentHtml::clean(void)
{
	// Clean html elements (if any)
	if (mRootElement)
	{
		delete mRootElement;
		mRootElement = 0;
	}
	// Clean template (if any)
	if (mTemplate)
	{
		delete mTemplate;
		mTemplate = 0;
	}
}

/**
 * @brief Request a reload/refresh of the internal buffer
 *
 */
void ContentHtml::refresh(void)
{
	// Clear the current content of buffer
	mBuffer.clear();
	// If the content contains html elements
	if (mRootElement)
	{
		// Configure root element to write into internal buffer
		mRootElement->setRenderBuffer(&mBuffer);
		// Request root element to render itself
		mRootElement->render();
	}
	// If the content is based on a template
	else if (mTemplate)
	{
		mTemplate->render( &mBuffer );
	}

	// The internal buffer content is now up-to-date
	mValidBuffer = true;
}

/**
 * @brief Get access to the root Element of this content
 *
 * @return HtmlElement* Pointer to the root element
 */
HtmlElement *ContentHtml::root(void)
{
	if (mRootElement == 0)
		throw -1;

	return mRootElement;
}

/**
 * @brief Set a template as the new root of this content
 *
 * @param tpl Pointer to the Template to use
 */
void ContentHtml::setTemplate(Template *tpl)
{
	// Clean all existing element and template
	clean();
	// Set the new root template
	mTemplate = tpl;
}

/**
 * @brief Get access to the root template of this content
 *
 * @return Template* Pointer to the root template
 */
Template *ContentHtml::tpl(void)
{
	if (mTemplate == 0)
		throw -1;

	return mTemplate;
}

} // namespace hermod
/* EOF */
