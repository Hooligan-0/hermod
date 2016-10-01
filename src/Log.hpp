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
#ifndef LOG_HPP
#define LOG_HPP

#include <fstream>
#include <iostream>
#include <sstream>
#include <netinet/in.h>
#include "Session.hpp"

namespace hermod {

/**
 * @class LogCtrl
 * @brief This class allow to create control-messages for log (ex: endl)
 *
 */
class LogCtrl
{
public:
	explicit LogCtrl(int type) { (void)type; }
};

/**
 * @class LogStream
 * @brief The LogStream objects are able to receive and store log datas
 *
 */
class LogStream
{
public:
	LogStream();
	explicit LogStream(int level);
	void append  (const std::string &msg);
	int  getLevel(void);
	void setBuffer(std::string *buffer);
	void setLevel(int level);
public:
	friend LogStream& operator<<(LogStream &ls, const char msg[]);
	friend LogStream& operator<<(LogStream &ls, const std::string &msg);
	friend LogStream& operator<<(LogStream &ls, int);
	friend LogStream& operator<<(LogStream &ls, LogCtrl &ctrl);
	friend LogStream& operator<<(LogStream &ls, struct in_addr &in);
	friend LogStream& operator<<(LogStream &ls, Session *sess);
	friend LogStream& operator<<(LogStream &ls, void *ptr);
private:
	int mLevel;
	std::string  mLine;
	std::string *mBuffer;
};

/**
 * @class Log
 * @brief A global logging system for Hermod
 *
 */
class Log
{
public:
	~Log();
	static void destroy();
	static Log* getInstance();
	static void setFile(const std::string &filename);
	static void sync(void);
public:
	static LogStream &error  (void);
	static LogStream &warning(void);
	static LogStream &info   (void);
	static LogStream &debug  (void);
protected:
	void writeToFile(const std::string &msg);
private:
	Log();
public:
	static LogCtrl   endl;
private:
	static Log*      mInstance;
private:
	std::fstream mFile;
	std::string  mFilename;
	std::string  mBuffer;
	LogStream    mDebug;
	LogStream    mInfo;
	LogStream    mWarning;
	LogStream    mError;
};

} // namespace hermod
#endif
