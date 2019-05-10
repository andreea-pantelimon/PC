#ifndef UTILS_H
#define UTILS_H

#include <cstdlib>
#include <cstdio>
#include <cstdint>

#define MAXBUFF 1552

// analizeaza o conditie si, daca aceasta este adevarata, inchide programul afisand o eroare data
#define ASSERT(assertion, call_description)				\
    if (assertion) {					\
        fprintf(stderr, "ERROR: " call_description);			\
        exit(EXIT_FAILURE);				\
    }

// similar define-ului de mai sus, dar acesta nu inchide programul daca asertiunea este adevarata,
// ci returneaza `false`
#define CHECK(assertion, call_description)				\
    if (assertion) {					\
        fprintf(stderr, call_description);			\
        return false;				\
    }

// structura pentru un mesaj transmis la un client TCP
struct __attribute__((packed)) tcp_msg_t {
    char ip[16];
    uint16_t udp_port;
    char topic_name[51];
    char type[11];
    char data[1501];
};

// structura pentru un mesaj transmis la server de catre un client TCP
struct __attribute__((packed)) serv_msg_t {
    char type;
    char topic_name[51];
    bool sf;
};

#endif  // UTILS_H
