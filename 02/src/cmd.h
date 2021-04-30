#ifndef _SHELL_COMMANDS_PARSER
#define _SHELL_COMMANDS_PARSER
#include <stdio.h>

struct _cmd {
    const char *name;
    const char **argv;
    int argc;
};

typedef struct _cmd cmd;

cmd** read_command(FILE* input);

size_t count_tokens(const char** tokens);

#endif

