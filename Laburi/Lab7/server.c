/*
 * Protocoale de comunicatii
 * Laborator 7 - TCP
 * Echo Server
 * server.c
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#include "helpers.h"

int main(int argc, char *argv[]) {
    int listenfd = 0, connfd = 0, connfd2 = 0;
    char buf[BUFLEN];
    struct sockaddr_in serv_addr;

    socklen_t x = sizeof(struct sockaddr_in);

    if(argc != 3) {
        printf("\n Usage: %s <ip> <port>\n", argv[0]);
        return 1;
    }

    // deschidere socket
    listenfd = socket(PF_INET, SOCK_STREAM, 0);

    // completare informatii despre adresa serverului
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    // legare proprietati de socket
    bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr_in));

    // ascultare de conexiuni
    listen(listenfd, 3);

    // acceptarea unei conexiuni, primirea datelor, trimiterea raspunsului
    connfd = accept(listenfd, (struct sockaddr *) &serv_addr, &x);
    connfd2 = accept(listenfd, (struct sockaddr *) &serv_addr, &x);

    int flag = 1;
    setsockopt(connfd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));
    setsockopt(connfd2, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));

    memset(buf, 0, BUFLEN);
    while (1) {
        if (recv(connfd, buf, BUFLEN, 0) > 0) {
            printf("Recieved %s\n", buf);
            if (strstr(buf, "exit") != NULL) {
                send(connfd2, buf, strlen(buf) + 1, 0);
                send(connfd, buf, strlen(buf) + 1, 0);
                break;
            }
            send(connfd2, buf, strlen(buf) + 1, 0);
        }

        if (recv(connfd2, buf, BUFLEN, 0) > 0) {
            printf("Recieved %s\n", buf);
            if (strstr(buf, "exit") != NULL) {
                send(connfd, buf, strlen(buf) + 1, 0);
                send(connfd2, buf, strlen(buf) + 1, 0);
                break;
            }
            send(connfd, buf, strlen(buf) + 1, 0);
        }


    }

    // inchidere socket(i)
    close(listenfd);
    close(connfd2);
    close(connfd);

    return 0;
}