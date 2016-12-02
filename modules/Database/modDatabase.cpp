/*
 * modDatabase - An Hermod module to use DBMS
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
#include <iostream>
#include "modDatabase.hpp"
#include "Config.hpp"
#include "DB.hpp"

namespace hermod {

extern "C" Module* create_object()
{
	hermod::Database::ModDatabase *module;
	module = new hermod::Database::ModDatabase;
	
	return module;
}

extern "C" void destroy_object(Module *module)
{
	hermod::Database::ModDatabase *dbModule = (hermod::Database::ModDatabase *)module;
	delete dbModule;
}

	namespace Database {

/**
 * @brief Default constructor
 *
 */
ModDatabase::ModDatabase()
  : Module()
{
	mCache.clear();

	// Set a name for this module
	setName("Database");
}

/**
 * @brief Default destructor
 *
 */
ModDatabase::~ModDatabase()
{
	// Delete all allocated "DB" objects
	while(mCache.size())
	{
		DB *db = mCache.back();
		mCache.pop_back();
		delete db;
	}
}

/**
 * @brief Get a database, identified by his name
 *
 * @param  name Name of the wanted database
 * @return DB*  Pointer to the database object (or NULL if not found)
 */
DB *ModDatabase::get(const std::string &name)
{
	DB *database = 0;

	std::string sysname;
	if ( ! name.empty())
		sysname = name;
	else
		sysname = "main";

	std::vector<DB *>::iterator it;
	for (it = mCache.begin(); it != mCache.end(); ++it)
	{
		if (name.compare( (*it)->getName() ) == 0)
		{
			database = (*it);
			break;
		}
	}

	if (database == 0)
	{
		std::string dbName;
		std::string dbUser;
		std::string dbPass;

		// Try to load the database dedicated file
		Config *cfg  = Config::getInstance("database");
		if (cfg)
		{
			dbName = cfg->get(sysname, "name").toStdStr();
			dbUser = cfg->get(sysname, "username").toStdStr();
			dbPass = cfg->get(sysname, "password").toStdStr();
		}
		// If no dedicated file, load config from main config
		else
		{
			// Only one "main" database can take place into global config
			if (sysname != "main")
				return 0;

			cfg  = Config::getInstance();
			dbName = cfg->get("mod:database", "name").toStdStr();
			dbUser = cfg->get("mod:database", "username").toStdStr();
			dbPass = cfg->get("mod:database", "password").toStdStr();
		}

		if ( ! dbName.empty() )
		{
			// Instanciate a new DB
			DB *newDB = new DB(sysname);

			newDB->setDbName(dbName);

			// Set User/Pass config
			if ( ! dbUser.empty() )
				newDB->setIdent(dbUser, dbPass);
			// Insert the new database into local cache
			mCache.push_back( newDB );
			// Set as return value
			database = newDB;
		}
	}
	return database;
}

/**
 * @brief Remove (free a previously allocated) database
 *
 * @param db Pointer to the database to remove. This pointer is invalid after this call
 */
void ModDatabase::remove(DB *db)
{
	std::vector<DB *>::iterator it;
	for (it = mCache.begin(); it != mCache.end(); ++it)
	{
		if ((*it) == db)
		{
			mCache.erase(it);
			delete db;
			break;
		}
	}
}

	} // namespace Database
} // namespace hermod
/* EOF */
