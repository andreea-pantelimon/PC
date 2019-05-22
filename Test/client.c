#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <ctype.h>
#include <math.h>
#include "helpers.h"

void usage(char *file) {
    fprintf(stderr, "Usage: %s server_address server_port\n", file);
    exit(0);
}

int get_key (int n, int p) {
    int a = n, res = 1;
    for(int i = 0; (1 << i) <= p; i++) {
        if((1 << i) & p) {
            res = (1LL * res * a) % N;
        }
        a = (1LL * a * a) % N;
    }

    return res % N;
}

void encrypt(char* msg, int k, int len) {
    for (int i = 0; i < len; ++i) {
        if (isupper(msg[i])) {
            msg[i] = (msg[i] + k - 'A') % 26 + 'A';
        } else {
            msg[i] = (msg[i] + k - 'a') % 26 + 'a';
        }
    }
}

int main(int argc, char *argv[])
{
    int sockfd = 0, g, k, p, K, ret, n, first_msg = 0;
    char buffer[BUFLEN];
    struct sockaddr_in serv_addr;

    if(argc != 4)
    {
        printf("\n Usage: %s <ip> <port> <num>\n", argv[0]);
        return 1;
    }

    srand(time(NULL));
    g = atoi(argv[3]);
    p = rand() % g;
    printf("p = %d\n", p);
    k = get_key(g, p);
    printf("k = %d\n", k);

    fd_set read_fds;
    fd_set tmp_fds;

    int fdmax;

    FD_ZERO(&tmp_fds);
    FD_ZERO(&read_fds);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    DIE(sockfd < 0, "socket");

    FD_SET(sockfd, &read_fds);
    fdmax = sockfd;
    FD_SET(0, &read_fds);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    ret = inet_aton(argv[1], &serv_addr.sin_addr);
    DIE(ret == 0, "inet_aton");

    ret = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    DIE(ret < 0, "connect");

    sprintf(buffer, "%d", k);
    printf("%s\n", buffer);
    n = send(sockfd, buffer, strlen(buffer), 0);
    DIE(n < 0, "send");

    while (1) {
        tmp_fds = read_fds;

        ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
        DIE(ret < 0, "select");

        if (FD_ISSET(0, &tmp_fds)) {  // citeste de la tastatura
            memset(buffer, 0, BUFLEN);
            fgets(buffer, BUFLEN - 1, stdin);

            if (strncmp(buffer, "exit", 4) == 0) {
                break;
            } else {
                buffer[strlen(buffer) - 1] = '\0';
                encrypt(buffer, K, strlen(buffer));
                n = send(sockfd, buffer, strlen(buffer), 0);
                DIE(n < 0, "send");
            }
        } else {
            memset(buffer, 0, BUFLEN);
            recv(sockfd, buffer, BUFLEN, 0);

            if (!first_msg) {
                first_msg = 1;
                k = atoi(buffer);
                printf("k = %d; p = %d\n", k, p);
                K = get_key(k, p);
                printf("K = %d\n", K);
            } else {
                encrypt(buffer, 26 - K, strlen(buffer));
                printf("%s\n", buffer);
            }
        }
    }

    close(sockfd);
    return 0;
}
