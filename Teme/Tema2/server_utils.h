#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

using namespace std;

#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <cstdint>
#include <cstring>
#include <string>
#include <netinet/in.h>
#include <cmath>
#include <zconf.h>
#include "general_utils.h"

#define topic first
#define topic_data second
#define SF second

// structura retine informatii despre un client
struct client_t {
    char id[11];
    unordered_map<string, bool> topics;
};

// retine informatii despre un topic al unui client care s-a deconectat, salvandu-le in caz ca
// acesta se reconecteaza
struct topic_t {
    bool sf;
    int last_msg;
    string topic_name;

    topic_t(): sf(false), last_msg(0), topic_name("") { }

    topic_t(bool _sf, int index, string& name): sf(_sf), last_msg(index), topic_name(name) { }
};

// structura pentru un mesaj primit de la un client UDP
struct __attribute__((packed)) recv_msg_t {
    char topic_name[50];
    uint8_t type;
    char data[1501];
};

// functia modifica datele dintr-un mesaj in functie de tipul acestuia
bool decode_message(recv_msg_t*, tcp_msg_t*);

// functia updateaza file descriptorul maxim setat
void update_max_fd(int&, fd_set*);

// functia inchide conexiunile pe socketii ce au file descriptorii setati
void close_all(fd_set*, int);

// functia realizeaza dezabonarea unui client TCP de la un anumit topic
void unsubscribe(int, char*, unordered_map<string, unordered_set<int>>&,
    unordered_map<int, client_t>&);

// functia realizeaza abonarea unui client TCP la un anumit topic
void subscribe(int, serv_msg_t*, unordered_map<string, unordered_set<int>>&,
    unordered_map<int, client_t>&);

// functia realizeaza dezabonarea unui client TCP de la toate topicurile sale, pe care le retine
// in eventualitatea unei noi conectari a respectivului client
void unsubscribe_all(int, unordered_map<string, unordered_set<int>>&,
     unordered_map<int, client_t>&, unordered_map<string, vector<tcp_msg_t>>&,
     unordered_map<string, vector<topic_t>>&, unordered_map<string, int>&);

// functia trimite mesajele bufferate pentru clientul cu un anumit ID
void send_buffered_messages(int, char*, unordered_map<string, unordered_set<int>>&,
    unordered_map<string, vector<topic_t>>&, unordered_map<string, vector<tcp_msg_t>>&,
    unordered_map<string, int>&, client_t&);

// updateaza bufferele pentru un anumit topic stiind ca s-au trimis mesajele de la un index dat
void update_buffer(int, string&, unordered_map<string, vector<tcp_msg_t>>&,
    unordered_map<string, vector<topic_t>>&);

#endif  // SERVER_UTILS_H
