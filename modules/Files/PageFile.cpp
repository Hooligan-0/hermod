/*
 * modFiles - An Hermod module to access static files
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
#include <iostream>
#include <fstream>
#include <string>
#include "PageFile.hpp"
#include "Config.hpp"
#include "File.hpp"
#include "Directory.hpp"

namespace hermod {
	namespace Files {

PageFile::PageFile(void)
    : Page()
{
	// Nothing to do
}

int PageFile::process(void)
{
	File reqFile;
	
	Config  *cfg = Config::getInstance();
	// Get the root path for files
	std::string fileRoot = cfg->get("mod:Files", "root");
	if (fileRoot.empty())
		throw runtime_error("Missing config (root)");
	
	reqFile.setRoot(fileRoot);
	
	// Compute relative (sub-)path of the file
	for (int i = 1; i < getArgCount(); i++)
		reqFile.addPath( getArg(i) );
	
	// Get the file name (last argument)
	if (getArgCount() > 0)
		reqFile.setName( getArg( getArgCount() ) );
	
	if (reqFile.isDirectory())
		directoryListing(reqFile);
	else
		fileDisplay(reqFile);
	
	return(0);
}

void PageFile::directoryListing(File &dirFile)
{
	Directory reqDir(dirFile);

	response()->header()->setContentType("text/html");
	cout << "<html>";
	cout <<   "<head><title>Directory listing</title></head>";
	cout <<   "<body>";
	cout <<     "<h1>Directory listing " << reqDir.getName() << "</h1>";
	cout <<     "<hr>";
	for (int i = 0; i < reqDir.count(); ++i)
	{
		File *f = reqDir.at(i);
		cout << "<li>";
		cout << f->getName();
		cout << "</li>";
	}
	cout <<   "</body>";
	cout << "</html>" << endl;
}

void PageFile::fileDisplay(File &file)
{
	std::string filename = file.getFullname();
	
	// Open file
	std::fstream dataFile;
	dataFile.open(filename.c_str(), std::ios::in|std::ios::binary);
	if ( ! dataFile.is_open() )
		return;
	
	// Read file content
	std::vector<unsigned char> data;
	data.insert(data.begin(),
	            std::istreambuf_iterator<char>(dataFile),
	            std::istreambuf_iterator<char>() );
	// Close file
	dataFile.close();
	
	response()->header()->setContentType("text/plain");

	std::string strData(data.begin(), data.end());
	cout << strData;
}

	} // namespace Files
} // namespace hermod
