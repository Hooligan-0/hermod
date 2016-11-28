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
#include "SessionCache.hpp"
#include "Session.hpp"

namespace hermod {

SessionCache* SessionCache::mInstance = NULL;  

/**
 * @brief Get a pointer to the cache
 *
 * The session cache works with a single object into application. To allow only
 * one instance (singleton) object constructor is private. This method is used
 * to get access to this static, unique variable.
 *
 * @return SessionCache* Pointer to the (singleton) cache object
 */
SessionCache* SessionCache::getInstance()
{
	if ( ! mInstance)
		mInstance = new SessionCache;
	
	return mInstance;
}

/**
 * @brief Destroy the whole session cache
 *
 * This method clear all session loaded into cache. Before freeing memory, each
 * of them is sync (saved) to the disk. At the end, the cache itself is
 * destroyed.
 */
void SessionCache::destroy(void)
{
	if ( ! mInstance)
		return;
	
	// Delete each cached Session
	while( mInstance->mCache.size() )
	{
		// Get the last cache item
		Session *sess = mInstance->mCache.back();
		// Remove this item from list
		mInstance->mCache.pop_back();
		// Save the session content to disk (if needed)
		sess->save();
		// Then, destroy it
		delete sess;
	}
	
	// Delete singleton object
	delete mInstance;
	mInstance = NULL;
}

/**
 * @brief Default (and private) constructor
 *
 */
SessionCache::SessionCache()
{
	// Nothing to do
}

/**
 * @brief Create a new session
 *
 * @return Session* Pointer to the newly created Session
 */
Session *SessionCache::create(void)
{
	// Allocate a new Session object
	Session *sess = new Session();
	// Then, use object itself to create a context
	sess->create();
	
	// Insert this new session into local cache
	mCache.push_back(sess);
	
	return sess;
}

/**
 * @brief Find and return an existing Session
 *
 * The specified session ID is first searched into the memory cache. If not
 * found, the method try to load session from disk.
 *
 * @param id The session identifier
 * @return Session* Pointer to the Session
 */
Session *SessionCache::getById(const String &id)
{
	Session *sess = NULL;
	
	// Sanity check
	if (id.isEmpty())
		return NULL;
	
	std::vector<Session *>::iterator it;
	for (it = mCache.begin(); it != mCache.end(); ++it)
	{
		if (id == (*it)->getId())
		{
			sess = (*it);
			break;
		}
	}
	
	// Not found into the cache, try to load from file
	if (sess == NULL)
	{
		sess = new Session();
		sess->load(id);
		if ( sess->isValid() )
		{
			mCache.push_back(sess);
		}
		else
		{
			delete sess;
			sess = NULL;
		}
	}
	return sess;
}

} // namespace hermod
/* EOF */
