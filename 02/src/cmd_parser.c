#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <string.h>
#include "cmd.h"


/**
 * @brief Make new cmd with proper size of argv
 * @param size Size of argv of new cmd
 */
cmd* make_cmd(size_t size) {
    cmd* tmp = malloc(sizeof(cmd));
    tmp -> argv = malloc(size * sizeof(char*));
    return tmp;
}


/**
 * @brief Make new cmd with only one arg - it's name
 * @param name Name of new cmd
 */
cmd* make_single_arg_cmd(const char* name) {
    cmd* tmp = malloc(sizeof(cmd));
    tmp -> name = name;
    tmp -> argv = malloc(sizeof(cmd));
    tmp -> argv[0] = name;
    tmp -> argc = 1;
    return tmp;
}


/**
 * @brief Modify cmd to prepare it to store
 * @param cur_cmd Cmd to modify
 * @param cmd_argc Var with current argc for cmd
 */
void finalize_cmd(cmd* cur_cmd, size_t* cmd_argc) {
    cur_cmd -> argc = *(cmd_argc);
    cur_cmd -> argv[*(cmd_argc)] = NULL;
    cur_cmd -> name = cur_cmd -> argv[0];
    *(cmd_argc) = 0;
}


/**
 * @brief Double buffer size with realloc, modify bufsize var
 * @param bufsize Size of the expanded buffer, doubles in this func
 * @param buffer Pointer to buffer to expand
 * @param type_size Size of the data type stored in the buffer
 */
void* expand_buffer(size_t* bufsize, void* buffer, size_t type_size) {
    *(bufsize) += *(bufsize);
    buffer = realloc(buffer, *(bufsize) * type_size);
    if (!buffer) {
        perror("buffer realloc error");
        exit(1);
    }
    return buffer;
}


/**
 * @brief Append element to end of buffer for char
 * @param c Char to append
 * @param buffer The pointer to buffer to add the item to. Change
 * @param pos The pointer to index of next to last elem of buffer. Change
 * @param bufsize The pointer to size of the buffer. May change
 */
void* append_bufferc(char c, char* buffer, size_t* pos, size_t* bufsize) {
    buffer[*(pos)] = c;
    *(pos) += 1;
    if (*(pos) >= *(bufsize)) {
        buffer = expand_buffer(bufsize, buffer, sizeof(char));
    }
    return buffer;
}


/**
 * @brief Append element to end of buffer for cmd
 * @param c Char to append
 * @param buffer The pointer to buffer to add the item to. Change
 * @param pos The pointer to index of next to last elem of buffer. Change
 * @param bufsize The pointer to size of the buffer. May change
 */
void* append_buffercmd(cmd* c, cmd** buffer, size_t* pos, size_t* bufsize) {
    buffer[*(pos)] = c;
    *(pos) += 1;
    if (*(pos) >= *(bufsize)) {
        buffer = expand_buffer(bufsize, buffer, sizeof(cmd));
    }
    return buffer;
}

/**
 * @brief Counts the number of tokens in the array
 * @param tokens Array of tokens, ending up with NULL
 */
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

/**
 * @brief Stores token in buffer of tokens
 * @param tokens Buffer to store cur token
 * @param token Current token
 * @param pos_token Pointer to next to last index in token
 * @param pos_tokens Pointer to next to last index in buffer
 * @param tokens_bufsize Pointer to size of buffer
 * @param token_bufsize Pointer to size of token buffer
 */
