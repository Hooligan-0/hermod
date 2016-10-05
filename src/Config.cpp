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
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sys/stat.h>
#include "Config.hpp"
#include "ConfigKey.hpp"
#include "Log.hpp"

namespace hermod {

Config* Config::mInstance = NULL;  

/**
 * @brief Delete the global config instance (singleton)
 *
 */
void Config::destroy(void)
{
	if ( ! mInstance)
		return;

	// Delete singleton object
	delete mInstance;
	mInstance = NULL;
}

/**
 * @brief Default (private) destructor
 *
 * The default destructor is private to avoid deleting the singleton. To delete
 * config, the static destroy() method must be called.
 */
Config::~Config()
{
	// Delete all config groups
	while( mGroups.size() )
	{
		ConfigGroup *g = mGroups.back();
		mGroups.pop_back();
		delete g;
	}

	// Delete sub-config files
	while( mFiles.size() )
	{
		Config *cfg = mFiles.back();
		mFiles.pop_back();
		delete cfg;
	}
}

/**
 * @brief Get (or create) the global configuration object
 *
 * @return Config* Pointer to the global config
 */
Config* Config::getInstance(void)
{
	if ( ! mInstance)
	{
		mInstance = new Config;
		mInstance->mName = "main";
	}
	return mInstance;
}

/**
 * @brief Get (or load) a secondary configuration file
 *
 * @param  filename Name of the configuration file
 * @return Config* Pointer to the config object for the specified file
 */
Config* Config::getInstance(const String &filename)
{
	Config *mainCfg = getInstance();

	if ( filename.isEmpty() )
		return mainCfg;

	// Search into the already loaded files
	std::vector<Config *>::iterator it;
	for (it = mainCfg->mFiles.begin(); it != mainCfg->mFiles.end(); ++it)
	{
		if ((*it)->getName() == filename)
			return (*it);
	}

	// Here, file not already loaded, try to load it

	std::size_t sep = mainCfg->mFilename.rfind('/');
	if (sep == std::string::npos)
	{
		Log::warning() << "Config: Cannot find base directory of additional files." << Log::endl;
		return 0;
	}

	String cfgfile = mainCfg->mFilename.substr(0, sep + 1);
	cfgfile += filename;
	cfgfile += ".cfg";

	// Test if the file exists
	struct stat fileInfo;
	if ( stat (cfgfile.data(), &fileInfo) )
		return 0;

	// Try to load the configuration file
	Config *newFile = 0;
	try {
		newFile = new Config;
		newFile->setName(filename);
		newFile->load(cfgfile);

		mainCfg->mFiles.push_back(newFile);
	} catch (...) {
		// ToDo : Log error ?
	}

	return newFile;
}

/**
 * @brief Create a new configuration group (like "[section]" into file)
 *
 * @param  name Name of the new group
 * @return ConfigGroup* Pointer to the newly created group
 */
ConfigGroup *Config::createGroup(const std::string &name)
{
	ConfigGroup *grp;
	
	// Search if the group already exists
	grp = getGroup(name);
	if (grp)
		return grp;
	
	// Create a new group
	try {
		grp = new ConfigGroup;
	} catch (std::exception& e) {
		return NULL;
	}
	
	grp->setName(name);
	mGroups.push_back(grp);
	
	return grp;
}

/**
 * @brief Get a configuration group identified by his name
 *
 * @param  name Name of the group
 * @return ConfigGroup* Pointer to the configuration group (or NULL)
 */
ConfigGroup *Config::getGroup(const std::string &name)
{
	size_t nbGroups;
	
	nbGroups = mGroups.size();
	
	// Look into the list of known groups
	for (size_t i = 0; i < nbGroups; i++)
	{
		ConfigGroup *grp = mGroups.at(i);
		if (grp->getName() == name)
			return grp;
	}
	return NULL;
}

/**
 * @brief Get the value of a configuration key (as string)
 *
 * @param  group Name of the configuration group
 * @param  key   Name of the key
 * @param  pos   Optional index into a key array
 * @return string Value of the key, as string
 */
String Config::get(const String &group, const String &key, size_t *pos)
{
	ConfigGroup *g;
	ConfigKey   *k;
	
	// Get the specified group
	g = getGroup(group);
	if (g == NULL)
		return std::string();
	
	// Get the specified key into the group
	k = g->getKey(key, pos);
	if (k == NULL)
		return String();
	
	return k->getValue();
}

/**
 * @brief Get a key into a group, identified by his position
 *
 * @param  group Name of the configuration group
 * @param  index Index of the key into the specified group
 * @return ConfigKey* Pointer to the configuration key object
 */
ConfigKey *Config::getKey(const std::string &group, int index)
{
	ConfigGroup *g;
	
	// Get the specified group
	g = getGroup(group);
	if (g == NULL)
		return NULL;
	
	return g->getKey(index);
}

/**
 * @brief Get a key identified by his name (and a group name)
 *
 * @param group Name of the group where the key is present
 * @param key   Name of the key search into this group
 * @return ConfigKey* Pointer to the key (or NULL if not found)
 */
ConfigKey *Config::getKey(const std::string &group,
                          const std::string &key)
{
	ConfigGroup *g;
	ConfigKey   *k;
	
	// Get the specified group
	g = getGroup(group);
	if (g == NULL)
		return NULL;

	// Get the specified key into the group
	k = g->getKey(key, 0);
	if (k == NULL)
		return NULL;
	
	return k;
}

/**
 * @brief Get the name of this Config object
 *
 * @return string Name of this config
 */
String Config::getName(void) const
{
	return mName;
}

/**
 * @brief Read the content of a config file, and load values
 *
 * @param filename Full name of the file to read
 */
void Config::load(const std::string &filename)
{
	std::fstream cfgFile;
	cfgFile.open(filename.c_str(), std::ios::in);
	if ( ! cfgFile.is_open())
		throw std::runtime_error("Config: Could not open the file!");

	// Save the file name
	mFilename = filename;

	ConfigGroup *group;

	std::string lineData;
	while ( getline(cfgFile, lineData) )
	{
		size_t found;
		found = lineData.find_first_not_of(" \t");
		if ( (found != std::string::npos) && (found != 0) )
			lineData.erase(0, found);
		if (lineData[0] == '[')
		{
			found = lineData.find(']');
			if (found == std::string::npos)
				continue;
			// Get or create this config group
			std::string grpName;
			grpName = lineData.substr(1, found - 1);
			group = createGroup(grpName);
			continue;
		}
		if (lineData[0] == '#')
		{
			// This line is a comment
			continue;
		}
		// Search key/value separator
		found = lineData.find('=');
		if (found == std::string::npos)
			continue;
		// Extract the key/value pair
		std::string key   ( lineData.substr(0, found) );
		std::string value ( lineData.substr(found+ 1, std::string::npos) );
		// Create a new config key
		ConfigKey *k = group->createKey(key, true);
		// Save the specified value
		k->setValue(value);
	}
	cfgFile.close();
}

/**
 * @brief Set the value of an existing key, or create it
 *
 * @param group Name of the configuration group
 * @param key Name of the key to modify or create
 * @param value New value to set (as string)
 */
void Config::set(const std::string &group,
                 const std::string &key,
                 const std::string &value)
{
	ConfigGroup *g;
	ConfigKey   *k;
	
	// Get or create specified group
	g = createGroup(group);
	// Get or create specified key
	k = g->createKey(key);
	
	k->setValue(value);
}

/**
 * @brief Set the name of this config object
 *
 * The name of a config object is mainly used when two or more configuration
 * file are loaded. This value can be readed by getName and can be set as
 * parameter of a getInstance.
 *
 * @param name New name to set
 */
void Config::setName(const String &name)
{
	mName = name;
}

} // namespace hermod
                                          
