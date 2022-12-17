/*
** pollserver.c -- a cheezy multiperson chat server
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>

#include "util.h"

#define PORT "9034"

int get_listener_socket(void) {
    int listener, yes = 1, rv;

    struct addrinfo hints = {
        .ai_family = AF_UNSPEC,
        .ai_socktype = SOCK_STREAM,
        .ai_flags = AI_PASSIVE
    }, *ai, *p;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "pollserver: %s\n", gai_strerror(rv));
        exit(EXIT_FAILURE);
    }

    for (p = ai; p != NULL; p = ai->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            continue;
        }

        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    freeaddrinfo(ai);

    if (p == NULL) {
        return -1;
    }

    if (listen(listener, 10) == -1) {
        return -1;
    }

    return listener;
}

void add_to_pfds(struct pollfd *pfds[], int newfd, int *fd_count, int *fd_size) {
    if (*fd_count >= *fd_size) {
        *fd_size *= 2;

        *pfds = realloc(*pfds, sizeof(**pfds) * (*fd_size));
    }

    (*pfds)[*fd_count].fd = newfd;
    (*pfds)[*fd_count].events = POLLIN;

    (*fd_count)++;
}

void del_from_pfds(struct pollfd pfds[], int i, int *fd_count) {
    pfds[i] = pfds[*fd_count - 1];

    (*fd_count)--;
}


int main(void) {
    int ret_val = EXIT_SUCCESS;

    int listener, newfd;
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen;

    char buf[256];

    char remoteIP[INET_ADDRSTRLEN];

    int fd_count = 0, fd_size = 5;

    struct pollfd *pfds = malloc(sizeof *pfds * fd_size);

    if (pfds == NULL) {
        fputs("pollserver: failed to allocated poll file descriptors\n", stderr);
        return EXIT_FAILURE;
    }

    listener = get_listener_socket();

    if (listener == -1) {
        fputs("pollserver: error getting listening socket\n", stderr);
        ret_val = EXIT_FAILURE;
        goto CLEAN_PFDS;
    }

    pfds[0].fd = listener;
    pfds[0].events = POLLIN;

    fd_count = 1;

    while (true) {
        int poll_count = poll(pfds, fd_count, -1);

        if (poll_count == -1) {
            perror("poll");
            return EXIT_FAILURE;
        }

        for (int i = 0; i < fd_count; i++) {
            if (pfds[i].revents & POLLIN) {
                if (pfds[i].fd == listener) {
                    addrlen = sizeof remoteaddr;
                    newfd = accept(
                        listener,
                        (struct sockaddr *) &remoteaddr,
                        &addrlen
                    );

                    if (newfd == -1) {
                        perror("pollserver: accept");
                    } else {
                        add_to_pfds(&pfds, newfd, &fd_count, &fd_size);

                        printf(
                            "pollserver: new connection from %s on socket %d\n",
                            inet_ntop(
                                remoteaddr.ss_family,
                                get_in_addr((struct sockaddr *) &remoteaddr),
                                remoteIP,
                                INET6_ADDRSTRLEN
                            ),
                            newfd
                        );
                    }
                } else {
                    int nbytes = recv(pfds[i].fd, buf, sizeof buf, 0);

                    int sender_fd = pfds[i].fd;

                    if (nbytes <= 0) {
                        if (nbytes == 0) {
                            printf("pollserver: socket %d hung up\n", sender_fd);
                        } else {
                            perror("pollserver: recv");
                        }

                        close(pfds[i].fd);

                        del_from_pfds(pfds, i, &fd_count);
                    } else {
                        for (int j = 0; j < fd_count; j++) {
                            int dest_fd = pfds[j].fd;

                            if (dest_fd != listener && dest_fd != sender_fd) {
                                if (send(dest_fd, buf, nbytes, 0) == -1) {
                                    perror("pollserver: send");
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    CLEAN_PFDS:
        free(pfds);

    return ret_val;
}
