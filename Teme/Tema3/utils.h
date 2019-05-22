#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "parson.h"

#define MAX_IP 16
#define BUFFLEN 4096
#define LINELEN 1000
#define SERVER_IP "185.118.200.35"
#define SERVER_PORT 8081
#define HTTP_PORT 80

// define-ul opreste executia programului daca asertiunea este adevarata
#define ASSERT(assertion, call_description)	\
	do {									\
		if (assertion) {					\
			perror(call_description);		\
			exit(EXIT_FAILURE);				\
		}									\
	} while(0)

// structura retine cookie-urile primite de la server
typedef struct {
    char **data;
    int size;
    int max_size;
} cookies_t;

// structura retine toate continuturile posibile ale unui JSON
typedef struct {
    char *url, *method, *type, *user, *password, *id, *token, *data_url;
    char *data_method, *q, *appid;
} json_t;

// functia adauga o linie la un mesaj HTTP
void compute_message(char *message, char *line);

// functia adauga un cookie la un mesaj HTTP
void compute_cookie(char *message, char *line);

// functia deschide o conexiune HTTP
int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag);

// functia trimite un mesaj la un server identificat printr-un socket
void send_to_server(int sockfd, char *message);

// functia returneaza un mesaj primit de la un server identificat printr-un socket
char *receive_from_server(int sockfd);

// functia adauga la un mesaj cookie-urile si tokenul de autorizare
void add_cookies_auth(char* message, cookies_t* cookies, const char* auth_token);

// returneaza un pointer la un mesaj de tip `GET` creat cu parametrii dati
char *compute_get_request(const char *host, const char *url, char *url_params,
    cookies_t* cookies, const char* auth_token);

// returneaza un pointer la un mesaj de tip `POST` creat cu parametrii dati
char *compute_post_request(const char *host, const char *url, const char *content_type,
    char *form_data, cookies_t* cookies, const char* auth_token);

// functia adauga un cookie la lista de cookie-uri primite
void add_cookie(char* crt_cookie, cookies_t* cookies, int len);

// functia populeaza campurile parametrului de tip `json_t` cu tot ce se gaseste in JSON-ul
// primit
void parse_json(JSON_Value *crt_response, json_t* json);

// functia are atat rolul de a parsa JSON-ul, cat si pe cea de a lua cookie-urile din raspunsul
// serverului
void parse_response(char* response, JSON_Value *crt_response, cookies_t* cookies, json_t *json);

// functia elibereaza memoria pentru raspuns si pentru mesajul trimis la server
void free_strings(char *message, char  *response);

// functia sterge cookie=urile retinute pana acum
void free_cookies(cookies_t *cookies);

// functia extrage dintr-o adresa lunga, numele de domeniu, caruia ii afla adresa IPv4
char* get_ip(char** full_name);

// se creeaza un `JSON_Value*` dintr-un string ce reprezinta un raspuns dat de server
// de asemenea, se sterg cookie-urile anterioare
void make_json_value(char* string, JSON_Value** crt_response, cookies_t* cookies);

// functia returneaza un mesaj ce va fi transmis catre server pe baza informatiilor extrase
// dintr-un raspuns anterior al acestuia
char* compute_request(char* host, json_t * json, cookies_t* cookies, char* auth_token, char* data,
    int task);

#endif  // UTILS_H
