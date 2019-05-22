#include "utils.h"

void compute_message(char *message, char *line) {
    strcat(message, line);
    strcat(message, "\r\n");
}

void compute_cookie(char *message, char *line) {
    strcat(message, line);
    strcat(message, "; ");
}

int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag) {
    struct sockaddr_in serv_addr;
    int sockfd = socket(ip_type, socket_type, flag);
    ASSERT(sockfd < 0, "Unable to open socket.\n");

    memset(&serv_addr, 0, sizeof(serv_addr));

    // se seteaza parametrii ce desemneaza tipul conexiunii
    serv_addr.sin_family = ip_type;
    serv_addr.sin_port = htons(portno);
    inet_aton(host_ip, &serv_addr.sin_addr);

    // se conecteaza socketul la adresa data
    ASSERT(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0,
            "Unable to connect socket.\n");

    return sockfd;
}

void send_to_server(int sockfd, char *message) {
    int bytes, sent = 0;
    int total = strlen(message);

    // cat timp mai sunt octeti de trimis, acestia sunt scrisi la socket
    do {
        bytes = write(sockfd, message + sent, total - sent);
        ASSERT(bytes < 0, "Unable to send message to socket.\n");

        sent += bytes;
    } while (bytes && sent < total);
}

char *receive_from_server(int sockfd) {
    int bytes, received = 0, total = BUFFLEN;
    char *response = calloc(BUFFLEN, sizeof(char));
    ASSERT(response == NULL, "Unable to allocate memory for response\n");

    // cat timp nu s-a ajuns la dimensiunea maxima a bufferului sau pana cand nu mai sunt octeti
    // de primit, acestia se citesc de la socketul dat ca parametru
    do {
        bytes = read(sockfd, response + received, total - received);
        ASSERT(bytes < 0, "Unable to read message from socket.\n");

        received += bytes;
    } while (bytes && received < total);

    ASSERT(received == total, "Received buffer is full.\n");

    return response;
}

void add_cookies_auth(char* message, cookies_t* cookies, const char* auth_token) {
    char *line = calloc(LINELEN, sizeof(char));
    ASSERT(line == NULL, "Unable to allocate memory for cookie line\n");

    // daca exista, se adauga cookie-urile
    if (cookies != NULL && cookies->size) {
        strcat(message, "Cookie: ");

        for (int i = 0; i < cookies->size - 1; ++i) {
            sprintf(line, "%s", cookies->data[i]);
            compute_cookie(message, line);
        }
        sprintf(line, "%s", cookies->data[cookies->size - 1]);
        compute_message(message, line);
    }

    // se adauga in header si JWT-ul de autorizare, daca acesta exista
    if (auth_token != NULL) {
        sprintf(line, "Authorization: Bearer %s", auth_token);
        compute_message(message, line);
    }

    free(line);
}

char *compute_get_request(const char *host, const char *url, char *url_params,
        cookies_t* cookies, const char* auth_token) {
    // se aloca bufferele ce vor stoca mesajul, respectiv fiecare linie din acesta
    char *message = calloc(BUFFLEN, sizeof(char));
    ASSERT(message == NULL, "Unable to allocate memory for message\n");
    char *line = calloc(LINELEN, sizeof(char));
    ASSERT(line == NULL, "Unable to allocate memory for line\n");

    // se scriu tipul cererii, URL-ul si numele protocolului in mesaj
    if (url_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, url_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }
    compute_message(message, line);

    // se adauga la mesaj IP-ul hostului
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // se adauga cookie-urile si tokenul de autorizare
    add_cookies_auth(message, cookies, auth_token);

    strcat(message, "\r\n");
    free(line);

    return message;
}

char *compute_post_request(const char *host, const char *url, const char *content_type,
        char *form_data, cookies_t* cookies, const char* auth_token) {
    // se aloca bufferele ce vor stoca mesajul, respectiv fiecare linie din acesta
    char *message = calloc(BUFFLEN, sizeof(char));
    ASSERT(message == NULL, "Unable to allocate memory for message\n");
    char *line = calloc(LINELEN, sizeof(char));
    ASSERT(line == NULL, "Unable to allocate memory for line\n");

    // se scriu tipul cererii, URL-ul si numele protocolului in mesaj
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    // se adauga la mesaj IP-ul hostului
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // se aduauga tipul mesajului
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    // se scrie in mesaj si lungimea campului de date
    sprintf(line, "Content-Length: %ld", strlen(form_data));
    compute_message(message, line);

    // se adauga cookie-urile si tokenul de autorizare
    add_cookies_auth(message, cookies, auth_token);

    strcat(message, "\r\n");
    compute_message(message, form_data);
    free(line);

    return message;
}

void add_cookie(char* crt_cookie, cookies_t* cookies, int len) {
    // se face realocare prin dublare pentru o complexitate temporala amortizata de O(1) la
    // adaugare
    if (cookies->size == cookies->max_size) {
        cookies->data = realloc(cookies->data, (cookies->max_size *= 2) * sizeof(char*));
        ASSERT(cookies->data == NULL, "Unable to reallocate memory for cookies\n");
    }

    // se copiaza cookie-ul in lista
    cookies->data[cookies->size] = calloc((len + 1),  sizeof(char));
    ASSERT(cookies->data[cookies->size] == NULL, "Unable to allocate memory for cookie\n");
    strncpy(cookies->data[cookies->size++], crt_cookie, len);
}

