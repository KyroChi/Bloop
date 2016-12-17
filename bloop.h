#define BLOOP_MAX_BUFFER_SIZE 1024
#define BLOOP_MAX_INPUT_ARGS 32
#define BLOOP_MAX_ARG_LENGTH 64
#define BLOOP_MAX_HISTORY_DEPTH 10
#define BLOOP_MAX_VARIABLE_NAME 64

typedef struct hist_list_s {
        char **command;
        struct hist_list_s *next;
        struct hist_list_s *prev;
} hist_list_t;

int shell_loop (char **history);
void shell_init (void);
char **read_line (void);
void parse_line (char **command, char *inputline);

int built_in (char **command);
int execute_command (char **command);

void set_vars (char *directory);
