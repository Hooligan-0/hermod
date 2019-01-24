/*
 * Hermod - Modular application framework
 *
 * Copyright (c) 2016-2019 Cowlab
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
#include "ModuleCache.hpp"
#include "Page.hpp"
#include "Server.hpp"
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
	enum Method { Undef,
	              Get, Head, Post,
	              Put, Delete, Link, Unlink,
	              Connect, Options, Trace,
	              Patch };
public:
	explicit Request(Server *server);
	~Request();
	unsigned int  countUriArgs(void);
	String  getContentType(void);
	Method  getMethod(void);
	String  getParam (const String &name);
	String  getUri   (unsigned int n);
	String  getFormValue (const String &name);
	String  getCookieByName(const String &name, bool allowEmpty);
	bool    hasFormValue (const String &name);
	bool    isAccept(const String &type);
	void    setBody (String *body);
	void    setHeaderParameter(const String &name, const String &value);
	void    setUri  (const String &route);
protected:
	void    loadFormInputs(void);
private:
	Server        *mServer;
	Method         mMethod;
	String        *mBody;
	std::vector<String>       mUri;
	std::map <String, String> mHeaderParameters;
	std::map <String, String> mFormParameters;
};

} // namespace hermod
#endif
