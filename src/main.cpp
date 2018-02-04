/*
 * Hermod - Modular application framework
 *
 * Copyright (c) 2016-2018 Cowlab
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
#include "App.hpp"
#include "config.h"
#include "Config.hpp"
#include "String.hpp"

using namespace hermod;

static int  config(int argc, char **argv);
static void daemonize(void);
static void help(void);
static void signal_handler(int sig);

/**
 * @brief Entry point of hermod
 *
 * @param argc Number of arguments on command line
 * @param argv Pointer to arguments array
 */
int main(int argc, char **argv)
{
	struct sigaction sa;
	int ret;

	ret = config(argc, argv);
	if (ret < 0)
		return(-1);
	else if (ret > 0)
	{
		if (ret == 1)
		{
			help();
			return(0);
		}
	}

	daemonize();

	// Initialise signal handler
	sigemptyset(&sa.sa_mask);
	sa.sa_flags   = 0;
	sa.sa_handler = signal_handler;
	// Install signal handler
	sigaction(SIGINT,  &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);

	hermod::App::getInstance()->init()->exec();
	hermod::App::destroy();

	return(0);
}

/**
 * @brief Load configuration, and parse command line arguments
 *
 * @param argc Number of arguments from command line
 * @param argv Array of arguments
 * @return integer If success a nul value is returned, negative value for error, positive value for special events
 */
static int config(int argc, char **argv)
{
	std::string cfgFilename(DEF_CFG_FILE);
	bool cfgDaemon = true;

	try {
		for (int i = 1; i < argc; i++)
		{
			bool hasMore = ( (i + 1) < argc);
			// -c : specify a config file
			if (std::string("-c").compare(argv[i]) == 0)
			{
				if ( ! hasMore)
					throw std::runtime_error("Missing argument for -c");
				i++;
				cfgFilename = argv[i];
			}
			// -f : strat foreground (do not daemonize)
			if (std::string("-f").compare(argv[i]) == 0)
			{
				cfgDaemon = false;
			}
			// --help : Only display help
			if (std::string("--help").compare(argv[i]) == 0)
				return(1);
		}
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return(-1);
	}

	Config *cfg = Config::getInstance();
	// 
	try {
		cfg->load(cfgFilename);
		std::cout << "Load configuration file " << cfgFilename << std::endl;
	} catch (std::exception &e) {
		std::cerr << "Failed to load config file " << cfgFilename << std::endl;
	}
	// Insert default config
	if ( cfg->get("global", "root_src").isEmpty() )
		cfg->set("global",  "root_src", DEF_DIR_ROOT);
	if ( cfg->get("global", "path_session").isEmpty() )
		cfg->set("global",  "path_session", DEF_DIR_SESS);
	if ( cfg->get("global", "log_file").isEmpty() )
		cfg->set("global", "log_file", DEF_LOG_FILE);
	if ( cfg->get("global", "session_mode").isEmpty() )
		cfg->set("global", "session_mode", "cookie");
	if ( cfg->get("plugins", "directory").isEmpty() )
		cfg->set("plugins", "directory", DEF_DIR_PLUGINS);
	// If the "-f" flag is set, this value override config
	if ( cfgDaemon == false )
		cfg->set("global", "daemon", "no");
	// Else, if no no value has been readed for "daemon", set true
	else if ( cfg->get("global", "daemon").isEmpty() )
		cfg->set("global", "daemon", "yes");

	return(0);
}

/**
 * @brief Detach app from his parent, go background (as daemon)
 *
 */
static void daemonize(void)
{
	try {
		// Get access to config
		Config *cfg = Config::getInstance();
		if (cfg == 0)
			throw 1;

		// Check if this instance need to be run in background
		ConfigKey *key = cfg->getKey("global", "daemon");
		if (key == 0)
			throw 2;
		if (key->getBoolean(true) == false)
			throw 0;
	} catch (...) {
		std::cout << "Start in foreground mode." << std::endl;
		return;
	}
	
	// ok, here we know that we must daemonize
	
	pid_t pid = fork();
	if (pid < 0)
	{
		std::cerr << "Error during fork() ... FATAL" << std::endl;
		exit(-1);
	}
	// End of the parent process
	if (pid > 0)
	{
		std::cerr << "Daemon started (pid=" << pid << ") :-)" << std::endl;
		exit(0);
	}
	
	// The child (now daemon) continue
	close(0);
	close(1);
	close(2);
	return;
}

/**
 * @brief Write on console a message to help using hermod
 *
 */
static void help(void)
{
	std::cout << "Usage: hermod [options]" << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "  -c <file> Load configuration from a specific file" << std::endl;
	std::cout << "  -f        Run in foreground" << std::endl;
	std::cout << "  --help    Display this information" << std::endl;
	std::cout << std::endl;
	std::cout << "Version: 0.2" << std::endl;
	std::cout << "Copyright (C) 2016-2018 Cowlab" << std::endl;
	std::cout << "License: GNU LGPL version 3 <https://www.gnu.org/licenses/lgpl-3.0.html>" << std::endl;
}

/**
 * @brief Function called when a system signal is catch
 *
 * @param sig ID of the received signal
 */
static void signal_handler(int sig)
{
	if (sig == SIGINT)
		hermod::App::sigInt();
	if (sig == SIGTERM)
		hermod::App::sigInt();
}
/* EOF */
