/*
 ** listener.c -- a datagram sockets "server" demo
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "util.h"

#define MYPORT "4950"

#define MAXBUFLEN 100

int main(int argc, char* argv[]) {
    int sockfd;
    struct addrinfo hints = {
        .ai_family = AF_INET6,
        .ai_socktype = SOCK_DGRAM,
        .ai_flags = AI_PASSIVE
    }, *servinfo, *p;
    int rv;
    int numbytes;
    char buf[MAXBUFLEN];
    char s[INET6_ADDRSTRLEN];

    if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) {
        fprintf(
            stderr,
            "listener:%d: getaddrinfo: %s\n",
            __LINE__,
            gai_strerror(rv)
        );

        return EXIT_FAILURE;
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "listener:%d: failed to bind to socket\n", __LINE__);
        return 2;
    }

    freeaddrinfo(servinfo);

    puts("listener: waiting to recvfrom...");

    struct sockaddr_storage their_addr;
    socklen_t addr_len = sizeof their_addr;

    if ((numbytes = recvfrom(
        sockfd,
        buf,
        MAXBUFLEN - 1,
        0,
        (struct sockaddr *) &their_addr,
        &addr_len
    )) == -1) {
        perror("recvfrom");
        return EXIT_FAILURE;
    }

    printf(
        "listener: got packet from %s\n",
        inet_ntop(
            their_addr.ss_family,
            get_in_addr((struct sockaddr *) &their_addr),
            s,
            sizeof s
        )
    );

    printf("listener: packet is %d bytes long\n", numbytes);
    buf[numbytes] = '\0';
    printf("listener: packet contains \"%s\"\n", buf);

    close(sockfd);

    return EXIT_SUCCESS;
}

