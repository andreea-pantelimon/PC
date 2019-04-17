/*
 * Protocoale de comunicatii
 * Laborator 7 - TCP
 * Echo Server
 * client.c
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include "helpers.h"

int main(int argc, char *argv[])
{
    int sockfd = 0;
    char buf[BUFLEN];
    struct sockaddr_in serv_addr; 

    if(argc != 3)
    {
        printf("\n Usage: %s <ip> <port>\n", argv[0]);
        return 1;
    }

    // deschidere socket
    sockfd = socket(PF_INET, SOCK_STREAM, 0);

    // completare informatii despre adresa serverului
    serv_addr.sin_family = PF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);

    // conectare la server
    connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr_in));

    // citire de la tastatura, trimitere de cereri catre server, asteptare raspuns
    while (strstr(buf, "exit") == NULL) {
        recv(sockfd, buf, strlen(buf) + 1, 0);
        printf("%s", buf);
        fgets(buf, BUFLEN, stdin);
        send(sockfd, buf, strlen(buf) + 1, 0);
    }

    // inchidere socket
    close(sockfd);

    return 0;
}
