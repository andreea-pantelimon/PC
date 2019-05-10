#include "subscriber_utils.h"

bool make_message(serv_msg_t* message, char* buff) {
    // se pune un \0 la final in caz ca nu s-au primit fix 1500 octeti de date
    buff[strlen(buff) - 1] = 0;
    char *token = strtok(buff, " ");
    CHECK(token == nullptr,
          "Commands are `subscribe <topic> <SF>`, `unsubscribe <topic>`, or `exit`.\n")

    if (!strcmp(token, "subscribe")) {
        message->type = 's';
    } else if (!strcmp(token, "unsubscribe")) {
        message->type = 'u';
    } else {
        CHECK(true, "Commands are `subscribe <topic> <SF>`, `unsubscribe <topic>`, or `exit`.\n")
    }

    // se analizeaza al doilea parametru
    token = strtok(nullptr, " ");
    CHECK(token == nullptr,
          "Commands are `subscribe <topic> <SF>`, `unsubscribe <topic>`, or `exit`.\n")
    CHECK(strlen(token) > 50, "Topic name can be at most 50 characters long.\n")
    strcpy(message->topic_name, token);

    if (message->type == 's') {
        // daca este o comanda de `subscribe`, se analizeaza si al treilea parametru
        token = strtok(nullptr, " ");
        CHECK(token == nullptr,
              "Commands are `subscribe <topic> <SF>`, `unsubscribe <topic>`, or `exit`.\n")
        CHECK(token[0] != '0' && token[0] != '1', "SF must be either 0 or 1.\n")
        message->sf = token[0] - '0';
    }

    return true;
}