// -------------------- GROUPs -------------------- //

namespace hermod {

ConfigGroup::ConfigGroup()
{
	mName.clear();
	mKeys.clear();
}

ConfigGroup::~ConfigGroup()
{
	while(mKeys.size())
	{
		ConfigKey *k = mKeys.back();
		mKeys.pop_back();
		delete k;
	}
}

String ConfigGroup::getName() const
{
	return mName;
}
void ConfigGroup::setName(const String &name)
{
	mName = name;
}

ConfigKey *ConfigGroup::createKey(const std::string &name, bool multiple)
{
	ConfigKey *key;
	
	if (multiple == false)
	{
		// Search if the key already exists
		key = getKey(name);
		if (key)
			return key;
	}
	
	// Create a new key
	try {
		key = new ConfigKey(name);
	} catch (std::exception& e) {
		return NULL;
	}
	
	mKeys.push_back(key);
	
	key->setPos(mKeys.size() - 1);
	
	return key;
}

ConfigKey *ConfigGroup::getKey(const std::string &name, size_t *pos)
{
	size_t nbKeys;
	
	nbKeys = mKeys.size();
	
	int first = 0;
	if (pos)
		first = *pos;
	
	// Look into the list of known keys
	for (size_t i = first; i < nbKeys; i++)
	{
		ConfigKey *key = mKeys.at(i);
		if (name.compare(key->getName()) == 0)
		{
			if (pos)
				*pos = i;
			return key;
		}
	}
	return NULL;
}

ConfigKey *ConfigGroup::getKey(unsigned int index)
{
	if (index >= mKeys.size())
		return NULL;
	
	return mKeys.at(index);
}

} // namespace hermod
/* EOF */
