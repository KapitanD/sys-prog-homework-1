#include "cmd.h"
#include <stdio.h>

int main(int argc, char** argv) {
    while(1) {
        printf("> ");
        cmd** cmds = read_command(stdin);
        int i = 0;
        while(1) {
            if (cmds[i] -> argc == 0) {
                break;
            }
            printf("cmd: %s, argv:\n", cmds[i] -> name);
            for (size_t j = 0; j < count_tokens(cmds[i] -> argv); j++) {
                printf("argv[%ld]: %s, ", j, cmds[i] -> argv[j]);
            }
            printf("\n");
            i++;
        }
    }
}
