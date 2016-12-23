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
 * Bloop supports user plugins written in ruby, and has its own rich scripting
 * language called bsh.
 *
 * TODO:
 * - accept bsh and .bsh files (Shell scripting language)
 * - include basic utilities
 * - .sh to .bsh interperter or compiler
 * - live tab completions <<<
 */

#include "bloop.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * Default path to look for a .bloop file
 */
#define BLOOP_DOTBLOOP_FILEPATH "."
#define BLOOP_DEFAULT_MOTD "Couldn't load .bloop, check that .bloop is in your filepath"
#define BLOOP_DEFAULT_PROMPT ">>> "

/*
 * Launch the shell, return 0 on a successful exit.
 */
int
main (int argc, char **argv)
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
        char *input = malloc(sizeof(char) * BLOOP_MAX_BUFFER_SIZE);
        char **command = malloc (sizeof(char*) * BLOOP_MAX_INPUT_ARGS);

        char *directory = malloc(sizeof(char) * BLOOP_MAX_VARIABLE_NAME);

        char **history = calloc(BLOOP_DEFAULT_HISTORY_DEPTH, sizeof(char *));
        history[BLOOP_DEFAULT_HISTORY_DEPTH] = NULL;

        int command_index = 0;

        while (status) {

                set_vars(directory);
                printf("%s ", directory);

                printf(BLOOP_DEFAULT_PROMPT);

                input = fgets(input, BLOOP_MAX_BUFFER_SIZE, stdin);

                add_history(history, input, command_index);
                command_index++;
                if (command_index == BLOOP_DEFAULT_HISTORY_DEPTH) {
                        realloc(history, sizeof(char *) *
                                (BLOOP_DEFAULT_HISTORY_DEPTH +
                                 command_index % BLOOP_DEFAULT_HISTORY_DEPTH
                                 * BLOOP_DEFAULT_HISTORY_DEPTH ));
                }

                parse_line(command, input);

                if (!built_in(command, history)) {
                        pid_t child_pid = fork();
                        if (child_pid == 0) {
                                if(execute_command(command)) {
                                        printf("Command failed to execute\n");
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
 */
int
built_in (char **command, char **history)
{
        if (!strcmp("exit", command[0])) {
                printf("[Process Complete]\n");
                exit(0);
                return 1;
        }

        if (!strcmp("history", command[0])) {
                print_history(history);
                return 2;
        }

        if (!strcmp("help", command[0])) {
                return 3;
        }

        /* Return 0 if the command is not a built in */
        return 0;
}

/*
 * Simple wrapper function for the execvp function, given a parsed string as
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
 *
 */
void
set_vars (char *directory)
{
        getcwd(directory, BLOOP_MAX_VARIABLE_NAME);
        return;
}

/*
 * Add a command to a command history list. Pass a pointer to the tail of the
 * history list and have the command appended to that list.
 *
 * Pretty sure this can be optimized. Maybe use realloc while walking the array
 * instead of pre walking the array and using malloc to create a buffer before
 * filling it with values.
 * Commands are truncated of newlines when stored in history.
 */
void
add_history (char **history, char *command, int index)
{
        int ii = 0;
        while (command[ii++])

        history[index] = malloc(sizeof(char) * ii - 1);

        (history[index])[ii - 2] = '\0';

        ii = 0;

        while (command[ii]) {
                if (command[ii] == '\n') {
                        break;
                } else {
                        history[index][ii] = command[ii];
                        ii++;
                }
        }

        return;
}

/*
 * Print out the command history of the shell session.
 */
void
print_history (char **history)
{
        int ii = 0;
        while (history[ii]) {
                printf("%i: %s\n", ii, history[ii]);
                ii++;
        }

        return;
}
