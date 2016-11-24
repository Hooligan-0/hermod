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
 * This String class offer high level methods to create, convert and compare
 * text strings. It is a replacement os the standard std::string that does not
 * offer enough methods for hermod needs.
 */
class String
{
public:
	String();
	String(const String &src);
	String(const char *src);
	String(const std::string &src);
	~String();
	String     &append   (const String &src);
	String     &append   (const char   *src);
	void        clear    (void);
	char       *data     (void) const;
	int         indexOf  (char c, int from = 0) const;
	bool        isEmpty  (void) const;
	int         lastIndexOf(char c, int from = -1) const;
	String      left     (unsigned int n) const;
	size_t      length   (void) const;
	String      mid      (int pos, int n = -1) const;
	String     &remove   (int pos, int n);
	void        reserve  (unsigned int size);
	String      right    (unsigned int n) const;
	size_t      size     (void) const;
	String      toBase64 (void) const;
	std::string toStdStr (void) const;
	int         toInt    (void) const;
	void        truncate (unsigned int pos);
	void        urlDecode(void);
public:
	static String hex(unsigned char *src, int len);
	static String number(unsigned long);
protected:
	void   copy(char *src, int len);
	void   copy(const String &src);
	void   realloc(size_t len);
	void   setLength(size_t len);
public:
	String & operator=(const String & src);
	String & operator=(const char *src);
	String & operator=(const std::string &src);
	String & operator+ (const char *src);
	String & operator+=(const String &src);
	String & operator+=(const char   *src);
	operator char*() const
	{
		return mBuffer;
	}
	operator std::string() const
	{
		if (mBuffer)
			return std::string(mBuffer);
		else
			return std::string("");
	}
public:
	friend bool          operator==(const String&, const String&);
	friend bool          operator==(const String&, const char *);
	friend bool          operator!=(const String&, const String&);
	friend bool          operator!=(const String&, const char *);
	friend std::ostream& operator<<(std::ostream&, const String &);
	friend bool          operator< (String const &a, String const &b);
	friend String        operator+ (String const &a, const char *b);
private:
	char  *mBuffer;
	size_t mLength;
	size_t mSize;
};

} // namespace hermod
#endif
