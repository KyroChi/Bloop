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
 * - Ruby plugin system, modular system
 * - accept bsh and .bsh files (Shell scripting language)
 * - include basic utilities
 * - run executable files
 * - .sh to .bsh interperter or compiler
 * - live tab completions
 * - command history
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

/*
 * jauns defaults for missing config files
 * Load defaults if the .jauns file cannot be found or is missing. Include a
 * default for any configurable setting included in the .jauns file. Do not
 * error out if the file cannot be found.
 */
#define BLOOP_DEFAULT_MOTD "Couldn't load .bloop, check that .bloop is in your filepath"
#define BLOOP_DEFAULT_PROMPT "$ "

/*
 * Launch the shell, return 0 on a successful exit.
 */
int
main (int argc, char **argv)
{
        char **command_history = malloc(sizeof(char*) *
                                        BLOOP_MAX_HISTORY_DEPTH);
        shell_init();
        return shell_loop(command_history);
}

/*
 * Loop through the shell. Take the user's input and execute the commands that
 * the user has entered. Read the input line, parse the input line, execute the
 * input line. Do this over and over until an exit command, or error is hit.
 *
 * Live tab completions should be handled here? Right?
 */
int
shell_loop (char **history)
{
        int status = 1;
        char *input = malloc(sizeof(char) * BLOOP_MAX_BUFFER_SIZE);
        char **command = malloc (sizeof(char*) * BLOOP_MAX_INPUT_ARGS);

        char *directory = malloc(sizeof(char) * BLOOP_MAX_VARIABLE_NAME);

        while (status) {

                set_vars(directory);
                printf("%s ", directory);

                printf(BLOOP_DEFAULT_PROMPT);
                input = fgets(input, BLOOP_MAX_BUFFER_SIZE, stdin);
                parse_line(command, input);

                if (!built_in(command)) {
                        /* fork and execute the command */
                        pid_t child_pid = fork();
                        /* Check childPID for -1 and errorno */
                        if (child_pid == 0) {
                                execute_command(command);
                        } else {
                                int status;
                                waitpid(child_pid, &status, 0);
                                //if (is_background_job(command)) {
                                        // Record in list of background jobs
                                //} else {
                                //        wait_pid(childPID);
                                //}
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
 * Simple test function to echo commands issued at the console, use for testing
 * shell inputs.
 */
void
echo_command (char **command)
{
        int ii = 0;
        printf("Command: %s\n", command[ii]);
        printf("Arguments:\n");

        while (command[++ii][0] != '\0') {
                printf("%s\n", command[ii]);
        }

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
built_in (char **command)
{
        if (!strcmp("exit", command[0])) {
                printf("[Process Complete]\n");
                exit(0);
                return 1;
        }

        if (!strcmp("history", command[0])) {
                return 2;
        }

        /* Return 0 if the command is not a built in */
        return 0;
}

/*
 * Simple wrapper function for the execvp function, given a parsed string as
 * an input.
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
