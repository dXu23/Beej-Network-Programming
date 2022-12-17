/*
** broadcaster.c -- a datagram "client" like talker.c, except
** this one can broadcast
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


#define SERVERPORT 4950

int main(int argc, char *argv[]) {
    int sockfd;
    struct hostent *he;
    int numbytes;
    int broadcast = 1;

    if (argc != 3) {
        fputs("Usage: Talker hostname message\n", stderr);
        return EXIT_FAILURE;
    }

    if ((he = gethostbyname(argv[1])) == NULL) {
        perror("broadcaster: gethostbyname");
        return EXIT_FAILURE;
    }

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("broadcaster:socket");
        return EXIT_FAILURE;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast) == -1) {
        perror("broadcaster:setsockopt (SO_BROADCAST");
        return EXIT_FAILURE;
    }

    struct sockaddr_in their_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(SERVERPORT),
        .sin_addr = *((struct in_addr *) he->h_addr)
    }; // connector's address information

    if ((numbytes = sendto(
        sockfd,
        argv[2],
        strlen(argv[2]),
        0,
       (struct sockaddr *) &their_addr,
        sizeof their_addr)
    ) == -1) {
        perror("talker: sendto");
        return EXIT_FAILURE;

    }

    printf("broadcaster: sent %d bytes to %s\n", numbytes, inet_ntoa(their_addr.sin_addr));

    close(sockfd);

    return EXIT_SUCCESS;
}

