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
#include "ModuleCache.hpp"
#include "Config.hpp"
#include "Log.hpp"
#include <dlfcn.h>
#include <stdexcept>

namespace hermod {

/**
 * @brief Default constructor
 *
 */
ModuleCache::ModuleCache()
{
}

/**
 * @brief Default destructor
 *
 */
ModuleCache::~ModuleCache()
{
}

/**
 * @brief Unload all modules of this cache
 *
 */
void ModuleCache::clear(void)
{
	while( mModules.size() )
		unload( mModules.size() - 1 );
}

/**
 * @brief Get the number of Modules into this cache object
 *
 * @return integet Number of known modules
 */
int  ModuleCache::count(void)
{
	return mModules.size();
}

/**
 * @brief Find a module identified by his name
 *
 * @return Module* Pointer to the module object (or NULL)
 */
Module *ModuleCache::find(const String &name)
{
	Module *module = 0;
	
	std::vector<Module *>::iterator it;
	for (it = mModules.begin(); it != mModules.end(); ++it)
	{
		if (name == (*it)->getName())
		{
			module = *it;
			break;
		}
	}
	
	return module;
}

/**
 * @brief Get a Module at a specific cache position
 *
 * @param  pos     Position of the module to get
 * @return Module* Pointer to the module object (or NULL)
 */
Module * ModuleCache::get(unsigned int pos)
{
	// If the requested position is greater than cache size, return null
	if (pos >= mModules.size())
		return 0;

	// Return the Module (pointer) at the specified position
	return mModules.at(pos);
}

/**
 * @brief Load a module defined by his name
 *
 * @param  name String with the name of the module to load
 * @return Module* Pointer to the newly loaded module (or null if error)
 */
Module * ModuleCache::load(const String &name)
{
	Config *cfg = Config::getInstance();
	
	String fullname = cfg->get("plugins", "directory");
	fullname += name;
	
	void *handle = dlopen(fullname.data(), RTLD_NOW|RTLD_GLOBAL);
	if (handle == 0)
	{
		Log::warning() << "Load module " << name << " ";
		Log::warning() << "failed : " << dlerror() << Log::endl;
		return 0;
	}
	
	Module *newModule = 0;
	try {
		Module *(*create)(void);
		
		create = (Module*(*)(void))dlsym(handle, "create_object");
		if (create == 0)
			throw std::runtime_error("init function missing");
		
		newModule = create();
		if (newModule == 0)
			throw std::runtime_error("Create failed");
	} catch (std::exception &e) {
		Log::warning() << "Load module " << name << " ";
		Log::warning() << "failed : " << e.what() << Log::endl;
		dlclose(handle);
		return 0;
	}
	
	newModule->setHandle(handle);
	newModule->setCache( this );
	
	mModules.push_back(newModule);
	
	return newModule;
}

/**
 * @brief Unload a previously loaded Module
 *
 * @param index Position into the cache of the module to unload
 */
void ModuleCache::unload(int index)
{
	Module *mod;
	
	try {
		void (*destroy)(Module*);
		
		mod = mModules.at(index);
		void *handle = mod->getHandle();
		destroy = (void(*)(Module*))dlsym(handle, "destroy_object");
		if (destroy == 0)
			throw std::runtime_error("Destroy failed");
		
		// Remove the Module from plugins -first-
		mModules.erase( mModules.begin() + index);
		// Then, destroy the plugin
		destroy(mod);
		// Unload/close the lib
		dlclose(handle);
	} catch (std::exception &e) {
		Log::warning() << "MOD: " << e.what() << Log::endl;
	}
	
	return;
}

} // namespace hermod
/* EOF */
