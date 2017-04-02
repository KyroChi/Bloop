/*
 *                             H I S T O R Y . C
 *
 * Implementations of the bloop history management functions.
 *
 * add_history
 * print_history
 */

#include "bloop.h"
#include <stdlib.h>
#include <stdio.h>

/*
 * add_history
 *
 * Adds a command to the shell's global history variable.
 *
 * Behavior is undefined if global history is not initialized when the funciton
 * is called. It should result in a segmentation fault most of the time.
 *
 * Behavior is also undefined if command and ptr are undefined. If ptr is
 * undefined a seg fault will likely occur.
 *
 * char *command: The command to add to the history, including arguments
 *     seperated by spaces. Store the exact form as it was stored after a call to
 *     bloop_input.
 * history_t *ptr: An initialized history_t pointer. This will contain the new
 *     command history entry.
 */
void
add_history (char *command, history_t *ptr)
{
	/* currently adds newlines and tabs */
        int ii = 0;

        ptr->prev = g_history_head;
        g_history_head->next = ptr;
        ptr->next = NULL;

        g_history_head = ptr;

        while (command[ii++]);

        ptr->command = malloc(sizeof(char) * ii - 1);

        (ptr->command)[ii - 2] = '\0';

        ii = 0;

        while (command[ii]) {
                if (command[ii] == '\n') {
                        break;
                } else {
                        (ptr->command)[ii] = command[ii];
                        ii++;
                }
        }

        return;
}

/*
 * print_history
 *
 * Prints the global history to stdout.
 *
 * Behavior is undefined if global history is not initialized when the function
 * is called. It should result in a segmentation fault most of the time.
 */
void
print_history ()
{
        int ii = 1;
        history_t *ptr = g_history;

        while (ptr->next) {

        #ifdef TEST
        #else
                printf("%i: %s\n", ii, (ptr->next)->command);
                fflush(stdout);
        #endif

                ptr = ptr->next;
                ii++;
        }

        return;
}
