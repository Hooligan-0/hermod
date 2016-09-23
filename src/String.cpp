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
#include <cstdlib>
#include "String.hpp"
#include <iostream>

namespace hermod {

/**
 * @brief Default constructor
 *
 */
String::String()
{
	mBuffer = 0;
	mLength = 0;
	mSize   = 0;
}

/**
 * @brief Constructor with copy from an existing String
 *
 * @param str Source string to copy
 */
String::String(const String &src)
{
	mBuffer = 0;
	mLength = 0;
	mSize   = 0;
	
	copy(src);
}

/**
 * @brief Constructor with copy from a null terminated c-string
 *
 * @param src Source c-string to copy
 */
String::String(const char *src)
{
	mBuffer = 0;
	mLength = 0;
	mSize   = 0;
	
	// Compute string length
	size_t len = 0;
	char *p = (char *)src;
	while(*p)
		p++;
	len = (p - src);
	// Copy the string
	copy((char *)src, len);
}

/**
 * @brief Constructor with copy from an std::string
 *
 * @param src Source std::string to copy
 */
String::String(const std::string &src)
{
	mBuffer = 0;
	mLength = 0;
	mSize   = 0;
	// Copy the content of the std:string
	copy((char *)src.c_str(), src.size());
}

/**
 * @brief Default destructor
 *
 */
String::~String()
{
	if (mBuffer)
		free(mBuffer);
}

/**
 * @brief Delete the current content of the string
 *
 */
void String::clear(void)
{
	if (mBuffer)
	{
		free(mBuffer);
		mBuffer = 0;
		mLength = 0;
		mSize   = 0;
	}
}

/**
 * @brief Copy content of another string into this one
 *
 * @param src Pointer to the source c-string
 * @param len Length of the string to copy
 */
void String::copy(char *src, int len)
{
	if (len == 0)
	{
		clear();
		return;
	}

	// Allocate memory
	realloc(len + 1);

	char *pDest = mBuffer;

	// Copy buffer
	for (int i = 0; i < len; i++)
		*pDest++ = *src++;

	// Add a NULL byte to finish the string
	*pDest = 0;

	// Update current length;
	mLength = len;
}

/**
 * @brief Copy content of another string into this one
 *
 * @param src Reference to a source string
 */
void String::copy(String const &src)
{
	copy((char *)src.ptr(), src.size());
}

/**
 * @brief Get the length of the string (in bytes)
 *
 * @return size_t Number of bytes into the string
 */
size_t String::length(void) const
{
	return(mLength);
}

/**
 * @brief Allocate (or re-allocate) memory to hold the string content
 *
 * @param len Size of the needed memory (in bytes)
 */
void String::realloc(size_t len)
{
	// Free the current memory buffer
	if (mBuffer)
	{
		free(mBuffer);
		mBuffer = 0;
	}

	if (len == 0)
		return;

	// Allocate a new memory buffer
	mBuffer = (char *)malloc(len);
	
	if (mBuffer == 0)
		throw -1;
	mSize = len;
	
	if (mLength > mSize)
		mLength = mSize;
}

/**
 * @brief Get a direct access to internal data buffer
 *
 * @return char* Pointer to the internal data buffer
 */
const char *String::ptr(void) const
{
	return (const char *)mBuffer;
}

/**
 * @brief Get the size of the memory used for current content (in bytes)
 *
 * @return size_t Number of bytes into the string
 */
size_t String::size(void) const
{
	return mSize;
}

int String::toInt(void) const
{
	int result = 0;

	char *c = mBuffer;

	for (unsigned int i = 0; i < mLength; i++)
	{
		if ( (*c < '0') || (*c > '9') )
			break;
		result = (result * 10);
		result += (*c - '0');
		c++;
	}
	return result;
}

/**
 * @brief Assign a new value to the string, based on another string
 *
 * @param src Source string
 * @return String& Reference to the string hitself
 */
String & String::operator=(String & src)
{
	copy((char *)src.ptr(), src.size());
	
	return *this;
}

/**
 * @brief Assign a new value to the string, based on a c-string
 *
 * @param src Pointer to a null-terminated c-string
 * @return String& Reference to the string hitself
 */
String & String::operator=(const char *src)
{
	// Compute string length
	char *p = (char *)src;
	while(*p)
		p++;
	int srcLength = (p - src);

	copy((char *)src, srcLength);

	return *this;
}

/**
 * @brief Assign a new value to the string, based on an std::string
 *
 * @param src Reference to a std::string object
 * @return String& Reference to the string hitself
 */
String & String::operator=(const std::string &src)
{
	// Copy the content of the std:string
	copy((char *)src.c_str(), src.size());
	
	return *this;
}

// ------------------------- Friend operators -------------------------

/**
 * @brief Compare alphabetical values of two strings
 *
 * @param a Reference to the first tested string
 * @param b Reference to the second tested string
 * @return boolean True if 'b' is greater than 'a'
 */
bool operator<(String const& a, String const& b)
{
	const char *aStr = a.ptr();
	const char *bStr = b.ptr();

	while ((*aStr != 0) && (*bStr != 0))
	{
		if (*aStr == *bStr)
		{
			aStr++;
			bStr++;
			continue;
		}
		bool result = (*aStr < *bStr);
		return result;
	}
	return false;
}

/**
 * @brief Copy the string content to an ostream
 *
 * @param os Reference to the output ostream
 * @param obj Reference to the source string
 * @return ostream& Reference to the output ostream
 */
std::ostream& operator<< (std::ostream& os, const String& obj)
{
	if (obj.mBuffer == 0)
		return os;
	
	os.write(obj.mBuffer, obj.mLength);
	return os;
}

} // namespace hermod
/* EOF */