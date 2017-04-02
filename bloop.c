/*
 *                               B L O O P . C
 *
 * Bloop is just another unix shell. This project is to learn about shells and
 * interacting with the operating system to manipulate file systems and run
 * executable files.
 *
 * This is a toy implementation of a bash-like shell, following roughly the
 * assignment page from the cornell computer science department:
 * https://www.cs.cornell.edu/Courses/cs414/2004su/homework/shell/shell.html
 *
 * TODO:
 * - accept bsh and .bsh files (Shell scripting language)
 * - include basic utilities
 * - .sh to .bsh interperter or compiler
 * - live tab completions <<<
 */

#include "bloop.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/ioctl.h>

/*
 * Default path to look for a .bloop file
 */
#define BLOOP_DOTBLOOP_FILEPATH "."
#define BLOOP_DEFAULT_MOTD "Couldn't load .bloop, check that .bloop is in your filepath"
#define BLOOP_DEFAULT_PROMPT ">>> "

/*
 * GCC entry point, initializes shell variables and starts the shell looping.
 */
int
main ()//int argc, char **argv)
{
        shell_init();
        return shell_loop();
}

/*
 * Loop through the shell. Take the user's input and execute the commands that
 * the user has entered. Read the input line, parse the input line, execute the
 * input line. Do this over and over until an exit command, or error is hit.
 *
 * Live tab completions should be handled here? Right?
 */
int
shell_loop (void)
{
        int status = 1;

        int return_status = 0;
        int command_index = 0;

        char *input = malloc(sizeof(char) * BLOOP_MAX_BUFFER_SIZE);
        char **command = malloc(sizeof(char) * BLOOP_MAX_ARG_LENGTH);
        char *directory = malloc(sizeof(char) * BLOOP_MAX_VARIABLE_NAME);

        if (!input || !directory || !command) {
                bloop_error("Shell failed to allocate memory EXIT(10)", 10);
        }

        input[0] = '\0';

        while (status) {

                set_vars(directory);
                printf("%s ", directory);

                printf(BLOOP_DEFAULT_PROMPT);
                bloop_input(input, BLOOP_MAX_BUFFER_SIZE);

                history_t *new_hist_command;
                if (!(new_hist_command = malloc(sizeof(history_t)))) {
                        bloop_error("HISTORY: failed to allocate more memory " \
                                "for added depth EXIT(10)", 10);
                }

                add_history(input, new_hist_command);
                command_index++;

                parse_line(command, input);

                if (!built_in(command)) {
                        pid_t child_pid = fork();
                        if (child_pid == 0) {
                                if(execute_command(command)) {
                                        printf("Command %s failed to execute\n", command[0]);
                                        exit(5);
                                }
                        } else {
                                waitpid(child_pid, &return_status, 0);
                        }
                }

        }

        return status;
}

/*
 * Load the configuration settings from the .bloop file and set up the shell
 * accordingly. This function is also responsible for loading any plugins into
 * the shell enviroment. This function is also responsible for initializing the
 * command history tracking for the shell.
 */
void
shell_init (void)
{
        // Open .bloop files, revert to defaults if no file is found
        // If file is found, read the config and adjust the shell appropriatly
        printf("Welcome to the bloop shell\n");

        /* Initialize global command history */
        if (!(g_history = malloc(sizeof(history_t)))) {
                bloop_error("HISTORY: malloc failure EXIT(10)", 10);
        }

        g_history->command = "\0";
        g_history->next = g_history->prev = NULL;

        g_history_head = g_history;

        return;
}

/*
 * Read the input from the user and translate it into a command with arguments
 *
 * Handling pipes, re-direction, ecetra?
 *
 * Returns a char** containing the argument in the first address and arguments
 * in the subsequent addresses. The last element is '\0' to denote termination
 * of the command arguments.
 */
void
parse_line (char **command, char *inputline)
{
        int ii, jj, kk;
        ii = jj = kk = 0;

        command[jj] = malloc(sizeof(char) * BLOOP_MAX_ARG_LENGTH);

        while (inputline[ii]) {
                if (inputline[ii] == ' ') {
                        command[++jj] = malloc(sizeof(char) *
                                               BLOOP_MAX_ARG_LENGTH);
                        kk = 0;
                } else {
                        if (inputline[ii] != '\n') {
                                (command[jj])[kk++] = inputline[ii];
                        }
                }
                ii++;
        }

        // Null terminate the char ** so that execvp can handle the commands
        command[++jj] = NULL;


        return;
}

/*
 * Checks the user's input against the built in commands and returns an integer
 * value accordingly.
 *
 * 0 - Command was not a built in
 * 1 - Command was 'exit', terminate the shell
 * 2 - Commnad was 'history', print out the command history
 * 3 - Command was 'help', print out the shell help menu
 * 4 - cd
 * 5 - kill
 * 6 - jobs
 * search for pipe, equals, exclamation
 */
int
built_in (char **command)
{
        if (!command[0]) {
                return -1;
        }

        if (!strcmp("exit", command[0])) {
                printf("[Process Complete]\n");
                exit(0);
                return 1;
        }

        if (!strcmp("history", command[0])) {
                print_history();
                return 2;
        }

        if (!strcmp("help", command[0])) {
                return 3;
        }

        /* Return 0 if the command is not a built in */
        return 0;
}

/*
 * Simple wrapper function for the execvp function, given a parsed command as
 * an input. Any return value means that execvp failed to find the requested
 * program and didn't execute the command. This should be handled by the
 * implementation of this function as it is not handled by the function.
 */
int
execute_command (char **command)
{
        return execvp(command[0], &command[0]);
}

/*
 * Undefined behaviors:
 */
void
set_vars (char *directory)
{
        getcwd(directory, BLOOP_MAX_VARIABLE_NAME);
        return;
}

/*
 * Handles input into the bloop shell, hook into tab completion.
 */
void
bloop_input (char *input_buffer, int max_size)
{
        int ii = 0;
        char entered;

        while (1) {
                if (ii == max_size - 1)
                        break;

                if (key_press) {
                        entered = fgetc(stdin);

                        switch (entered) {
                        case '\n':
                                input_buffer[ii++] = entered;
                                input_buffer[ii] = '\0';
                                return;
                        case '\t':
                                /* Tab completion goes here */
                                break;

                        }

                        input_buffer[ii++] = entered;
                }

        }

        return;
}

/*
 * Return number of buffered keys in stdin
 * can be used to check if anything is buffered in stdin.
 */
int
key_press (void)
{
        static int STDIN, init, bytes;
        STDIN = init = bytes = 0;
        struct termios term;

        if (!init) {
                tcgetattr(STDIN, &term);
                term.c_lflag &= ~ICANON;
                tcsetattr(STDIN, TCSANOW, &term);
                setbuf(stdin, NULL);
                init = 1;
        }

        ioctl(STDIN, FIONREAD, &bytes);
        return bytes;
}

/*
 * Error handling for shell errors.
 */
void
bloop_error (char *message, int exit_code) {
        fflush(stdout);
        fprintf(stderr, "%s\n", message);
        fflush(stderr);
        exit(exit_code);
}
