#ifndef UTIL_H
#define UTIL_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>

void * get_in_addr(struct sockaddr *sa);

int sendall(int s, char *buf, int *len);

uint64_t pack754(long double f, unsigned bits, unsigned expbits);

long double unpack754(uint64_t i, unsigned bits, unsigned expbits);

/**
 * @brief store a 16-bit int into a char buffer (like htons())
 */
void packi16(unsigned char *buf, unsigned int i);

/**
 * @brief store a 32-bit int into a char buffer (like htonl())
 */
void packi32(unsigned char *buf, unsigned long int i);

/**
 * @brief store a 64-bit int into a char buffer (like htonl())
 */
void packi64(unsigned char *buf, unsigned long long int i);

/**
 * @brief unpack a 16-bit int from a char buffer (like ntohs())
 */
int unpacki16(unsigned char *buf);

/**
 * @brief unpack a 16-bit unsigned from a char buffer (like ntohs())
 */
unsigned int unpacku16(unsigned char *buf);

/**
 * @brief unpack a 32-bit int from a char buffer (like ntohl())
 */
long int unpacki32(unsigned char *buf);

/**
 * @brief unpack a 32-bit unsigned from a char buffer (like ntohl())
 */
unsigned long int unpacku32(unsigned char *buf);

/**
 * @brief unpack a 64-bit int from a char buffer (like ntohl())
 */
long long int unpacki64(unsigned char *buf);

/**
 * @brief unpack a 64-bit unsigned from a char buffer (like ntohl())
 */
unsigned long long int unpacku64(unsigned char *buf);

/**
 * @brief store data dictated by the format string in the buffer
 *
 * | bits | signed | unsigned | float | string |
 * |------|--------|----------|-------|--------|
 * |   8  |    c   |     C    |       |        |
 * |  16  |    h   |     H    |   f   |        |
 * |  32  |    l   |     L    |   d   |        |
 * |  64  |    q   |     Q    |   g   |        |
 * |      |        |          |       |   s    |
 *
 * (16-bit unsigned length is automatically prepended to strings)
 */
unsigned int pack(unsigned char *buf, char *format, ...);


/**
 * @brief unpack data dictated by the format string into the buffer
 *
 * | bits | signed | unsigned | float | string |
 * |------|--------|----------|-------|--------|
 * |   8  |    c   |     C    |       |        |
 * |  16  |    h   |     H    |   f   |        |
 * |  32  |    l   |     L    |   d   |        |
 * |  64  |    q   |     Q    |   g   |        |
 * |      |        |          |       |   s    |
 *
 * (string is extracted based on its store length, but 's' can be prepended with a max length)
 */
void unpack(unsigned char *buf, char *format, ...);

#endif
