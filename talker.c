/*
** talker.c -- a datagram "client" demo
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


#define SERVERPORT "4950"

int main(int argc, char *argv[]) {
    int sockfd;
    struct addrinfo hints = {
        .ai_family = AF_INET6,
        .ai_socktype = SOCK_DGRAM
    }, *servinfo, *p;
    int rv;
    int numbytes;

    if (argc != 3) {
        fputs("Usage: talker hostname message\n", stderr);
        return EXIT_FAILURE;
    }

    if ((rv = getaddrinfo(argv[1], SERVERPORT, &hints, &servinfo)) != 0) {
        fprintf(
            stderr,
            "talker:getaddrinfo:%d: %s\n",
            __LINE__,
            gai_strerror(rv)
        );

        return EXIT_FAILURE;
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fputs("talker: failed to create socket\n", stderr);
        return 2;
    }

    if ((numbytes = sendto(
        sockfd,
        argv[2],
        strlen(argv[2]),
        0,
        p->ai_addr,
        p->ai_addrlen)
    ) == -1) {
        perror("talker: sendto");
        return EXIT_FAILURE;

    }

    freeaddrinfo(servinfo);

    printf("talker: sent %d bytes to %s\n", numbytes, argv[1]);
    close(sockfd);

    return EXIT_SUCCESS;
}

