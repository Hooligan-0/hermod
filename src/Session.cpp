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
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <sys/stat.h>
#include "Config.hpp"
#include "Session.hpp"

using namespace hermod;
using namespace std;

Session::Session()
{
	mCache.clear();
	mIsNew = false;
	mValid = false;
	mCount = 1;
}

void Session::clearFileKey(void)
{
	for( map<string,string>::iterator ii=mCache.begin(); ii!=mCache.end(); ++ii)
	{
		if ( (*ii).first.compare(0, 4, "key_") == 0)
		{
			mCache.erase(ii);
			ii=mCache.begin();
		}
	}
}

void Session::create(void)
{
	ostringstream convert;
	int rndKeyId = std::rand();
	string rndKey;
	
	convert << rndKeyId;
	rndKey = convert.str();
	
	Config  *cfg = Config::getInstance();
	mFilename  = cfg->get("global", "path_session");
	mFilename += "hermod-session-" + rndKey;
	
	// Save key to the cache
	mCache["Key"] = rndKey;

	mKey = rndKey;
	mValid = true;
	mIsNew = true;
}

void Session::load(string key)
{
	struct stat buffer;
	string sessionfile;

	Config  *cfg = Config::getInstance();
	sessionfile  = cfg->get("global", "path_session");
	sessionfile += "hermod-session-" + key;

	if ( stat (sessionfile.c_str(), &buffer) )
		return;

	mKey      = key;
	mFilename = sessionfile;
	
	std::ifstream sfile(mFilename.c_str());
	std::string lineData;
	while (getline (sfile,lineData))
	{
		// Search key/value separator
		std::size_t pos = lineData.find(": ");
		if (pos == std::string::npos)
			continue;
		
		std::string key = lineData.substr(0, pos);
		std::string val = lineData.substr(pos + 2);
		// Save key into memory cache
		mCache[key] = val;
	}
	sfile.close();

	mCount ++;

	ostringstream updCount;
	updCount << mCount;
	mCache["COUNT"] = updCount.str();

	mValid = true;
	mIsNew = false;
}

void Session::save(void)
{
	fstream sfile;
	ostringstream dat;
	
	sfile.open(mFilename.c_str(), ios::out | ios::trunc);
	
	if ( ! sfile.is_open())
		throw runtime_error("Session: Could not open the file!");
	
	std::map<string, string>::iterator it;
	for (it = mCache.begin(); it != mCache.end(); ++it)
	{
		dat << it->first << ": " << it->second << endl;
	}
	sfile << dat.str();
	sfile.flush();
	sfile.close();
}

string Session::getId(void)
{
	return mKey;
}

std::string Session::getKey(const std::string &key)
{
	for( map<string,string>::iterator ii=mCache.begin(); ii!=mCache.end(); ++ii)
	{
		if ( (*ii).first.compare(key) == 0)
		{
			return (*ii).second;
		}
	}
	return std::string("");
}

int Session::getKeyInt(const std::string &key)
{
	std::string strValue = getKey(key);
	int intValue = atoi(strValue.c_str());
	return intValue;
}

void Session::setKey(const std::string &key, const std::string &value)
{
	mCache[ key ] = value;
}

void Session::setKey(const std::string &key, unsigned long value)
{
	ostringstream temp;
	temp << value;
	mCache[ key ] = temp.str();
}

void Session::removeKey(const std::string &key)
{
	std::map<string,string>::iterator it;
	it = mCache.find(key);
	if (it != mCache.end())
		mCache.erase (it);
}

void Session::auth(unsigned long id, string user)
{
	ostringstream convertId;
	std::string   strId;
	
	convertId << id;
	strId = convertId.str();
	
	mCache["AuthUserId"] = strId;
	mCache["AuthUsername"] = user;
}

bool Session::isNew(void)
{
	return mIsNew;
}

bool Session::isValid(void)
{
	return mValid;
}

int Session::isAuth(void)
{
	for( map<string,string>::iterator ii=mCache.begin(); ii!=mCache.end(); ++ii)
	{
		if ( (*ii).first.compare("AuthUsername") == 0)
		{
			return (1);
		}
	}
	return 0;
}
/* EOF */
