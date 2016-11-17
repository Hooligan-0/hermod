/*
 * modDummy - An Hermod module for test and demo
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
#ifndef PAGE404_HPP
#define PAGE404_HPP

#include "Page.hpp"

namespace hermod {
	namespace Dummy {

class Page404: public Page
{
public:
	Page404();
	int process();
};

	} // namespace Dummy
} // namespace hermod
#endif
