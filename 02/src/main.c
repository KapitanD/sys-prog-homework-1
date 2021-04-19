#include "cmd.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**
 * @brief Count numer of commands in array, ending with NULL
 * @param cmds Array of commands, ending with NULL
 */
size_t count_cmds(cmd** cmds) {
    size_t i = 0;
    while(1) {
        if (cmds[i] == NULL) {
            return i;
        }
        i++;
    }
}


/**
 * @brief Count number of pipes in cmds array
 * @param cmds Array of commands, ending with NULL
 */
size_t count_pipes(cmd** cmds) {
    size_t i = 0, count = 0;
    while(1) {
        if (cmds[i] == NULL) {
            return count;
        }
        if (strcmp(cmds[i] -> name, "|") == 0) {
            count++;
        }
        i++;
    }
}

int main(int argc, char** argv) {
    int w_stat;
    while(1) {
        // Read comand or several comands from stdin
        cmd** cmds = read_command(stdin);
        if (cmds == NULL)
            break;
        // i - current command index
        size_t i = 0;
        size_t cmds_count = count_cmds(cmds);

        if (cmds_count == 0) {
            break;
        }

        // Processing # comments
        if (cmds_count != 0 && strcmp(cmds[0] -> name, "#") == 0) {
            continue;
        }
        // Count number of pipes and create a pipe for every
        size_t pipes_count = count_pipes(cmds);
        int *pipefds = (int*)malloc(2*pipes_count*sizeof(int));
        if (pipefds == NULL) {
            perror("error: cant create pipe");
            exit(1);
        }

        for (size_t j = 0; j < pipes_count; ++j) {
            if (pipe(pipefds + j * 2) < 0) {
                perror("error: pipe creation failed");
                exit(1);
            }
        }
        // w_stat - status code of last command, init is -1
        w_stat = -1;
        while(1) { // Processing commands
            // init fds for child input and output is standart
            int child_input = 0, child_output = 1;
            // cmds end with NULL
            if (cmds[i] == NULL) {
                break;
            }
            // skip command sep symbols
            if (strcmp(cmds[i] -> name, "|") == 0 
                || strcmp(cmds[i] -> name, ">") == 0 
                || strcmp(cmds[i] -> name, ">>") == 0 
                || strcmp(cmds[i] -> name, "&&") == 0 
                || strcmp(cmds[i] -> name, "||") == 0 ) {
                i++;
                continue;
            }
            // if next cmd is |, redirect output
            if (i + 2 < cmds_count && strcmp(cmds[i + 1] -> name, "|") == 0) {
                child_output = pipefds[i + 1];
            } 
            // if prev cmd is |, redirect input
            if (i != 0 && strcmp(cmds[i - 1] -> name, "|") == 0) {
                child_input = pipefds[i - 2];
            }
            // process cd command
            if (strcmp(cmds[i] -> name, "cd") == 0) {
                int err = chdir(cmds[i] -> argv[1]);
                if (err != 0) {
                    perror("cd error");
                    exit(1);
                }
                i++;
                continue;
            } else if (i + 2 < cmds_count 
                        && strcmp(cmds[i + 1] -> name, ">") == 0) {
                int out_fd = open(cmds[i + 2] -> name, O_WRONLY | O_CREAT 
                                            | O_TRUNC, S_IRUSR | S_IWUSR);
                if (fork() == 0) {
                    // Child process
                    close(1);
                    if (dup2(out_fd, 1) < 0) {
                        perror("error: couldnt get output");
                        exit(1);
                    }
                    if(child_input != 0) {
                        close(0);
                        if(dup2(child_input, 0) < 0) {
                            perror("error: couldnt get input");
                            exit(1);
                        }
                    }
                    for (size_t j = 0; j < pipes_count * 2; ++j) {
                        close(pipefds[j]);
                    }
                    return execvp(cmds[i] -> name, 
                                    (char* const*) cmds[i] -> argv);
                } else {
                    // Parent process
                    close(out_fd);
                    if(child_input != 0)
                        close(child_input);
                    wait(NULL);
                }
                i += 2;
                continue;
            } else if(i + 2 < cmds_count 
                        && strcmp(cmds[i + 1] -> name, ">>") == 0) {
                int out_fd = open(cmds[i + 2] -> name, O_WRONLY | O_APPEND 
                                            | O_CREAT, S_IRUSR | S_IWUSR);
                if (fork() == 0) {
                    // Child process
                    close(1);
                    if (dup2(out_fd, 1) < 0) {
                        perror("error: couldnt get output");
                        exit(1);
                    }
                    if(child_input != 0) {
                        close(0);
                        if(dup2(child_input, 0) < 0) {
                            perror("error: couldnt get input");
                            exit(1);
                        }
                    }
                    for (size_t j = 0; j < pipes_count * 2; ++j) {
                        close(pipefds[j]);
                    }
                    return execvp(cmds[i] -> name, 
                                    (char* const*) cmds[i] -> argv);
                } else {
                    // Parent process
                    close(out_fd);
                    if(child_input != 0)
                        close(child_input);
                    wait(NULL);
                }
                i += 3;
                continue;
            } else if(i + 1 < cmds_count 
                        && strcmp(cmds[i + 1] -> name, "&&") == 0) {
                if (fork() == 0) {
                    // Child process
                    return execvp(cmds[i] -> name, 
                                    (char* const*) cmds[i] -> argv);
                } else {
                    // Parent process
                    if (w_stat == 0) {
                        i += 2;
                        continue;
                    }
                    wait(&w_stat);
                    if (w_stat != 0) {
                        i += 3;
                        continue;
                    }
                }
                i++;
                continue;
            } else if(i + 1 < cmds_count 
                        && strcmp(cmds[i + 1] -> name, "||") == 0) {
                if (fork() == 0) {
                    // Child process
                    return execvp(cmds[i] -> name, 
                                    (char* const*) cmds[i] -> argv);
                } else {
                    // Parent process
                    if (w_stat > 0) {
                        i += 2;
                        continue;
                    }
                    wait(&w_stat);
                    if (w_stat == 0) {
                        i += 3;
                        continue;
                    }
                }
                i++;
                continue;
            } else {
                if (fork() == 0) {
                    // Child process
                    if(child_input != 0) {
                        close(0);
                        if(dup2(child_input, 0) < 0) {
                            perror("error: couldnt get input");
                            exit(1);
                        }
                    }
                    if(child_output != 1) {
                        close(1);
                        if(dup2(child_output, 1) < 0) {
                            perror("error: couldnt get output");
                            exit(1);
                        }
                    }
                    for (size_t j = 0; j < pipes_count * 2; ++j) {
                        close(pipefds[j]);
                    }
                    return execvp(cmds[i] -> name,
                                    (char* const*) cmds[i] -> argv);
                    
                } else {
                    // Parent process
                    if (child_input != 0)
                        close(child_input);
                    if (child_output != 1)
                        close(child_output);
                    while(wait(&w_stat) >= 0);
                }
            }
            i++;
        }
        for (size_t j = 0; j < pipes_count * 2; ++j) {
            close(pipefds[j]);
        }
    }
}
