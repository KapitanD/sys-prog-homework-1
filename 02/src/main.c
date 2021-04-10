#include "cmd.h"
#include <stdio.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

size_t count_cmds(cmd** cmds) {
    size_t i = 0;
    while(1) {
        if (cmds[i] -> argc == 0) {
            return i;
        }
        i++;
    }
    // never reach that point
    return i;
}

int main(int argc, char** argv) {
    int pipefds[2];
    if(pipe(pipefds) != 0) {
        perror("pipe error");
        exit(1);
    }
    while(1) {
        printf("> ");
        cmd** cmds = read_command(stdin);
        size_t i = 0;
        size_t cmds_count = count_cmds(cmds);
        while(1) {
            int child_input = 0, child_output = 1;
            if (cmds[i] -> argc == 0) {
                break;
            }
            // printf("cmd: %s, argv:\n", cmds[i] -> name);
            // for (size_t j = 0; j < count_tokens(cmds[i] -> argv); j++) {
            //     printf("argv[%ld]: %s, ", j, cmds[i] -> argv[j]);
            // }
            // printf("\nExecuting %s...", cmds[i] -> name);
            if (strcmp(cmds[i] -> name, "|") == 0 || strcmp(cmds[i] -> name, ">") == 0 || strcmp(cmds[i] -> name, ">>") == 0 ) {
                i++;
                continue;
            }

            if (i + 2 < cmds_count && strcmp(cmds[i + 1] -> name, "|") == 0) {
                child_output = pipefds[1];
            } 
            if (i != 0 && strcmp(cmds[i - 1] -> name, "|") == 0) {
                child_input = pipefds[0];
            }

            if (strcmp(cmds[i] -> name, "cd") == 0) {
                int err = chdir(cmds[i] -> argv[1]);
                if (err != 0) {
                    perror("cd error");
                    exit(1);
                }
                i++;
                continue;
            } else if(i + 2 < cmds_count && strcmp(cmds[i + 1] -> name, ">") == 0) {
                int outfd = open(cmds[i + 2] -> name, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
                if (fork() == 0) {
                    // Child process
                    dup2(outfd, 1);
                    if(child_input != 0) {
                        dup2(child_input, 0);
                    }
                    return execvp(cmds[i] -> name, (char* const*) cmds[i] -> argv);
                } else {
                    // Parent process
                    wait(NULL);
                }
                i += 2;
                continue;
            } else if(i + 2 < cmds_count && strcmp(cmds[i + 1] -> name, ">>") == 0) {
                int outfd = open(cmds[i + 2] -> name, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
                if (fork() == 0) {
                    // Child process
                    dup2(outfd, 1);
                    if(child_input != 0) {
                        dup2(child_input, 0);
                    }
                    return execvp(cmds[i] -> name, (char* const*) cmds[i] -> argv);
                } else {
                    // Parent process
                    wait(NULL);
                }
                i += 3;
                continue;
            } else {
                if (fork() == 0) {
                    // Child process
                    if(child_input != 0) {
                        dup2(child_input, 0);
                    }
                    if(child_output != 1) {
                        dup2(child_output, 1);
                    }
                    return execvp(cmds[i] -> name, (char* const*) cmds[i] -> argv);
                } else {
                    // Parent process
                    wait(NULL);
                }
            }
            i++;
        }
    }
}
