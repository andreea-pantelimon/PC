#include <netinet/in.h>
#include <arpa/inet.h>
#include <zconf.h>
#include <netinet/tcp.h>
#include "subscriber_utils.h"

using namespace std;

int main(int argc, char** argv) {
    ASSERT(argc != 4, "Usage: ./subscriber <ID> <IP_SERVER> <PORT_SERVER>.\n")
    ASSERT(strlen(argv[1]) > 10, "Subscriber ID should be at most 10 characters long.\n")

    char buff[MAXRECV];
    int serv_sock, num_bytes, flag = 1;
    sockaddr_in serv_addr;
    tcp_msg_t* recv_msg;
    serv_msg_t sent_msg;
    fd_set fds, tmp_fds;

    // se creeaza socketul dedicat conexiunii la server
    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT(serv_sock < 0, "Unable to open server socket.\n")

    // se completeaza datele despre socketul TCP corespunzator conexiunii la server
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[3]));
    ASSERT(inet_aton(argv[2], &serv_addr.sin_addr) == 0,
            "Incorrect <IP_SERVER>. Conversion failed.\n")
    ASSERT(connect(serv_sock, (sockaddr*) &serv_addr, sizeof(serv_addr)) < 0,
            "Unable to connect to server.\n")
    ASSERT(send(serv_sock, argv[1], strlen(argv[1]) + 1, 0) < 0, "Unable to send ID to server.\n")

    // se dezactiveaza algoritmul lui Nagle pentru conexiunea la server
    setsockopt(serv_sock, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));

    // se seteaza file descriptorii socketilor pentru server si pentru stdin
    FD_ZERO(&fds);
    FD_SET(serv_sock, &fds);
    FD_SET(0, &fds);

    while (1) {
        tmp_fds = fds;

        ASSERT(select(serv_sock + 1, &tmp_fds, nullptr, nullptr, nullptr) < 0,
               "Unable to select.\n")

        if (FD_ISSET(0, &tmp_fds)) {
            // s-a primit un mesaj de la stdin
            memset(buff, 0, MAXRECV);
            fgets(buff, MAXBUFF - 1, stdin);

            if (!strcmp(buff, "exit\n")) {
                break;
            }

            // s-a primit o comanda diferita de `exit`
            if (make_message(&sent_msg, buff)) {
                ASSERT(send(serv_sock, (char*) &sent_msg, sizeof(sent_msg), 0) < 0,
                       "Unable to send message to server.\n")

                // se afiseaza mesajele corespunzatoare comenzii date
                if (sent_msg.type == 's') {
                    printf("subscribed %s\n", sent_msg.topic_name);
                } else {
                    printf("unsubscribed %s\n", sent_msg.topic_name);
                }
            }
        }

        if (FD_ISSET(serv_sock, &tmp_fds)) {
            // s-a primit un mesaj de la server (adica de la un client UDP al acestuia)
            memset(buff, 0, MAXRECV);
            num_bytes = recv(serv_sock, buff, sizeof(tcp_msg_t), 0);
            ASSERT(num_bytes < 0, "Error receiving from server.\n")

            if (num_bytes == 0) {
                // serverul a inchis conexiunea cu clientul curent
                break;
            }

            // se afiseaza mesajul primit
            recv_msg = (tcp_msg_t*)buff;
            printf("%s:%hu - %s - %s - %s\n", recv_msg->ip, recv_msg->udp_port,
                   recv_msg->topic_name, recv_msg->type, recv_msg->data);
        }
    }

    close(serv_sock);

    return  0;
}
