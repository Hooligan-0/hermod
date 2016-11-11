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
#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <map>
#include <fcgio.h>
#include "ModuleCache.hpp"
#include "Page.hpp"
#include "String.hpp"

namespace hermod {

/**
 * @class Request
 * @brief The Request class handle received datas and environment of an incoming request
 *
 * When hermod receive an incoming connection from a client, datas are saved into a
 * Request object. This object is then used to dispatch the request to a module and by
 * the page to get uri, parameters, form datas ...
 */
class Request
{
public:
	enum Method { Undef, Get, Post, Option };
public:
	explicit Request(FCGX_Request *req);
	~Request();
	unsigned int     countUriArgs(void);
	FCGX_Request    *getFCGX  (void);
	String           getContentType(void);
	Request::Method  getMethod(void);
	String           getParam (const String &name);
	std::string getUri(unsigned int n);
	String      getFormValue (const String &name);
	std::string getCookieByName(const std::string &name, bool allowEmpty);
	bool        hasFormValue (const String &name);
	void        setUri(const std::string &route);
protected:
	void        loadFormInputs(void);
private:
	FCGX_Request  *mFcgiRequest;
	Method         mMethod;
	std::vector<std::string> mUri;
	std::map <String, String> mFormParameters;
};

} // namespace hermod
#endif
