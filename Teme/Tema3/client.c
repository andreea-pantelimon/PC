#include "utils.h"

int main(void) {
    JSON_Value *crt_response = NULL;
    int sockfd, port = SERVER_PORT;
    json_t json;
    cookies_t cookies;
    char *message = NULL, *response = NULL, *auth_token = NULL;
    char *host_ip = calloc(MAX_IP, sizeof(char));
    ASSERT(host_ip == NULL, "Unable to allocate memory for host IP\n");
    char *data = calloc(BUFFLEN, sizeof(char));
    ASSERT(data == NULL, "Unable to allocate memory for message data\n");

    cookies.size = 0;
    cookies.max_size = 2;
    cookies.data = malloc(2 * sizeof(char*));
    ASSERT(cookies.data == NULL, "Unable to allocate memory for cokies buffer\n");

    // dat fiind faptul ca ultimul task este compus din 2 parti, se considera ca acesta reprezinta
    // 2 taskuri
    for (int task = 1; task <= 6; ++task) {
        // fiecare task introduce niste conditii specifice
        if (task == 1) {
            json.url = calloc(13, sizeof(char));
            ASSERT(json.url == NULL, "Unable to allocate memory for task1 url\n");
            json.method = calloc(5, sizeof(char));
            ASSERT(json.method == NULL, "Unable to allocate memory for task1 method\n");
            strcpy(json.url, "/task1/start");
            strcpy(json.method, "GET");
            strcpy(host_ip, SERVER_IP);
        } else if (task == 2) {
            sprintf(data, "user=%s&pass=%s", json.user, json.password);
            free_cookies(&cookies);
        } else if (task == 3) {
            auth_token = strdup(json.token);
            sprintf(data, "raspuns1=omul&raspuns2=numele&id=%s", json.id);
        } if (task == 5) {
            free(host_ip);
            host_ip = get_ip((char**)&json.data_url);
            port = HTTP_PORT;
            sprintf(data, "q=%s&APPID=%s", json.q, json.appid);
        } if (task == 6) {
            sprintf(data, "%s", strchr(response, '{'));
            strcpy(host_ip, SERVER_IP);
            port = SERVER_PORT;
        }

        // se elibereaza memoria pentru mesajele de la task-ul anterior si se deschide o noua
        // conexiune
        free_strings(message, response);
        sockfd = open_connection(host_ip, port, AF_INET, SOCK_STREAM, 0);
        ASSERT(sockfd < 0, "Unable to connect to server\n");

        // se creeaza si se trimite mesajul pentru acest task, dupa care se asteapta un raspuns
        message = compute_request(host_ip, &json, &cookies, auth_token, data, task);
        send_to_server(sockfd, message);
        response = receive_from_server(sockfd);

        // se parseaza JSON-ul acolo unde este nevoie
        if (task < 5) {
            make_json_value(response, &crt_response, &cookies);
            parse_response(response, crt_response, &cookies, &json);
        }

        // se inchide conexiunea
        close(sockfd);
    }

    // se afiseaza ultimul mesaj primit de la server
    printf("%s\n", response);

    // se elibereaza toata memoria folosita
    free_strings(message, response);
    free(data);
    free(auth_token);
    free(host_ip);
    json_value_free(crt_response);
    free_cookies(&cookies);
    free(cookies.data);

    return 0;
}