void parse_json(JSON_Value *crt_response, json_t* json) {
    JSON_Object* crt_obj = json_object(crt_response);
    json->url = (char*)json_object_get_string(crt_obj, "url");
    ASSERT(json->url == NULL, "No \"url\" field received in response JSON");
    json->method = (char*)json_object_get_string(crt_obj, "method");
    ASSERT(json->method == NULL, "No \"method\" field received in response JSON");
    json->type = (char*)json_object_get_string(crt_obj, "type");

    json->user = (char*)json_object_dotget_string(crt_obj, "data.username");
    json->password = (char*)json_object_dotget_string(crt_obj, "data.password");
    json->data_method = (char*)json_object_dotget_string(crt_obj, "data.method");
    json->data_url = (char*)json_object_dotget_string(crt_obj, "data.url");
    json->token = (char*)json_object_dotget_string(crt_obj, "data.token");

    json->id = (char*)json_object_dotget_string(crt_obj, "data.queryParams.id");
    json->q = (char*)json_object_dotget_string(crt_obj, "data.queryParams.q");
    json->appid = (char*)json_object_dotget_string(crt_obj, "data.queryParams.APPID");
}

void parse_response(char* response, JSON_Value *crt_response, cookies_t* cookies, json_t *json) {
    char *crt_cookie, *cookie_end;
    int len;

    // se citeste partea de JSON
    parse_json(crt_response, json);
    cookies->size = 0;

    // se cauta si se retin toate cookie=urile din antetul HTTP
    while ((crt_cookie = strstr(response, "Set-Cookie: ")) != NULL) {
        cookie_end = strchr(crt_cookie, ';');
        len = cookie_end - crt_cookie - 12;

        add_cookie(crt_cookie + 12, cookies, len);
        response = crt_cookie + 12;
    }
}

void free_strings(char *message, char  *response) {
    free(message);
    free(response);
}

void free_cookies(cookies_t *cookies) {
    for (int i = 0; i < cookies->size; ++i) {
        free(cookies->data[i]);
    }

    cookies->size = 0;
}

char* get_ip(char** full_name) {
    // url-ul care va fi accesat de la IP-ul determinat
    char *url = strchr(*full_name, '/');
    int domain_len = url - *full_name;
    struct addrinfo hints, *result;
    struct sockaddr_in* address;

    char *domain_name = calloc(domain_len + 1, sizeof(char));
    ASSERT(domain_name == NULL, "Unable to allocate memory for domain name\n");
    char *host_ip = calloc(MAX_IP, sizeof(char));
    ASSERT(host_ip == NULL, "Unable to allocate memory for IP string\n");

    // se scriu in `domanin_name` acele caractere din adresa completa care se gasesc inainte de
    // primul '/'
    strncpy(domain_name, *full_name, domain_len);
    memset(&hints, 0, sizeof(hints));

    // se seteaza prametrii cautarii
    hints.ai_family = AF_INET;
    hints.ai_protocol = IPPROTO_TCP;

    // se interogheaza DNS-ul ca sa se afle IP-ul domeniului primit
    ASSERT(getaddrinfo(domain_name, "http", &hints, &result) < 0, "Unable to get domain IP\n");

    // cum s-a restrictionat cautarea la adrese IPv4, prima returnata va fi cea cautata
    address = (struct sockaddr_in*)result->ai_addr;
    ASSERT(inet_ntop(result->ai_family, &address->sin_addr, host_ip, MAX_IP) == NULL,
           "Incorrect IP provided\n");

    strcpy(*full_name, url);

    // se elibereaza memoria pentru numele domeniului si se returneaza IP-ul dorit
    free(domain_name);
    free(result);
    return host_ip;
}

void make_json_value(char* string, JSON_Value** crt_response, cookies_t* cookies) {
    json_value_free(*crt_response);
    free_cookies(cookies);
    *crt_response = json_parse_string(strchr(string, '{'));
}

char* compute_request(char* host, json_t * json, cookies_t* cookies, char* auth_token, char* data,
                      int task) {
    char *message;
    char* url, *method;

    // doar pentru task-ul 5 URL-ul si metoda sunt in campul `data` al JSON-ului
    if (task == 5) {
        auth_token = NULL;
        url = json->data_url;
        method = json->data_method;
    } else {
        url = json->url;
        method = json->method;
    }

    // se determina tipul de cerere ce va fi facuta
    if (!strncmp(method, "GET", 3)) {
        message = compute_get_request(host, url, data, cookies, auth_token);
    } else if (!strncmp(method, "POST", 4)) {
        // de pus doar json
        ASSERT(json->type == NULL, "No \"type\" field in response JSON");
        message = compute_post_request(host, url, json->type, data, cookies, auth_token);
    } else {
        ASSERT(1, "Unknown request type.\n");
        return NULL;
    }

    // doar pentru task-ul 1, se elibereaza memoria folosita pentru `json`, urmand ca la celelalte
    // task-uri, aceasta sa fie eliberata prin JSON_Value-ului in care a fost parsat JSON-ul
    if(task == 1) {
        free(json->url);
        free(json->method);
    }

    return message;
}