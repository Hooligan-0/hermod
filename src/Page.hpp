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
#ifndef PAGE_HPP
#define PAGE_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "Content.hpp"
#include "ContentHtml.hpp"
#include "ContentJson.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Session.hpp"
#include "String.hpp"

namespace hermod {

/**
 * @class Page
 * @brief This class define a generic Page skeleton
 *
 * When a request is received and match a route, a Page is allocated to process
 * the request and generate some datas as response. This class must be used as
 * parent class for module-specific pages.
 */
class Page
{
public:
	Page();
	virtual ~Page();
	void   setRequest(Request   *obj);
	void   setReponse(Response  *obj);
	void   initSession(int mode = 0);
	void   loadSession(int mode = 0);
	
	Content     *initContent(void);
	ContentHtml *initContentHtml(void);
	ContentJson *initContentJson(void);
	
	bool   useSession(void);
	void   useSession(bool use);
public:	
	virtual String getArg(int n);
	virtual int getArgCount(void);
	virtual int process() = 0;
protected:
	Session  *session (void);
	Request  *request (void);
	Response *response(void);
private:
	Request  *mRequest;
	Response *mResponse;
	Session  *mSession;
	bool      mUseSession;
};

} // namespace hermod
#endif
