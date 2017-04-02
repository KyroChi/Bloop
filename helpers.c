/*
 *                             H E L P E R S . C
 *
 * Implementations of bloop helper and utility functions.
 *
 * bloop_error
 * key_press
 */

#include "bloop.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>

/*
 * Error handling for shell errors. This function exits the shell use
 * bloop_warning for warnings.
 *
 * char *message: The mesage to be printed to stdout when the shell exits.
 * int exit_code: The exit code that the shell will exit with.
 */
void
bloop_error (char *message, int exit_code)
{
        fflush(stdout);
        fprintf(stderr, "%s\n", message);
        fflush(stderr);
        exit(exit_code);
}

#if 0
void
bloop_warning (char *message)
{
        return;
}
#endif

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
                term.c_lflag = ICANON;
                tcsetattr(STDIN, TCSANOW, &term);
                setbuf(stdin, NULL);
                init = 1;
        }

        ioctl(STDIN, FIONREAD, &bytes);
        return bytes;
}

/*
 * Wrapper for the malloc call to enable debug memory tracking
 */
void *
blp_mem_alloc (size_t size)
{
	void *ptr;
	if (!(ptr = malloc(size))) {
	        bloop_error("malloc failed to allocate memory", 9);
	}
	
#ifdef DEBUG
        /* Add to global memory tracker */
#endif
	
	return ptr;
}

/*
 * Wrapper for the free call to allow for debug memory tracking
 */
void
blp_mem_free (void *ptr)
{
#ifdef DEBUG
        /* remove from the memory tracker */
        /* ASSERT that the block is being managed */
#endif
        free(ptr);
	return;
}
