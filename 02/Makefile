CC	=  cc
CFLAGS	=  -I .
CFLAGS	+= -W -Wall -Wextra -Werror -Wfloat-equal
CFLAGS	+= -Wundef -Wpointer-arith -Wcast-align -Wshadow
CFLAGS	+= -Wstrict-overflow=5 -Wwrite-strings -Waggregate-return
CFLAGS	+= -Wswitch-enum -Wunreachable-code -Winit-self
CFLAGS	+= -Wno-unused-parameter -pedantic -O3
LDFLAGS	=

MKDIR_P = mkdir -p
BASE_SOURCES    = src/main.c src/cmd_parser.c
SOURCES		= $(BASE_SOURCES)
OBJS		= $(SOURCES:.c=.o)
EXECUTABLE	= bin/shell
EXECUTABLE_DEBUG = bin/shell_debug
BIN_DIR = bin

all: test

directories: ${BIN_DIR}

${BIN_DIR}:
	${MKDIR_P} ${BIN_DIR}

build: directories $(EXECUTABLE) 
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
	python3 test/checker.py -e ./$(EXECUTABLE) --max $(max)

clean:
	rm -rf $(EXECUTABLE) $(OBJS)

.PHONY: clean