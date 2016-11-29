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
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <sys/stat.h>
#include "Config.hpp"
#include "Session.hpp"

using namespace std;

namespace hermod {

/**
 * @brief Default constructor
 *
 */
Session::Session()
{
	mCache.clear();
	mIsNew = false;
	mValid = false;
	mCount = 1;
	mTtlLast  = time(0);
	mTtlLimit = TTL_DEFAULT;
}

/**
 * @brief TEMPORARY - This method clear all entries with a key name that start with "key_"
 *
 */
void Session::clearFileKey(void)
{
	for( map<String,String>::iterator ii=mCache.begin(); ii!=mCache.end(); ++ii)
	{
		if ( (*ii).first.left(4) == "key_" )
		{
			mCache.erase(ii);
			ii=mCache.begin();
		}
	}
}

/**
 * @brief Initialize a new session
 *
 * This method initialize the current object as new session. A new ID is created
 * and the name of the file used to save datas is computed (but not created)
 */
void Session::create(void)
{
	// Get a random integer using standard rand
	int rndKeyId = std::rand();
	// Convert this value to string
	String rndKey = String::number(rndKeyId);
	
	Config  *cfg = Config::getInstance();
	mFilename  = cfg->get("global", "path_session");
	mFilename += "hermod-session-" + rndKey;
	
	// Save key to the cache
	mCache["Key"] = rndKey;

	mKey = rndKey;
	mValid = true;
	mIsNew = true;
	// Update the last access time
	updateTtl();
}

/**
 * @brief Load a session from a file
 *
 * @param key Id of the session to load
 */
void Session::load(String sessId)
{
	struct stat buffer;
	String sessionfile;

	Config  *cfg = Config::getInstance();
	sessionfile  = cfg->get("global", "path_session");
	sessionfile += "hermod-session-" + sessId;

	if ( stat (sessionfile.data(), &buffer) )
		return;

	mKey      = sessId;
	mFilename = sessionfile;
	
	std::ifstream sfile(mFilename.data());
	std::string lineData;
	while (getline (sfile,lineData))
	{
		// Search key/value separator
		std::size_t pos = lineData.find(": ");
		if (pos == std::string::npos)
			continue;
		
		String key = lineData.substr(0, pos);
		String val = lineData.substr(pos + 2);
		// Save key into memory cache
		mCache[key] = val;
	}
	sfile.close();

	mCount ++;

	mCache["COUNT"] = String::number(mCount);

	mValid = true;
	mIsNew = false;
	// Update the last access time
	updateTtl();
}

/**
 * @brief Save the session into a file
 *
 */
void Session::save(void)
{
	fstream sfile;
	ostringstream dat;
	
	sfile.open(mFilename.data(), ios::out | ios::trunc);
	
	if ( ! sfile.is_open())
		throw runtime_error("Session: Could not open the file!");
	
	std::map<String, String>::iterator it;
	for (it = mCache.begin(); it != mCache.end(); ++it)
	{
		dat << it->first << ": " << it->second << endl;
	}
	sfile << dat.str();
	sfile.flush();
	sfile.close();
	// Update the last access time
	updateTtl();
}

/**
 * @brief Get the session ID
 *
 * @return String The session ID
 */
String Session::getId(void)
{
	return mKey;
}

/**
 * @brief Get the value of a key, identified by his key name
 *
 * @param  key    Name of the key
 * @return String Value of the key as string
 */
String Session::getKey(const String &key)
{
	String result;

	for( map<String,String>::iterator ii=mCache.begin(); ii!=mCache.end(); ++ii)
	{
		if ( (*ii).first == key )
		{
			result = (*ii).second;
			break;
		}
	}
	// Update the last access time
	updateTtl();

	return result;
}

/**
 * @brief Get the numeric value of a key
 *
 * @param key Name of the key
 * @return int Value of the key as integer
 */
int Session::getKeyInt(const String &key)
{
	String strValue = getKey(key);
	return strValue.toInt();
}

/**
 * @brief Get the cache TTL
 *
 * @return integer Current value od TTL (in seconds)
 */
int Session::getTtlLimit(void)
{
	return mTtlLimit;
}

/**
 * @brief Set the value of a key
 *
 * @param key Name of the key to update
 * @param value New value to set
 */
void Session::setKey(const String &key, const String &value)
{
	// Update the last access time
	updateTtl();
	// Save the new value into session key
	mCache[ key ] = value;
}

/**
 * @brief Set the value of a key (numeric)
 *
 * @param key Name of the key to update
 * @param value New value to set (long int)
 */
void Session::setKey(const String &key, unsigned long value)
{
	// Update the last access time
	updateTtl();
	// Save the new value into session key
	mCache[ key ] = String::number(value);
}

/**
 * @brief Set the cache TTL
 *
 * @param limit New TTL value
 */
void Session::setTtlLimit(int limit)
{
	mTtlLimit = limit;
}

/**
 * @brief Remove a key
 *
 * @param key Name of the key to delete
 */
void Session::removeKey(const String &key)
{
	std::map<String,String>::iterator it;
	it = mCache.find(key);
	if (it != mCache.end())
		mCache.erase (it);
	// Update the last access time
	updateTtl();
}

/**
 * @brief TEMPORARY - This is a debug method used to test an authentication system
 *
 */
void Session::auth(unsigned long id, String user)
{
	mCache["AuthUserId"]   = String::number(id);
	mCache["AuthUsername"] = user;
}

/**
 * @brief Test if this is a new session
 *
 * @return boolean True if newly created (using create()), False in other case
 */
bool Session::isNew(void)
{
	return mIsNew;
}

/**
 * @brief Test if the TTL of this session has expired
 *
 * @return boolean True if the last cache access is oldest than TTL
 */
bool Session::isTtlExpired(void)
{
	bool expired = false;

	// If the limit value is negative, the TTL is unlimited
	if (mTtlLimit < 0)
		return false;

	int elapsed = time(0) - mTtlLast;
	if (elapsed > mTtlLimit)
		expired = true;

	return expired;
}

/**
 * @brief Test if the session is valid
 *
 * @return boolean True if the session is valid
 */
bool Session::isValid(void)
{
	return mValid;
}

/**
 * @brief TEMPORARY - This is a debug method used to test an authentication system
 *
 */
int Session::isAuth(void)
{
	for( map<String,String>::iterator ii=mCache.begin(); ii!=mCache.end(); ++ii)
	{
		if ( (*ii).first == "AuthUsername" )
		{
			return (1);
		}
	}
	return 0;
}

/**
 * @brief Update last access time (this reset TTL)
 *
 */
void Session::updateTtl(void)
{
	mTtlLast = time(0);
}

} // namespace hermod
/* EOF */
