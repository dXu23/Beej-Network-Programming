#include <stdio.h>
#include <stdlib.h>
#include <poll.h>

int main(void) {
    struct pollfd pfds[1] = { { .events = POLLIN } };

    puts("Hit RETURN or wait 2.5 seconds for timeout");

    int num_events = poll(pfds, 1, 2500);

    if (num_events == 0) {
        puts("Poll timed out!");
    } else {
        int pollin_happened = pfds[0].revents & POLLIN;

        if (pollin_happened) {
            printf("File descriptor %d is ready to read\n", pfds[0].fd);
        } else {
            printf("Unexpected event occurred: %d\n", pfds[0].revents);
        }
    }

    return EXIT_SUCCESS;
}
