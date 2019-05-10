#ifndef TCP_UTILS_H
#define TCP_UTILS_H

#include <cstring>
#include "general_utils.h"

#define MAXRECV (sizeof(tcp_msg_t) + 1)

// parseaza o comanda de la tastatura si creeaza din aceasta un mesaj ce va fi trimis serverului
bool make_message(serv_msg_t*, char*);

#endif  // TCP_UTILS_H
