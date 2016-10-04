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
#include <ctime>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <sstream>
#include "Log.hpp"

namespace hermod {

Log *       Log::mInstance = NULL;
LogCtrl     Log::endl(0);

/**
 * @brief Default (private) contructor for Log object
 *
 */
Log::Log()
{
	mBuffer.clear();
	// Initialize 'error' stream
	mError.setLevel(99);
	mError.setBuffer( &mBuffer );
	// Initialize 'warning' stream
	mWarning.setLevel(30);
	mWarning.setBuffer( &mBuffer );
	// Initialize 'info' stream
	mInfo.setLevel(20);
	mInfo.setBuffer( &mBuffer );
	// Initialize 'debug' stream
	mDebug.setLevel(10);
	mDebug.setBuffer( &mBuffer );
}

/**
 * @brief Default destructor of Log objects
 *
 */
Log::~Log()
{
}

/**
 * @brief Destroy the global Log cache
 *
 */
void Log::destroy(void)
{
	if ( ! mInstance)
		return;
	// Free singleton object
	delete mInstance;
	mInstance = NULL;
}

/**
 * @brief Get a pointer to the cache
 *
 * The Log system works with a single object into application. To allow only
 * one instance (singleton) object constructor is private. This method is used
 * to get access to this static, unique variable.
 *
 * @return Log* Pointer to the (singleton) cache object
 */
Log* Log::getInstance()
{
	if ( ! mInstance)
	{
		mInstance = new Log;
	}
	return mInstance;
}

/**
 * @brief Get the debug stream
 *
 * The logging system is composed of serval streams. Each of them for a specific
 * kind of messages (based on a level mechanism). This static method return the
 * 'debug' stream of global Log.
 *
 * @return LogStream& Reference to debug stream
 */
LogStream &Log::debug(void)
{
	// Get the Log singleton object
	Log *l = getInstance();
	// Return the debug LogStream
	return l->mDebug;
}

/**
 * @brief Get the error stream
 *
 * The logging system is composed of serval streams. Each of them for a specific
 * kind of messages (based on a level mechanism). This static method return the
 * 'error' stream of global Log.
 *
 * @return LogStream& Reference to error stream
 */
LogStream &Log::error(void)
{
	// Get the Log singleton object
	Log *l = getInstance();
	// Return the error LogStream
	return l->mError;
}

/**
 * @brief Get the info stream
 *
 * The logging system is composed of serval streams. Each of them for a specific
 * kind of messages (based on a level mechanism). This static method return the
 * 'info' stream of global Log.
 *
 * @return LogStream& Reference to info stream
 */
LogStream &Log::info(void)
{
	// Get the Log singleton object
	Log *l = getInstance();
	// Return the info Logstream
	return l->mInfo;
}

/**
 * @brief Get the warning stream
 *
 * The logging system is composed of serval streams. Each of them for a specific
 * kind of messages (based on a level mechanism). This static method return the
 * 'warning' stream of global Log.
 *
 * @return LogStream& Reference to warning stream
 */
LogStream &Log::warning(void)
{
	// Get the Log singleton object
	Log *l = getInstance();
	// Return the warning LogStream
	return l->mWarning;
}

/**
 * @brief Set the file where logs must be written
 *
 * @param filename Name of the file to use
 */
void Log::setFile(const std::string &filename)
{
	// Sanity check
	if (filename.empty())
		throw std::runtime_error("Log: setFile without filename");

	// Get the Log singleton object
	Log *l = getInstance();

	// Close current Log file (if any)
	if ( l->mFile.is_open() )
		l->mFile.close();
	
	// Set new file name
	l->mFilename = filename;

	// Open the target file
	l->mFile.open(l->mFilename.c_str(), std::ios::out | std::ios::trunc);
	if ( ! l->mFile.is_open())
	{
		l->mFilename.clear();
		throw std::runtime_error("Log: setFile could not open the file!");
	}
}

/**
 * @brief Flush the messages from memory to output (file or standard output)
 *
 * For better performances, log messages are not directly written. All streams
 * put datas into a global buffer into memory. This static method is used to
 * request a flush from memory to target output.
 */
void Log::sync(void)
{
	Log *l = getInstance();

	if (l->mFile.is_open())
		l->writeToFile(l->mBuffer);
	else
		std::cout << l->mBuffer << std::flush;
	l->mBuffer.clear();
}

/**
 * @brief Write data to log file
 *
 * When an output file is defined, this method is called (by sync) to write data
 * on disk.
 *
 * @param msg Reference to log string
 */
void Log::writeToFile(const std::string &msg)
{
	mFile << msg;
	mFile.flush();
}

// --------------------  -------------------- //

/**
 * @brief Default constructor
 *
 */
LogStream::LogStream()
{
	mBuffer = 0;
	mLevel  = 0;
}

/**
 * @brief Constructor for a defined log level
 *
 * @param level The log level of this new stream
 */
LogStream::LogStream(int level)
{
	mBuffer = 0;
	mLevel  = level;
}

/**
 * @brief Append a std::string to the log stream
 *
 * @param msg An std::string to append
 */
void LogStream::append(const std::string &msg)
{
	mLine.append(msg);
}

/**
 * @brief Get the current level for this stream
 *
 * @return integer The current log level
 */
int LogStream::getLevel(void) const
{
	return(mLevel);
}

/**
 * @brief Set the output buffer
 *
 * @param buffer Pointer to the output buffer to use
 */
void LogStream::setBuffer(std::string *buffer)
{
	mBuffer = buffer;
}

/**
 * @brief Set a new log level for the stream
 *
 * @param level The new log level to set
 */
void LogStream::setLevel(int level)
{
	mLevel = level;
}

/**
 * @brief Overload << operator to append a c-string
 *
 * @param ls  Reference to the current LogStream
 * @param msg The c-string to append
 * @return LogStream& Reference to the resulting LogStream (same as input)
 */
LogStream& operator<<(LogStream &ls, const char msg[])
{
	std::ostringstream oss;
	oss << msg;
	ls.append( oss.str() );
	return ls;
}

/**
 * @brief Overload << operator to append a String
 *
 * @param ls  Reference to the current LogStream
 * @param msg The String to append
 * @return LogStream& Reference to the resulting LogStream (same as input)
 */
LogStream& operator<<(LogStream &ls, const String &msg)
{
	std::ostringstream oss;
	oss << msg.data();
	ls.append( oss.str() );
	return ls;
}

/**
 * @brief Overload << operator to append a std::string
 *
 * @param ls  Reference to the current LogStream
 * @param msg The std::string to append
 * @return LogStream& Reference to the resulting LogStream (same as input)
 */
LogStream& operator<<(LogStream &ls, const std::string &msg)
{
	std::ostringstream oss;
	oss << msg;
	ls.append( oss.str() );
	return ls;
}

/**
 * @brief Overload << operator to append an integer value (as text)
 *
 * @param ls  Reference to the current LogStream
 * @param msg The integer value to append
 * @return LogStream& Reference to the resulting LogStream (same as input)
 */
LogStream& operator<<(LogStream &ls, int i)
{
	std::ostringstream oss;
	oss << i;
	ls.append( oss.str() );
	return ls;
}

/**
 * @brief Overload << operator to call a LogCtrl command
 *
 * @param ls  Reference to the current LogStream
 * @param ctrl The LogCtrl command to execute
 * @return LogStream& Reference to the resulting LogStream (same as input)
 */
LogStream& operator<<(LogStream &ls, LogCtrl &ctrl)
{
	// ToDo: decode ctrl value
	(void)ctrl;

	time_t t = time(0);
	struct tm now;
	localtime_r(&t, &now);
	std::ostringstream oss;
	// Insert timestamp
	oss << std::setw(2) << std::setfill('0') << now.tm_hour << ":";
	oss << std::setw(2) << std::setfill('0') << now.tm_min  << ":";
	oss << std::setw(2) << std::setfill('0') << now.tm_sec  << " ";
	// Insert the level code
	if (ls.mLevel <= 10)
		oss << "DBG ";
	else if (ls.mLevel <= 20)
		oss << "INF ";
	else if (ls.mLevel <= 30)
		oss << "WRN ";
	else
		oss << "ERR ";
	// Insert the log line
	oss << ls.mLine << "\n";
	
	if (ls.mBuffer)
		ls.mBuffer->append( oss.str() );
	ls.mLine.clear();
	return ls;
}

/**
 * @brief Overload << operator to append an IP address
 *
 * @param ls Reference to the current LogStream
 * @param in The in_addr strucure that contain the IP to append
 * @return LogStream& Reference to the resulting LogStream (same as input)
 */
LogStream& operator<<(LogStream &ls, struct in_addr &in)
{
	std::ostringstream oss;
	unsigned long ip = in.s_addr;
	
	oss << (unsigned long)((ip >>  0) & 0xFF) << ".";
	oss << (unsigned long)((ip >>  8) & 0xFF) << ".";
	oss << (unsigned long)((ip >> 16) & 0xFF) << ".";
	oss << (unsigned long)((ip >> 24) & 0xFF);
	ls.append(oss.str());
	return ls;
}

/**
 * @brief Overload << operator to append a Session id
 *
 * @param ls Reference to the current LogStream
 * @param sess Pointer to the Session
 * @return LogStream& Reference to the resulting LogStream (same as input)
 */
LogStream& operator<<(LogStream &ls, Session *sess)
{
	std::ostringstream oss;
	oss << sess->getId() << " ";
	ls.append(oss.str());
	return ls;
}

/**
 * @brief Overload << operator to append a pointer address (mainly for debug)
 *
 * @param ls Reference to the current LogStream
 * @param ptr Pointer to an arbitrary address
 * @return LogStream& Reference to the resulting LogStream (same as input)
 */
LogStream& operator<<(LogStream &ls, void *ptr)
{
	const void * address = static_cast<const void*>(ptr);
	
	std::ostringstream oss;
	oss << address << " ";
	ls.append(oss.str());
	return ls;
}

} // namespace hermod
/* EOF */
