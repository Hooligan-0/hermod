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
#ifndef RESPONSEHEADER_HPP
#define RESPONSEHEADER_HPP
#include <map>
#include "String.hpp"

namespace hermod {

/**
 * @class ResponseHeader
 * @brief This class contains the header of a Response
 *
 */
class ResponseHeader {
public:
	ResponseHeader();
	void addHeader(const String &key, String value);

	void setContentType(const String &type);
	void setRetCode(int code, const String &reason);

	String getHeader(void);

private:
	int    mRetCode;
	String mRetReason;
	String mContentType;
	std::map <String, String> mHeaders;
};

} // namespace hermod
#endif
