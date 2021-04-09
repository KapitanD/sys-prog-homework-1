#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <string.h>
#include "cmd.h"

cmd* make_cmd(size_t size) {
    cmd* tmp = malloc(sizeof(cmd));
    tmp -> argv = malloc(size * sizeof(char*));
    return tmp;
}

cmd* make_empty_arg_cmd(const char* name) {
    cmd* tmp = malloc(sizeof(cmd));
    tmp -> name = name;
    tmp -> argv = malloc(sizeof(cmd));
    tmp -> argv[0] = name;
    tmp -> argc = 1;
    return tmp;
}

void* expand_buffer(size_t* bufsize, void* buffer, size_t type_size) {
    *(bufsize) += *(bufsize);
    buffer = realloc(buffer, *(bufsize) * type_size);
    if (!buffer) {
        perror("buffer realloc error");
        exit(1);
    }
    return buffer;
}

void* append_bufferc(char c, char* buffer, size_t* pos, size_t* bufsize) {
    buffer[*(pos)] = c;
    *(pos) += 1;
    if (*(pos) >= *(bufsize)) {
        buffer = expand_buffer(bufsize, buffer, sizeof(char));
    }
    return buffer;
}

void* append_buffercmd(cmd* c, cmd** buffer, size_t* pos, size_t* bufsize) {
    buffer[*(pos)] = c;
    *(pos) += 1;
    if (*(pos) >= *(bufsize)) {
        buffer = expand_buffer(bufsize, buffer, sizeof(cmd));
    }
    return buffer;
}

size_t count_tokens(const char** tokens) {
    size_t i = 0;
    while (1) {
        if (tokens[i] == NULL) {
            break;
        }
        i++;
    }
    return i;
}

char* store_token(const char** tokens, char* token, size_t* pos_token, size_t* pos_tokens, size_t* tokens_bufsize, size_t* token_bufsize) {
    if (*(pos_token) != 0) {
        token = append_bufferc('\0', token, pos_token, token_bufsize);
        tokens[*(pos_tokens)] = token;
        token = malloc(*(token_bufsize) * sizeof(char));
        *(pos_token) = 0;
        *(pos_tokens) += 1;
        if (*(pos_tokens) >= *(tokens_bufsize)) {
            token = expand_buffer(tokens_bufsize, tokens, sizeof(char*));
        }
        return token;
    }
    return token;
}

const char** get_tokens(FILE* input) {
    int c = EOF;

    size_t token_bufsize = 64, tokens_bufsize = 64, pos_line = 0, pos_token = 0, pos_tokens = 0;

    const char **tokens = malloc(tokens_bufsize * sizeof(char*));
    char *token = malloc(token_bufsize * sizeof(char));

    int is_single_quotted = 0, is_double_qoutted = 0, is_shielded = 0;

    if (!tokens) {
        perror("allocation error");
        exit(1);
    }

    while(c) {
        c = fgetc(input);

        if (c == EOF) {
            break;
        }

        else if (c == '"') {
            if (is_shielded || is_single_quotted) {
                is_shielded = 0;
                token = append_bufferc(c, token, &pos_token, &token_bufsize);
            } else if (!is_double_qoutted) {
                is_double_qoutted = 1;
            } else {
                is_double_qoutted = 0;
            }
        }

        else if (c == '\'') {
            if (is_double_qoutted) {
               token = append_bufferc(c, token, &pos_token, &token_bufsize);
            } else if (!is_single_quotted) {
                is_single_quotted = 1;
            } else {
                is_single_quotted = 0;
            }
        }

        else if (c == '\\') {
            if (is_shielded || is_single_quotted) {
                is_shielded = 0;
                token = append_bufferc(c, token, &pos_token, &token_bufsize);
            } else {
                is_shielded = 1;
            }
        }

        else if (c == ' ' || c == '\t') {
            if (is_single_quotted || is_double_qoutted || is_shielded) {
                token = append_bufferc(c, token, &pos_token, &token_bufsize);
                is_shielded = 0;
            } else {
                token = store_token(tokens, token, &pos_token, &pos_tokens, &tokens_bufsize, &tokens_bufsize);
            }
        }

        else if (c == '\n') {
            if (is_single_quotted || is_double_qoutted || is_shielded) {
                is_shielded = 0;
            } else {
                break;
            }
        }

        else if(c == '|') {
            if (is_single_quotted || is_double_qoutted || is_shielded) {
                is_shielded = 0;
                token = append_bufferc(c, token, &pos_token, &token_bufsize);
            } else {
                token = store_token(tokens, token, &pos_token, &pos_tokens, &tokens_bufsize, &token_bufsize);
                token = append_bufferc(c, token, &pos_token, &token_bufsize);
                token = store_token(tokens, token, &pos_token, &pos_tokens, &tokens_bufsize, &token_bufsize);
            }
        }

        else if(c == '>') {
            if (is_single_quotted || is_double_qoutted || is_shielded) {
                is_shielded = 0;
                token = append_bufferc(c, token, &pos_token, &token_bufsize);
            } else {
                if (token[pos_token - 1] == '>') {
                    token = append_bufferc(c, token, &pos_token, &token_bufsize);
                    token = store_token(tokens, token, &pos_token, &pos_tokens, &tokens_bufsize, &token_bufsize);
                } else {
                    token = append_bufferc(c, token, &pos_token, &token_bufsize);
                }
            }
        }

        else {
            token = append_bufferc(c, token, &pos_token, &token_bufsize);
        }

        pos_line++;
    }
    token = store_token(tokens, token, &pos_token, &pos_tokens, &tokens_bufsize, &tokens_bufsize);
    tokens[pos_tokens] = NULL;
    return tokens;
}

cmd** read_command(FILE* input) {
    const char** tokens = get_tokens(input);
    size_t tokens_size = count_tokens(tokens), cmds_bufsize = 64, cmds_pos = 0, i = 0, cmd_argc = 0;

    cmd** cmds = malloc(cmds_bufsize * sizeof(cmd*));
    cmd* cur_cmd = make_cmd(tokens_size);

    while(i < tokens_size) {
        const char* token = tokens[i];
        if (strcmp(token, "|") == 0 || strcmp(token, ">>") == 0 || strcmp(token, ">") == 0) {
            cur_cmd -> argc = cmd_argc;
            cmd_argc = 0;
            cur_cmd -> name = cur_cmd -> argv[0];
            cmds = append_buffercmd(cur_cmd, cmds, &cmds_pos, &cmds_bufsize);
            cur_cmd = make_empty_arg_cmd(token);
            cmds = append_buffercmd(cur_cmd, cmds, &cmds_pos, &cmds_bufsize);
            cur_cmd = make_cmd(tokens_size);
        } else {
            cur_cmd -> argv[cmd_argc] = token;
            cmd_argc++;
        }
        i++;
    }
    cur_cmd -> argc = i;
    cur_cmd -> name = cur_cmd -> argv[0];
    cmds = append_buffercmd(cur_cmd, cmds, &cmds_pos, &cmds_bufsize);
    cmds[cmds_pos] = make_cmd(0);
    return cmds;
}