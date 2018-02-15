# hermod #

**Version** 0.3 (currently alpha/dev version)

## About ##

Hermod is a C++ web application server. Depending on target application it can
be seen as a framework or as an application server.

## Documentation ##

* [Hermod configuration file](doc/config_file.md)

* [How to use nginx as frontend](doc/nginx.md)

## Changelog ##

* v0.3 Current "dev" version (unstable)

  * Allow multiple server implementations (FastCGI, native HTTP, ...)

* v0.2 First working alpha version

  * Add class to handle HTML ans JSON contents (response)
  * Improve Router to allow module to register their targets
  * Improve the Session subsystem. Now can work without cookie (use
  application based token)
  * Improve String class to offer high-level string manipulation methods

* v0.1 Project kickoff (concept testing)

The goal of this first version was to test the concept of a modular C++ web
application server. Based on GNU cgicc.