# Makefile for Remote Execution simulation
# By B22CS061 & B22CS062

CC = g++
CFLAGS = -Wall -std=c++11 $(shell opp_includepath)
LDFLAGS = $(shell opp_libs)

# Source files
SOURCES = $(wildcard src/*.cc) $(wildcard src/*_m.cc)
OBJECTS = $(SOURCES:.cc=.o)
EXECUTABLE = remoteexecution

# Message files
MSG_FILES = src/RemoteExecution.msg
MSG_CC_FILES = $(MSG_FILES:.msg=_m.cc)
MSG_H_FILES = $(MSG_FILES:.msg=_m.h)

all: $(MSG_CC_FILES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.cc
	$(CC) -c $(CFLAGS) $< -o $@

%_m.cc %_m.h: %.msg
	opp_msgc $<

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) $(MSG_CC_FILES) $(MSG_H_FILES)

.PHONY: all clean 