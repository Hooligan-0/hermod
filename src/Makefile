##
 # Hermod - Modular application framework
 #
 # Copyright (c) 2016-2019 Cowlab
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
TARGET = hermod
SRC  = main.cpp App.cpp Config.cpp ConfigKey.cpp Log.cpp Request.cpp String.cpp
SRC += Module.cpp ModuleCache.cpp
SRC += Router.cpp Route.cpp RouteTarget.cpp
SRC += Page.cpp Session.cpp SessionCache.cpp
SRC += Server.cpp ServerFastcgi.cpp ServerLibFcgi.cpp
SRC += Content.cpp
SRC += ContentHtml.cpp ContentHtml/HtmlElement.cpp ContentHtml/HtmlAttribute.cpp
SRC += ContentHtml/HtmlTag.cpp ContentHtml/HtmlHtml.cpp ContentHtml/HtmlH.cpp
SRC += ContentHtml/Template.cpp
SRC += ContentJson.cpp ContentJson/JsonElement.cpp ContentJson/JsonObject.cpp
SRC += ContentJson/JsonArray.cpp ContentJson/JsonString.cpp
SRC += Response.cpp ResponseHeader.cpp

INSTALL="/usr/local"

CC = c++
CFLAGS   = -Wall -Wextra -Wuninitialized -Wunused-label -Wunused-value -Wunused-variable -Wno-unknown-pragmas
CFLAGS  += -g
CFLAGS  += -DINSTALL=\"$(INSTALL)\"
LDFLAGS  = -lfcgi -lfcgi++
LDFLAGS += -ldl -rdynamic
CHECK  = --enable=warning
CHECK += --enable=performance
CHECK += --enable=style

_COBJ =  $(SRC:.cpp=.o)
COBJ = $(patsubst %, %,$(_COBJ))

all: $(COBJ)
	@echo "  [LD] $(TARGET)"
	@$(CC) $(CFLAGS) -o $(TARGET) $(COBJ) $(DBOBJ) $(LDFLAGS)

install:
	@echo "  [CP] Install binary file (" $(TARGET) ")"
	@cp $(TARGET) $(INSTALL)/bin/
	@echo "  [CP] Install headers files (*.hpp)"
	@cp *.hpp $(INSTALL)/include/hermod/
	@cp ContentHtml/*.hpp $(INSTALL)/include/hermod/ContentHtml/
	@cp ContentJson/*.hpp $(INSTALL)/include/hermod/ContentJson/

check:
	@echo "  [CPPCHECK] ."
	cppcheck $(CHECK) --quiet .
	
clean:
	@echo "  [RM] $(TARGET) *.o"
	@rm -f $(TARGET) $(COBJ)
	@echo "  [RM] temporary files (*~)"
	@rm -f *~ ContentHtml/*~ ContentJson/*~

$(COBJ) : %.o : %.cpp
	@echo "  [CC] $@"
	@$(CC) $(CFLAGS) -c $< -o $@
