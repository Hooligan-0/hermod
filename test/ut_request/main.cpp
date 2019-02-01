/*
 * Hermod - Modular application framework
 *
 * Copyright (c) 2019 Cowlab
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
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <string>
#include <signal.h>
#include <unistd.h>
#include "Request.hpp"
#include "Log.hpp"
#include "String.hpp"

using namespace hermod;

static void ut_HeaderParameter(void);
static void ut_FormValues(void);

static int log_level;

/**
 * @brief Entry point of hermod
 *
 * @param argc Number of arguments on command line
 * @param argv Pointer to arguments array
 */
int main(int argc, char **argv)
{
	int i;

	log_level = 1;

	for (i = 1; i < argc; i++)
	{
		std::string arg( argv[i] );
		if (arg.compare("-v") == 0)
			log_level = 2;
	}

	try {
		// Call Header parameters unit-test
		std::cout << " * Test header parameters ";
		ut_HeaderParameter();
		std::cout << "[PASS]" << std::endl;
		// Call Form Values unit-test
		std::cout << " * Test Form values       ";
		ut_FormValues();
		std::cout << "[PASS]" << std::endl;
	} catch(const char *e) {
		std::cout << "[FAILED]" << std::endl;
		if (log_level > 1)
		{
			std::cerr << e << std::endl;
			Log::sync();
		}
		return(-1);
	}

	return(0);
}

/**
 * @brief Test parsing of Form data
 *
 */
static void ut_FormValues(void)
{
	Request *req = 0;

	// Test with urlEnconded datas sended as POST
	try {
		String *body = 0;
		body = new String("var1=value1&vtest=dummy&var2=value2");

		req = new Request(0);
		req->setHeaderParameter("REQUEST_METHOD", "POST");
		req->setHeaderParameter("CONTENT_TYPE", "application/x-www-form-urlencoded");
		req->setHeaderParameter("CONTENT_LENGTH", String::number(body->length()) );
		req->setBody( body );

		String v = req->getFormValue("vtest");
		if (v != "dummy")
			throw 0;

		// Test finished, delete Request object
		delete req;
		req = 0;
	} catch(...) {
		if (req)
			delete req;
		throw "FormValues: Failed into POST";
	}

	// Test with urlEnconded datas sended as PATCH
	try {
		String *body = 0;
		body = new String("var1=value1&vtest=dummy&var2=value2");

		req = new Request(0);
		req->setHeaderParameter("REQUEST_METHOD", "PATCH");
		req->setHeaderParameter("CONTENT_TYPE", "application/x-www-form-urlencoded");
		req->setHeaderParameter("CONTENT_LENGTH", String::number(body->length()) );
		req->setBody( body );

		String v = req->getFormValue("vtest");
		if (v != "dummy")
			throw 0;

		// Test finished, delete Request object
		delete req;
		req = 0;
	} catch(...) {
		if (req)
			delete req;
		throw "FormValues: Failed into PATCH";
	}
}

/**
 * @brief Test access to header parameters of the request
 *
 * 1) Try to set and read back one header parameter
 * 2) Try to set and read back multiple header parameters
 */
static void ut_HeaderParameter(void)
{
	Request *req;
	int i;

	// Test with only one header
	req = new Request(0);
	req->setHeaderParameter("testArg", "testValue");
	String t = req->getParam("testArg");
	delete req;
	if (t != "testValue")
		throw "HeaderParameter: one header";

	// Test with multiple headers
	try {
		req = new Request(0);
		for (i = 0; i < 5; ++i)
		{
			String pName, pValue;
			pName  = "testArg"   + String::number(i);
			pValue = "testValue" + String::number(i);
			req->setHeaderParameter(pName, pValue);
		}
		for (i = 0; i < 5; ++i)
		{
			String pName, pWanted;
			pName  = "testArg" + String::number(i);
			pWanted= "testValue" + String::number(i);
			String tval = req->getParam(pName);
			if (tval != pWanted)
				throw i;
		}
		delete req;
	} catch(...) {
		throw "HeaderParameter: multiple header";
	}

}
/* EOF */
