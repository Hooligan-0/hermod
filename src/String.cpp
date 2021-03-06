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

namespace hermod {

const static char base64Lookup[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

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
 * @param src Source string to copy
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

	// Sanity check
	if (src == 0)
		return;

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

	// If the source string is empty, nothing more to do
	if (src.length() == 0)
		return;

	// Copy the content of the std:string
	copy((char *)src.c_str(), src.length());
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
 * @brief Append a String at the end of the current one
 *
 * @param src Reference to the String to append
 * @return String& Reference to the object hitself
 */
String & String::append(const String &src)
{
	char *s;
	char *p;

	// If the source string is empty, nothing to do
	if (src.length() == 0)
		return *this;

	// Compute the new total length
	int len = (mLength + src.length());

	// Allocate a buffer to hold the new string
	char *newBuffer = (char *)malloc(len + 1);

	// Copy the current string to the new buffer
	if (mBuffer != 0)
	{
		p = newBuffer;
		s = mBuffer;
		for (unsigned int i = 0; i < mLength; i++)
		{
			*p = *s;
			s++;
			p++;
		}
	}
	else
		p = newBuffer;

	// Copy the additionnal part to the new buffer
	s = (char *)src.data();
	for (unsigned int i = 0; i < src.length(); i++)
	{
		*p = *s;
		s++;
		p++;
	}
	*p = 0;

	// If the string has already an internal buffer, free it
	if (mBuffer)
		free(mBuffer);
	// Set the newly allocated buffer as internal buffer
	mBuffer = newBuffer;
	mSize = (len + 1);

	mLength = len;

	return *this;
}

/**
 * @brief Append a c-string at the end of the current String
 *
 * @param src Pointer to the c-string to append
 * @return String& Reference to the object hitself
 */
String & String::append(const char *src)
{
	char *s;

	// Sanity check - If source pointer is NULL, nothing to append
	if (src == 0)
		return *this;

	// Compute the length of the string to append
	unsigned int srcLen = 0;
	char *p = (char *)src;
	while(*p)
		p++;
	srcLen = (p - src);

	// Compute the new total length
	int len = (mLength + srcLen);

	// Allocate a buffer to hold the new string
	char *newBuffer = (char *)malloc(len + 1);

	// Copy the current string to the new buffer
	if (mBuffer != 0)
	{
		p = newBuffer;
		s = mBuffer;
		for (unsigned int i = 0; i < mLength; i++)
		{
			*p = *s;
			s++;
			p++;
		}
	}
	else
		p = newBuffer;

	// Copy the additionnal part to the new buffer
	s = (char *)src;
	for (unsigned int i = 0; i < srcLen; i++)
	{
		*p = *s;
		s++;
		p++;
	}
	*p = 0;

	// If the string has already an internal buffer, free it
	if (mBuffer)
		free(mBuffer);
	// Set the newly allocated buffer as internal buffer
	mBuffer = newBuffer;
	mSize = (len + 1);

	mLength = len;

	return *this;
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
	if ((src == 0) || (len <= 0))
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
void String::copy(const String &src)
{
	copy((char *)src.data(), src.length());
}

/**
 * @brief Get a pointer to internal data buffer
 *
 * This method allow to get a direct access to internal datas. The returned
 * pointer can be used to read or write datas. This is usefull to implement
 * externally some specific data convertion, or to use this String with standard
 * C library functions. If the internal buffer is resized, this pointer may
 * become invalid.
 *
 * @return char* Pointer to the internal data buffer
 */
char *String::data(void) const
{
	return (char *)mBuffer;
}

/**
 * @brief Search the first occurence of a character into the string
 *
 * @param c    Character to find
 * @param from Offset where to start the search (default 0 for first character)
 * @return int Position of the first occurence of the character
 */
int String::indexOf(char c, int from) const
{
	unsigned int searchPos;

	// If the string is NULL, return with error
	if (mBuffer == 0)
		return -1;

	// If a negative start position has been specified
	if (from < 0)
	{
		// Compute the offset from the string length
		int testPos = mLength + from;
		if (testPos < 0)
			return -1;
		searchPos = testPos;
	}
	// Else, a positive (or nul) offset has been specified
	else
	{
		// Test if the requested offset if greater than length
		int test = mLength - from;
		if (test < 0)
			return -1;
		// Ok, use the specified offset
		searchPos = from;
	}

	// Do the search !
	while(searchPos < mSize)
	{
		if (mBuffer[searchPos] == c)
			break;
		searchPos++;
	}
	// If the search has reach the end of buffer
	if (searchPos == mSize)
		// Set error value for result
		searchPos = -1;

	return searchPos;
}

/**
 * @brief Test if the length of the String is nul.
 *
 * @return boolean True if the data length is 0
 */
bool String::isEmpty(void) const
{
	if (mLength == 0)
		return true;
	return false;
}

/**
 * @brief Search the last occurence of a character into the string
 *
 * @param c    Character to find
 * @param from Offset where to start the search (default -1 for last character)
 * @return int Position of the last occurence of the character
 */
int String::lastIndexOf(char c, int from) const
{
	int searchPos;

	// If the string is NULL, return with error
	if (mBuffer == 0)
		return -1;

	// If a negative start position has been specified
	if (from < 0)
	{
		// Compute the offset from the string length
		searchPos = mLength + from;
		if (searchPos < 0)
			return -1;
	}
	// Else, a positive (or nul) offset has been specified
	else
	{
		// Test if the requested offset if greater than length
		searchPos = mLength - from;
		if (searchPos < 0)
			return -1;
		// Ok, use the specified offset
		searchPos = from;
	}

	// Do the search !
	while(searchPos >= 0)
	{
		if (mBuffer[searchPos] == c)
			break;
		searchPos--;
	}

	return searchPos;
}


/**
 * @brief Get a substring with this first 'n' left characters
 *
 * @param n Number of characters to get
 * @return String A new string with a copy of 'n' characters
 */
String String::left(unsigned int n) const
{
	String subString;

	// If the current string is NULL, return empty String
	if (mBuffer == 0)
		return subString;

	// If the requested length is greater than string length
	if (n > mLength)
		// Limit to the number of available bytes
		n = mLength;

	subString.copy(mBuffer, n);

	return subString;
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
 * @brief Get a substring of 'n' characters from any position into the string
 *
 * @param pos Position of the first byte of the substring
 * @param n   Number of characters to get
 * @return String A new string with a copy of 'n' bytes
 */
String String::mid(int pos, int n) const
{
	String result;

	// Negative pos is not supported yet, return empty string
	if (pos < 0)
		return result;

	// If a negative (or nul) length is requested, nothing to do
	if (n <= 0)
		return result;

	// Check the maximum length of the substring
	int copyLen = (mLength - pos);
	// If the requested size is greater, reduce to maximum value
	if (n > copyLen)
		n = copyLen;

	// Copy substring into result
	result.copy(mBuffer + pos, n);

	return result;
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
 * @brief Remove characters at any position into the string
 *
 * @param pos Position of the first character to remove
 * @param n   Number of characters to remove
 */
String &String::remove(int pos, int n)
{
	int currentLen = mLength;
	// If the start position is beyond the end of string
	if (pos > currentLen)
		// Nothing to remove ... return current string
		return *this;

	// Pointer to the beginning of modified datas
	char *pDst = (mBuffer + pos);
	// Pointer after the removed datas
	char *pSrc = (pDst + n);
	// Pointer to the buffer end
	char *pEnd = (mBuffer + mLength);

	if (pSrc > pEnd)
		pSrc = pEnd;

	int lenBegin = pos;
	int lenEnd   = (int)(pEnd - pSrc);

	if ((lenBegin == 0) && (lenEnd == 0))
	{
		clear();
		return *this;
	}

	// Do copy !
	int removed = 0;
	for (int i = pos; i < currentLen; ++i)
	{
		*pDst = *pSrc;
		pDst ++;
		if (pSrc < pEnd)
		{
			pSrc++;
			removed++;
		}
	}

	// Update the string length
	mLength = lenBegin + lenEnd;
	return *this;
}

/**
 * @brief Force the allocation of a specified content size
 *
 * @param size The size (in bytes) of the content
 */
void String::reserve(unsigned int size)
{
	clear();
	realloc(size + 1);
	mBuffer[size] = 0;
	mLength = size;
}

/**
 * @brief Get a substring with the last 'n' characters
 *
 * @param n Number of characters to get
 * @return String A new string with a copy of the last 'n' characters
 */
String String::right(unsigned int n) const
{
	String subString;

	// If the current string is NULL, return empty String
	if (mBuffer == 0)
		return subString;

	// If the requested length is greater than string length
	if (n > mLength)
		// Limit to the number of available bytes
		n = mLength;

	unsigned int offset = (mLength - n);

	subString.copy(mBuffer + offset, n);

	return subString;
}

void String::setLength(size_t len)
{
	mLength = len;
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

/**
 * @brief Encode the string content into base64
 *
 * @return String Copy of this string, encoded into base64
 */
String String::toBase64(void) const
{
	int dstLen = (( (mLength / 3) + ((mLength % 3) > 0) ) * 4);
	String result;

	if (dstLen == 0)
		return result;

	// Pre-allocate memory to hold result data
	result.reserve(dstLen);
	
	long temp;
	unsigned char *s = (unsigned char *)mBuffer;
	unsigned char *d = (unsigned char *)result.data();
	for (unsigned int i = 0; i < (mLength / 3); i++)
	{
		// Read three bytes from input buffer
		temp  = (*s++) << 16;
		temp += (*s++) << 8;
		temp += (*s++);
		// Write four bytes to output buffer
		*d++ = base64Lookup[(temp & 0x00FC0000) >> 18];
		*d++ = base64Lookup[(temp & 0x0003F000) >> 12];
		*d++ = base64Lookup[(temp & 0x00000FC0) >> 6 ];
		*d++ = base64Lookup[(temp & 0x0000003F)      ];
	}
	// Encode remaining bytes
	switch(mLength % 3)
	{
		case 1:
			temp  = (*s++) << 16;
			*d++ = base64Lookup[(temp & 0x00FC0000) >> 18];
			*d++ = base64Lookup[(temp & 0x0003F000) >> 12];
			*d++ = '=';
			*d++ = '=';
			break;
		case 2:
			temp  = (*s++) << 16;
			temp += (*s++) << 8;
			*d++ = base64Lookup[(temp & 0x00FC0000) >> 18];
			*d++ = base64Lookup[(temp & 0x0003F000) >> 12];
			*d++ = base64Lookup[(temp & 0x00000FC0) >> 6 ];
			*d++ = '=';
			break;
	}
	return result;
}

/**
 * @brief Convert the numeric string (base 10) to an integer
 *
 * @return integer Numerical value of the string
 */
int String::toInt(void) const
{
	int result = 0;

	// Sanity check - If the current string is NULL, nothing to convert
	if (mBuffer == 0)
		return 0;

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
 * @brief Convert the String to a C++ standard library string object
 *
 * @return std::string The returned std::string contains a copy of datas
 */
std::string String::toStdStr(void) const
{
	if (mBuffer)
		return std::string(mBuffer);
	return std::string("");
}

/**
 * @brief Cut the string at a specified position
 *
 * @param pos Position where to cut the string
 */
void String::truncate(unsigned int pos)
{
	// If the requested position is 0, clear the whole string
	if (pos == 0)
	{
		clear();
		return;
	}

	// If the requested position is greater than string length ...
	if (pos > mLength)
		// ... nothing to do :)
		return;
	
	// Insert a 0x00 at the specified position
	mBuffer[pos] = 0;
	// Update the current length
	mLength = pos;
}


/**
 * @brief Decode the current string that contains urlencoded data
 *
 * This method is used to convert a string from urlencoded format to binary data. There is
 * no restriction for the target content, this allow non-ascii (binary) data to be
 * produced by percent+hex values. The size of the output string can be equal or less than
 * the source (length is updated) but the internal storage is never reduced.
 */
void String::urlDecode(void)
{
	if (mBuffer == 0)
		return;

	char *cIn  = mBuffer;
	char *cOut = mBuffer;

	// Read the whole string, search special chars
	for (unsigned int i = 0; i < mLength; i++)
	{
		// The plus char is used for spaces
		if (*cIn == '+')
		{
			// Replace plus char by space
			*cOut = ' ';
		}
		// The percent char is used for encoding data into hex
		else if (*cIn == '%')
		{
			char newChar = 0;
			cIn++; i++;
			// Convert the first hex char (uppper 4 bits)
			if ((*cIn >= '0') && (*cIn <= '9'))
				newChar = ((*cIn - '0') << 4);
			else if ((*cIn >= 'A') && (*cIn <= 'F'))
				newChar = ((*cIn - 'A' + 10) << 4);
			else if ((*cIn >= 'a') && (*cIn <= 'f'))
				newChar = ((*cIn - 'a' + 10) << 4);

			cIn++; i++;
			// Convert the second hex char (lower 4 bits)
			if ((*cIn >= '0') && (*cIn <= '9'))
				newChar |= (*cIn - '0');
			else if ((*cIn >= 'A') && (*cIn <= 'F'))
				newChar |= (*cIn - 'A' + 10);
			else if ((*cIn >= 'a') && (*cIn <= 'f'))
				newChar |= (*cIn - 'a' + 10);

			// Update string
			*cOut = newChar;
		}
		// Copy all other non-reserver characters
		else
		{
			*cOut = *cIn;
		}
		cIn++;
		cOut++;
	}

	*cOut = 0;
	mLength = (cOut - mBuffer);
}

/**
 * @brief Assign a new value to the string, based on another string
 *
 * @param src Source string
 * @return String& Reference to the string hitself
 */
String & String::operator=(const String & src)
{
	// If the source string is a empty
	if (src.isEmpty())
	{
		// Nothing to copy, clear current content and return
		clear();
		return *this;
	}

	copy((char *)src.data(), src.length());
	
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
	// If the source string is a null-pointer
	if (src == 0)
	{
		// Nothing to copy, clear current content and return
		clear();
		return *this;
	}

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
	// If the source string is a empty
	if (src.empty())
	{
		// Nothing to copy, clear current content and return
		clear();
		return *this;
	}

	// Copy the content of the std:string
	copy((char *)src.c_str(), src.length());
	
	return *this;
}

/**
 * @brief Overload the "+" operator to append a c-string to the current object
 *
 * @param src Pointer to a c-string to append (the right member of the "+")
 * @return String& A reference to the String object hitself
 */
String & String::operator+(const char *src)
{
	if (src == 0)
		return *this;

	append(src);
	return *this;
}

/**
 * @brief Overload the "+=" operator to append another String to the current object
 *
 * @param src Reference to a String to append (the right member of the "+")
 * @return String& A reference to the String object hitself
 */
String & String::operator+=(const String &src)
{
	append(src);
	return *this;
}

/**
 * @brief Overload the "+=" operator to append a c-string to the current object
 *
 * @param src Pointer to a c-string to append (the right member of the "+=")
 * @return String& A reference to the String object hitself
 */
String & String::operator+=(const char *src)
{
	if (src == 0)
		return *this;

	append(src);
	return *this;
}

// ------------------------------ Static Methods ------------------------------

String String::hex(unsigned char *src, int len)
{
	const char hex[] = "0123456789abcdef";
	char *tmpBuffer = (char *)malloc((len * 2) + 1);

	char *d = tmpBuffer;
	for (int i = 0; i < len; i++)
	{
		unsigned char b = *src;
		*d = hex[b >> 4];
		d++;
		*d = hex[b & 0x0F];
		d++;
		src++;
	}
	*d = 0;

	String result(tmpBuffer);
	free(tmpBuffer);

	return result;
}

String String::number(unsigned long n)
{
	char tmp[32];

	char *p = (tmp + 31);
	*p = 0;

	do
	{
		char digit = '0' + (n % 10);
		*--p = digit;
		n /= 10;
		if (p == tmp)
			break;
	}
	while (n != 0);

	String result(p);

	return result;
}

// ------------------------- Friend operators -------------------------

/**
 * @brief Overload the "+" operator to append a c-string to a String
 *
 * @param a Reference to a String (the left member of "+")
 * @param b Pointer to a c-string to append (the right member of the "+")
 * @return String A new String that contains the two input strings
 */
String operator+(const String &a, const char *b)
{
	String result(a);

	result.append(b);

	return result;
}

/**
 * @brief Test if the content of two strings are equal
 *
 * @param src Reference to a string object (left member of "==")
 * @param str Reference to a string object (right member of "==")
 * @return boolean True if the two strings are equal
 */
bool operator==(String const& src, const String &str)
{
	char *pSrc   = src.data();
	char *pOther = str.data();

	for (unsigned int i = 0; i < src.length(); ++i)
	{
		if (*pOther == 0)
			return false;
		if (*pOther != *pSrc)
			return false;
		pSrc++;
		pOther++;
	}
	if (*pOther != 0)
		return false;

	return true;
}

/**
 * @brief Test if the content of a string and a c-string are equal
 *
 * @param src Reference to a string object (left member of "==")
 * @param str Pointer to the c-string to compare
 * @return boolean True if the two strings are equal
 */
bool operator==(String const& src, const char *str)
{
	char *p = src.data();

	for (unsigned int i = 0; i < src.length(); ++i)
	{
		if (*str == 0)
			return false;
		if (*str != *p)
			return false;
		p++;
		str++;
	}
	if (*str != 0)
		return false;

	return true;
}

/**
 * @brief Test if the content of two strings are different
 *
 * @param src Reference to a string object (left member of "!=")
 * @param str Reference to a string object (right member of "!=")
 * @return boolean True if the two strings are different
 */
bool operator!=(String const& src, const String &str)
{
	// Use the "==" operator, and invert the result

	if (src == str)
		return false;
	return true;
}

/**
 * @brief Test if the content of a string and a c-string are different
 *
 * @param src Reference to a string object (left member of "!=")
 * @param str Pointer to the c-string to compare
 * @return boolean True if the two strings are different
 */
bool operator!=(String const& src, const char *str)
{
	// Use the "==" operator, and invert the result

	if (src == str)
		return false;
	return true;
}

/**
 * @brief Compare alphabetical values of two strings
 *
 * @param a Reference to the first tested string
 * @param b Reference to the second tested string
 * @return boolean True if 'b' is greater than 'a'
 */
bool operator<(String const& a, String const& b)
{
	const char *aStr = a.data();
	const char *bStr = b.data();

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