char* store_token(const char** tokens, char* token, size_t* pos_token, 
                    size_t* pos_tokens, size_t* tokens_bufsize, 
                    size_t* token_bufsize) {
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

/**
 * @brief Get tokens from input file
 * @param input File to get tokens from
 */
const char** get_tokens(FILE* input) {
    int c = 0;

    size_t token_bufsize = 64, 
            tokens_bufsize = 64, pos_token = 0, pos_tokens = 0;

    const char **tokens = malloc(tokens_bufsize * sizeof(char*));
    char *token = malloc(token_bufsize * sizeof(char));

    int is_single_quotted = 0, is_double_qoutted = 0, is_shielded = 0;

    if (!tokens) {
        perror("allocation error");
        exit(1);
    }

    while(c != EOF) {
        c = fgetc(input);

    switch (c) {
        case EOF:
            break;
        case '"':
            if (is_shielded || is_single_quotted) {
                is_shielded = 0;
                token = append_bufferc(c, token, &pos_token, &token_bufsize);
            } else if (!is_double_qoutted) {
                is_double_qoutted = 1;
            } else {
                is_double_qoutted = 0;
            }
            break;
        case '\'':
            if (is_double_qoutted) {
                token = append_bufferc(c, token, &pos_token, &token_bufsize);
            } else if (!is_single_quotted) {
                is_single_quotted = 1;
            } else {
                is_single_quotted = 0;
            }
            break;
        case '\\':
            if (is_shielded || is_single_quotted) {
                is_shielded = 0;
                token = append_bufferc(c, token, &pos_token, &token_bufsize);
            } else {
                is_shielded = 1;
            }
            break;
        case ' ':
        case '\t':
            if (is_single_quotted || is_double_qoutted || is_shielded) {
                token = append_bufferc(c, token, &pos_token, &token_bufsize);
                is_shielded = 0;
            } else {
                token = store_token(tokens, token, &pos_token, &pos_tokens,
                                            &tokens_bufsize, &tokens_bufsize);
            }
            break;
        case '\n':
            if (is_shielded) {
                is_shielded = 0;
            } else if (is_single_quotted || is_double_qoutted) {
                token = append_bufferc(c, token, &pos_token, &token_bufsize);
            } else {
                if (pos_token == 0 && pos_tokens == 0)
                    token = append_bufferc(c, token, &pos_token, 
                                            &token_bufsize);
                c = EOF;
            }
            break;
        case '>':
        case '|':
        case '&':
            if (is_single_quotted || is_double_qoutted || is_shielded) {
                is_shielded = 0;
                token = append_bufferc(c, token, &pos_token, &token_bufsize);
            } else {
                if (token[pos_token - 1] == c) {
                    token = append_bufferc(c, token, &pos_token,
                                            &token_bufsize);
                    token = store_token(tokens, token, &pos_token, &pos_tokens,
                                            &tokens_bufsize, &token_bufsize);
                } else {
                    token = store_token(tokens, token, &pos_token, &pos_tokens,
                                            &tokens_bufsize, &token_bufsize);
                    token = append_bufferc(c, token, &pos_token, 
                                            &token_bufsize);
                }
            }
            break;
        default:
            if (token[pos_token - 1] == '>' || token[pos_token - 1] == '|' 
                                            || token[pos_token - 1] == '&') {
                token = store_token(tokens, token, &pos_token, &pos_tokens,
                                            &tokens_bufsize, &token_bufsize);
            }
            token = append_bufferc(c, token, &pos_token, &token_bufsize);
        }
    }
    token = store_token(tokens, token, &pos_token, &pos_tokens, 
                                            &tokens_bufsize, &tokens_bufsize);
    tokens[pos_tokens] = NULL;
    return tokens;
}

/**
 * @brief Get tokens from input and conver it into commands array
 * @param input Input file to get tokens from
 */
cmd** read_command(FILE* input) {
    const char** tokens = get_tokens(input);

    size_t tokens_size = count_tokens(tokens),
            cmds_bufsize = 64, cmds_pos = 0, i = 0, cmd_argc = 0;

    cmd** cmds = malloc(cmds_bufsize * sizeof(cmd*));
    cmd* cur_cmd = make_cmd(tokens_size);

    while(i < tokens_size) {
        const char* token = tokens[i];
        int pred = (strcmp(token, "|") == 0) || (strcmp(token, ">>") == 0) 
                        || (strcmp(token, ">") == 0) 
                        || (strcmp(token, "&&") == 0) 
                        || (strcmp(token, "||") == 0);
        if (pred) {
            finalize_cmd(cur_cmd, &cmd_argc);
            
            cmds = append_buffercmd(cur_cmd, cmds, &cmds_pos, &cmds_bufsize);
            cur_cmd = make_single_arg_cmd(token);

            cmds = append_buffercmd(cur_cmd, cmds, &cmds_pos, &cmds_bufsize);
            cur_cmd = make_cmd(tokens_size);
        } else {
            cur_cmd -> argv[cmd_argc] = token;
            cmd_argc++;
        }
        i++;
    }
    finalize_cmd(cur_cmd, &cmd_argc);

    cmds = append_buffercmd(cur_cmd, cmds, &cmds_pos, &cmds_bufsize);
    cmds[cmds_pos] = NULL;

    return cmds;
}