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
#ifndef SESSIONCACHE_HPP
#define SESSIONCACHE_HPP
#include <vector>
#include "String.hpp"

namespace hermod {

class Session;

/**
 * @class SessionCache
 * @brief A global memory cache for Session
 *
 */
class SessionCache
{
public:
	static void destroy();
	static SessionCache* getInstance();
	static void clean(void);
public:
	Session *create (void);
	Session *getById(const String &id);
private:
	SessionCache();
	static SessionCache   *mInstance;
	std::vector<Session *> mCache;
};

} // namespace hermod
#endif
