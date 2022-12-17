/*
 ** client.c -- a stream socket client demo
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include "util.h"

#define PORT "3490" // the port client will be connecting to

#define MAXDATASIZE 100 // max number of bytes we can get at once

int main(int argc, char *argv[]) {
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints = {
        .ai_family = AF_UNSPEC,
        .ai_socktype = SOCK_STREAM
    }, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fputs("Usage: client hostname\n", stderr);
        return EXIT_FAILURE;
    }

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "client:getaddrinfo:%d: %s\n", __LINE__, gai_strerror(rv));
        return EXIT_FAILURE;
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(
                    p->ai_family,
                    p->ai_socktype,
                    p->ai_protocol
                )) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client:%d: failed to connect\n", __LINE__);
        return 2;
    }

    inet_ntop(
        p->ai_family,
        get_in_addr((struct sockaddr *) p->ai_addr),
        s,
        sizeof s
    );

    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo);

    if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1) {
        perror("recv");
        return EXIT_FAILURE;
    }

    buf[numbytes] = '\0';

    printf("client: received '%s'\n", buf);

    close(sockfd);

    return EXIT_SUCCESS;
}

