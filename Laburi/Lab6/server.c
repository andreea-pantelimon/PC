/*
 * Protocoale de comunicatii:
 * Laborator 6: UDP
 * mini-server de backup fisiere
 */
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "helpers.h"


void usage(char*file)
{
    fprintf(stderr, "Usage: %s server_port\n", file);
    exit(0);
}

/*
 * Utilizare: ./server server_port nume_fisier
 */
int main(int argc, char **argv)
{
    if (argc != 2)
        usage(argv[0]);

    int fd, sock;
    socklen_t socklen;
    struct sockaddr_in from_station;
    char buf[BUFLEN];
    memset(buf, 0, BUFLEN);

    sock = socket(PF_INET, SOCK_DGRAM, 0);

    from_station.sin_family = AF_INET;
    from_station.sin_port = htons(atoi(argv[1]));
    from_station.sin_addr.s_addr = INADDR_ANY;

    bind(sock, (struct sockaddr *) &from_station, sizeof(struct sockaddr_in));

    recvfrom(sock, buf, BUFLEN, 0, (struct sockaddr *) &from_station, &socklen);
    printf("%s\n", buf);
    fd = open(buf, O_WRONLY | O_CREAT, 0644);
    memset(buf, 0, BUFLEN);

    while (1) {
        recvfrom(sock, buf, BUFLEN, 0, (struct sockaddr *) &from_station, &socklen);
        if (strcmp(buf, "FINISH") == 0) {
            break;
        }
        write(fd, buf, strlen(buf));
        memset(buf, 0, BUFLEN);
    }

    close(sock);
    close(fd);

    return 0;
}
