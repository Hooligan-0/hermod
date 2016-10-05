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
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <cstddef> // std::size_t
#include <vector>
#include "String.hpp"
#include "ConfigKey.hpp"

namespace hermod {

class ConfigGroup;

/**
 * @class Config
 * @brief This class define the main object to handle configuration keys
 *
 */
class Config
{
public:
	static void destroy();
	static Config* getInstance(void);
	static Config* getInstance(const String &filename);
	String get(const String &group, const String &key, size_t *pos = 0);
	ConfigKey  *getKey(const std::string &group, const std::string &key);
	ConfigKey  *getKey(const std::string &group, int index);
	String      getName(void) const;
	void set(const std::string &group,
	         const std::string &key,
	         const std::string &value);
	void load (const std::string &filename);
protected:
	ConfigGroup *createGroup(const std::string &name);
	ConfigGroup *getGroup   (const std::string &name);
private:
	Config() {
		mGroups.clear();
		mFiles.clear();
	};
	~Config();
	void setName(const String &name);
private:
	static Config* mInstance;
	String         mName;
	std::string    mFilename;
	std::vector<ConfigGroup *> mGroups;
	std::vector<Config *> mFiles;
};

/**
 * @class ConfigGroup
 *
 */
class ConfigGroup
{
public:
	ConfigGroup();
	~ConfigGroup();
	String     getName() const;
	void setName(const String &name);
	ConfigKey *createKey(const std::string &name, bool multiple = false);
	ConfigKey *getKey   (const std::string &name, size_t *pos = 0);
	ConfigKey *getKey   (unsigned int index);
private:
	String mName;
	std::vector<ConfigKey *> mKeys;
};

} // namespace hermod
#endif
