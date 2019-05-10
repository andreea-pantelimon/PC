#include <netinet/tcp.h>
#include <arpa/inet.h>
#include "server_utils.h"

using namespace std;

int main(int argc, char** argv) {
    ASSERT(argc != 2, "Usage: ./server <PORT>\n")

    // buffere de mesaje pentru clientii TCP cu SF = 1, indexate dupa numele topicurilor
    unordered_map<string, vector<tcp_msg_t>> buffers;

    // asigura legatura dintre un topic si file descriptorii clientilor TCP abonati la el
    unordered_map<string, unordered_set<int>> online;

    // clientii TCP cu SF care sunt offline, pentru care se retin fd-ul si pozitia ultimului mesaj
    // primit din vectorul de mesaje bufferate al respectivului topic
    unordered_map<int, client_t> clients;  // retine file descriptorii clientilor si clientii insisi

    // retine topicele unui client offline, identificat dupa id-ul sau
    unordered_map<string, vector<topic_t>> client_topics;

    // retine numarul de clienti abonati la fiecare topic pentru a putea sterge bufferele
    // topicurilor catre care nu mai e nimeni abonat
    unordered_map<string, int> clients_per_topic;

    char buff[MAXBUFF];
    int udp_sock, port_num, tcp_sock, max_fd, new_sock, bytes_recv, flag = 1;
    socklen_t udp_socklen = sizeof(sockaddr), tcp_socklen = sizeof(sockaddr);
    sockaddr_in udp_addr, tcp_addr, new_tcp;
    recv_msg_t* udp_msg;
    tcp_msg_t tcp_msg;
    serv_msg_t* serv_msg;
    client_t crt_client;
    fd_set active_fds, tmp_fds;
    bool exit_msg = false;
    string crt_topic;

    // se creeaza socketul UDP
    udp_sock = socket(PF_INET, SOCK_DGRAM, 0);
    ASSERT(udp_sock < 0, "Unable to create UDP socket.\n")

    // se creeaza socketul TCP
    tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT(tcp_sock < 0, "Unable to create TCP socket.\n")

    port_num = atoi(argv[1]);
    ASSERT(port_num < 1024, "Incorrect port number.\n")

    // se completeaza informatiile despre socketul UDP si despre cel pasiv TCP
    udp_addr.sin_family = tcp_addr.sin_family = AF_INET;
    udp_addr.sin_port = tcp_addr.sin_port = htons(port_num);
    udp_addr.sin_addr.s_addr = tcp_addr.sin_addr.s_addr = INADDR_ANY;

    ASSERT(bind(udp_sock, (sockaddr*) &udp_addr, sizeof(sockaddr_in)) < 0,
        "Unable to bind UDP socket.\n")
    ASSERT(bind(tcp_sock, (sockaddr*) &tcp_addr, sizeof(sockaddr_in)) < 0,
        "Unable to bind TCP socket.\n")

    ASSERT(listen(tcp_sock, INT_MAX) < 0, "Unable to listen on the TCP socket.\n")

    // se seteaza file descriptorii socketilor creati pana acum
    FD_ZERO(&active_fds);
    FD_SET(tcp_sock, &active_fds);
    FD_SET(udp_sock, &active_fds);
    FD_SET(0, &active_fds);
    max_fd = tcp_sock;

    // serverul ruleaza pana cand se primeste "exit" de la tastatura
    while(!exit_msg) {
        tmp_fds = active_fds;
        memset(buff, 0, MAXBUFF);

        ASSERT(select(max_fd + 1, &tmp_fds, nullptr, nullptr, nullptr) < 0, "Unable to select.\n")

        for (int i = 0; i <= max_fd; ++i) {
            if (FD_ISSET(i, &tmp_fds)) {
                if (!i) {
                    // s-a primit o comanda de la stdin
                    fgets(buff, MAXBUFF - 1, stdin);

                    if (!strcmp(buff, "exit\n")) {
                        exit_msg = true;
                        break;
                    } else {
                        printf("Only accepted command is `exit`.\n");
                    }
                } else if (i == udp_sock) {
                    // s-a primit un mesaj de la un client UDP
                    ASSERT(recvfrom(udp_sock, buff, MAXBUFF, 0, (sockaddr*) &udp_addr, &udp_socklen)
                        < 0, "Nothing received from UDP socket.\n")

                    // se converteste mesajul in formatul ce va fi transmis catre clientii TCP
                    // sau bufferat
                    tcp_msg.udp_port = ntohs(udp_addr.sin_port);
                    strcpy(tcp_msg.ip, inet_ntoa(udp_addr.sin_addr));
                    udp_msg = (recv_msg_t*)buff;

                    if (decode_message(udp_msg, &tcp_msg)) {
                        if (buffers.find(tcp_msg.topic_name) != buffers.end()) {
                            // daca topicul exista in hashul de buffere, mesajul trebuie bufferat
                            buffers[tcp_msg.topic_name].push_back(tcp_msg);
                        }

                        if (online.find(tcp_msg.topic_name) != online.end()) {
                            // se trimite mesajul tuturor clientilor abonati la acest topic
                            for (int fd : online[tcp_msg.topic_name]) {
                                ASSERT(send(fd, (char*) &tcp_msg, sizeof(tcp_msg_t), 0) < 0,
                                    "Unable to send message to TCP client")
                            }
                        }
                    }
                } else if (i == tcp_sock) {
                    // s-a primit o noua cerere de conexiune TCP
                    new_sock = accept(i, (sockaddr*) &new_tcp, &tcp_socklen);
                    ASSERT(new_sock < 0, "Unable to accept new client.\n")

                    // se dezactiveaza algoritmul lui Nagle pentru conexiunea la clientul TCP
                    setsockopt(new_sock, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));

                    FD_SET(new_sock, &active_fds);

                    if(new_sock > max_fd) {
                        // s-a alocat un socket mai mare decat maximul de pana acum
                        max_fd = new_sock;
                    }

                    ASSERT(recv(new_sock, buff, MAXBUFF - 1, 0) < 0, "No client ID received.\n")

                    strcpy(crt_client.id, buff);
                    crt_client.topics.clear();

                    // se trimite ce s-a bufferat
                    send_buffered_messages(new_sock, buff, online, client_topics, buffers,
                                           clients_per_topic, crt_client);

                    // se se updateaza hash-ul pentru clienti si cel pentru topicuri
                    clients.insert({new_sock, crt_client});
                    client_topics.erase(buff);

                    printf("New client %s connected from %s:%hu.\n", buff,
                           inet_ntoa(new_tcp.sin_addr), ntohs(new_tcp.sin_port));
                } else {
                    // se primeste o comanda de la un client TCP
                    bytes_recv = recv(i, buff, MAXBUFF - 1, 0);
                    ASSERT(bytes_recv < 0, "Nothing received from TCP subscriber.\n")

                    if (!bytes_recv) {
                        printf("Client %s disconnected.\n", clients[i].id);

                        FD_CLR(i, &active_fds);
                        update_max_fd(max_fd, &active_fds);
                        unsubscribe_all(i, online, clients, buffers, client_topics,
                                clients_per_topic);
                        close(i);
                    } else {
                        // s-a primit o comanda de subscribe sau unsubscribe
                        serv_msg = (serv_msg_t*)buff;

                        if (serv_msg->type == 'u') {
                            unsubscribe(i, serv_msg->topic_name, online, clients);
                        } else {
                            subscribe(i, serv_msg, online, clients);
                        }
                    }
                }
            }
        }
    }

    // se inchid toti socketii inca in folosinta
    close_all(&active_fds, max_fd);

    return 0;
}