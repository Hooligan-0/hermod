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
#ifndef STRING_HPP
#define STRING_HPP
#include <cstddef>
#include <ostream>

namespace hermod {

/**
 * @class String
 * @brief This class handle text strings
 *
 */
class String
{
public:
	String();
	String(const String &src);
	String(const char *src);
	String(const std::string &src);
	~String();
	void   clear (void);
	char  *data (void) const;
	size_t length(void) const;
	void   reserve(unsigned int size);
	size_t size(void) const;
	int    toInt(void) const;
	void   urlDecode(void);
private:
	void   copy(char *src, int len);
	void   copy(const String &src);
	void   realloc(size_t len);
public:
	String & operator=(String & src);
	String & operator=(const char *src);
	String & operator=(const std::string &src);
	operator char*() { return mBuffer; }
	operator std::string()
	{
		if (mBuffer)
			return std::string(mBuffer);
		else
			return std::string("");
	}
public:
	friend bool          operator==(const String&, const char *);
	friend std::ostream& operator<<(std::ostream&, const String &);
	friend bool          operator< (String const &a, String const &b);
private:
	char  *mBuffer;
	size_t mLength;
	size_t mSize;
};

} // namespace hermod
#endif
