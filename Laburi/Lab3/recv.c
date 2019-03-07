#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10001

int main(int argc,char** argv){
    msg r;
    init(HOST,PORT);
    char check_bit = 0;

    memset(r.payload, 0, MAX_LEN);

    if (recv_message(&r) < 0) {
        perror("Receive message");
        return -1;
    }

    printf("[%s] Got msg with payload: %s\n", argv[0], r.payload);

    for (int i = 0; i < r.len; ++i) {
        for (int j = 0; j < 7; ++j) {
            check_bit ^= (1 << j) & r.payload[i];
        }
    }

    if (check_bit != r.payload[MAX_LEN - 1]) {
        printf("[%s] Error detected\n", argv[0]);
    }

    return 0;
}
