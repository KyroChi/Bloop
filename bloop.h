#define BLOOP_MAX_BUFFER_SIZE 1024
#define BLOOP_MAX_INPUT_ARGS 32
#define BLOOP_MAX_ARG_LENGTH 64
#define BLOOP_MAX_HISTORY_DEPTH 10
#define BLOOP_MAX_VARIABLE_NAME 64
#define BLOOP_DEFAULT_HISTORY_DEPTH 10

int shell_loop (void);
void shell_init (void);
char **read_line (void);
void parse_line (char **command, char *inputline);

int built_in (char **command, char **history);
int execute_command (char **command);

void set_vars (char *directory);

void add_history (char **history, char *input, int index);
void print_history (char **history);
