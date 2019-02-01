##
 # Hermod - Modular application framework
 #
 # Copyright (c) 2019 Cowlab
 #
 # Hermod is free software: you can redistribute it and/or modify
 # it under the terms of the GNU Lesser General Public License 
 # version 3 as published by the Free Software Foundation. You
 # should have received a copy of the GNU Lesser General Public
 # License along with this program, see LICENSE file for more details.
 # This program is distributed WITHOUT ANY WARRANTY see README file.
 #
 # Authors: Saint-Genest Gwenael <gwen@hooligan0.net>
 #

CFLAGS = -g -I../../src -Wall -Wextra

DEPS = ../../src/String.o ../../src/Log.o ../../src/Session.o 
DEPS += ../../src/Config.o ../../src/ConfigKey.o

all: hermod
	@echo "  [CC] main.c"
	@g++ $(CFLAGS) -c main.cpp -o main.o
	@echo "  [LD] ut"
	@g++ $(CFLAGS) -o ut main.o $(DEPS) ../../src/Request.o 

hermod:
	make -C ../../src

clean:
	rm -f ut *.o *~
