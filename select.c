/*
** select.c -- a select() demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define STDIN 0

int main(void) {
    struct timeval tv = { .tv_sec = 2, .tv_usec = 500000 };
    fd_set readfds;

    FD_ZERO(&readfds);
    FD_SET(STDIN, &readfds);

    // don't care about writefds and exceptfds
    select(STDIN + 1, &readfds, NULL, NULL, &tv);

    puts(FD_ISSET(STDIN, &readfds) ? "A key was pressed!" : "Timed out.");

    return EXIT_SUCCESS;
}

