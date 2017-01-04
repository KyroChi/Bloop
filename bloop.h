/*
 *                               B L O O P . H
 *
 */
#include <stdio.h>
#include <stdlib.h>

#define BLOOP_MAX_BUFFER_SIZE 1024
#define BLOOP_MAX_INPUT_ARGS 32
#define BLOOP_MAX_ARG_LENGTH 64
#define BLOOP_MAX_HISTORY_DEPTH 10
#define BLOOP_MAX_VARIABLE_NAME 64

/* Terminal Escapes */
#define DEL 0x7F

struct _builtin {
        char *name;
        int code;
        //void *function(void);
};

/* Builtin lookup table */
static const struct _builtin builtins[] = {
        {"exit", 1},
        {"history", 2},
        {"help", 3}
};

/*
 * Manage history as a linked list
 *
 * Declare global HISTORY variable that is common for the current bloop instance
 */
typedef struct history_entry_s {
        char *command;
        struct history_entry_s *next;
        struct history_entry_s *prev;
} history_t;

history_t *HISTORY;
history_t *HISTORY_HEAD;

/*
 * Error handling for shell errors.
 */
void
_bloop_error (char *message, int exit_code) {
        fflush(stdout);
        fprintf(stderr, "%s\n", message);
        fflush(stderr);
        exit(exit_code);
}

#ifdef DEBUG
#define _ASSERT(x, message) if (!x) {_bloop_error(message, 9);}
#else
#define _ASSERT(x, message)
#endif

int shell_loop (void);
void shell_init (void);
char **read_line (void);
void parse_line (char **command, char *inputline);

int built_in (char **command);
int execute_command (char **command);

void set_vars (char *directory);

void add_history (char *input, history_t *node);
void print_history (void);

void bloop_input (char *input_buffer, int max_size);
int key_press (void);
