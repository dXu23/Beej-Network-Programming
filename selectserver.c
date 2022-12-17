/*
** selectserver.c -- a cheezy multiperson chat server
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

#include "util.h"

#define PORT "9034" // port we're listening on
                    //
int main(void) {
    fd_set master, read_fds;
    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    int fdmax;

    int listener, newfd;
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen;

    char buf[256];
    int nbytes;

    char remoteIP[INET6_ADDRSTRLEN];

    int yes = 1;
    int i, j, rv;

    struct addrinfo hints = {
        .ai_family = AF_UNSPEC,
        .ai_socktype = SOCK_STREAM,
        .ai_flags = AI_PASSIVE
    }, *ai, *p;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver:%d: %s\n", __LINE__, gai_strerror(rv));
        return EXIT_FAILURE;
    }

    for (p = ai; p != NULL; p = p->ai_next) {
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

    if (p == NULL) {
        fputs("selectserver: failed to bind\n", stderr);
        return 2;
    }

    if (listen(listener, 10) == -1) {
        perror("selectserver: listen");
        return 3;
    }

    FD_SET(listener, &master);

    fdmax = listener;

    while (true) {
        read_fds = master;
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("selectserver: select");
            exit(4);
        }

        for (i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == listener) {
                    addrlen = sizeof(remoteaddr);
                    newfd = accept(
                        listener,
                        (struct sockaddr *) &remoteaddr,
                        &addrlen
                    );

                    if (newfd == -1) {
                        perror("selectserver: accept");
                    } else {
                        FD_SET(newfd, &master);
                        if (newfd > fdmax) {
                            fdmax = newfd;
                        }

                        printf(
                            "selectserver: new connection from %s on socket %d\n",
                            inet_ntop(
                                remoteaddr.ss_family,
                                get_in_addr((struct sockaddr *) &remoteaddr),
                                remoteIP,
                                INET6_ADDRSTRLEN),
                            newfd
                        );
                    }
                } else {
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                        if (nbytes == 0) {
                            printf("selectserver: socket %d hung up\n", i);
                        } else {
                            perror("selectserver: recv");
                        }

                        close(i);
                        FD_CLR(i, &master);
                    } else {
                        for (j = 0; j <= fdmax; j++) {
                            if (FD_ISSET(j, &master)) {
                                if (j != listener && j != i) {
                                    if (send(j, buf, nbytes, 0) == -1) {
                                        perror("selectserver: send");
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return EXIT_SUCCESS;
}

