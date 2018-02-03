/*
 * Hermod - Modular application framework
 *
 * Copyright (c) 2016-2018 Cowlab
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
#ifndef SESSION_HPP
#define SESSION_HPP
#include <ctime>
#include <map>
#include "String.hpp"

using namespace std;

namespace hermod {

#define TTL_DEFAULT 300

/**
 * @class Session
 * @brief This class manage one user session
 *
 * A session is a named collection of key/value array. This allow to track and
 * save user data across multiple requests. A Session object can be standalone,
 * but the general case is to put them into a SessionCache to keep them into
 * memory. When a Session object is deleted, the array is saved to a file.
 */
class Session
{
public:
	Session();
	void   create(void);
	bool   drop(void);
	void   load(String sessId);
	void   save(void);
	void auth(unsigned long id, String user);
	String getKey   (const String &key);
	int    getKeyInt(const String &key);
	int    getTtlLimit(void);
	void   setKey(const String &key, const String &value);
	void   setKey(const String &key, unsigned long value);
	void   setTtlLimit(int limit);
	void   removeKey(const String &key);
	void   clearFileKey(void);
public:
	String getId  (void);
	bool   isNew  (void);
	bool   isTtlExpired(void);
	bool   isValid(void);
	int    isAuth (void);
protected:
	void   updateTtl(void);
private:
	int    mCount;
	int    mTtlLimit;
	time_t mTtlLast;
	bool   mIsNew;
	bool   mValid;
	String mKey;
	String mFilename;
	std::map <String, String> mCache;
};
} // namespace hermod
#endif
