CC	=  cc
CFLAGS	=  -I .
CFLAGS	+= -W -Wall -Wextra -Werror -Wfloat-equal
CFLAGS	+= -Wundef -Wpointer-arith -Wcast-align -Wshadow
CFLAGS	+= -Wstrict-overflow=5 -Wwrite-strings -Waggregate-return
CFLAGS	+= -Wswitch-enum -Wunreachable-code -Winit-self
CFLAGS	+= -Wno-unused-parameter -pedantic -O3
LDFLAGS	= -g

BASE_SOURCES    = src/main.c
SOURCES		= $(BASE_SOURCES)
OBJS		= $(SOURCES:.c=.o)
EXECUTABLE	= bin/shell
EXECUTABLE_DEBUG = bin/shell_debug

all: test

build: $(EXECUTABLE)
build_debug: $(EXECUTABLE_DEBUG)

debug: build_debug
	gdb $(EXECUTABLE_DEBUG)

$(EXECUTABLE): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

$(EXECUTABLE_DEBUG): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

test: build
	python checker.py -e ./$(EXECUTABLE)

clean:
	rm -rf $(EXECUTABLE) $(OBJS)

.PHONY: clean