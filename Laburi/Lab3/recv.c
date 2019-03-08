#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10001

int main(int argc,char** argv){
    init(HOST,PORT);
    msg r;
    pkt p;
    int check_bit;

    for (int i = 0; i < 20; ++i) {
        memset(r.payload, 0, MAX_LEN);
        check_bit = 0;

        if (recv_message(&r) < 0) {
            perror("Unable to recieve message");
            return -1;
        }

        memcpy(p.payload, r.payload, MAX_LEN);
        printf("[%s] Got msg with payload: %s\n", argv[0], p.payload);

        for (int i = 0; i < r.len; ++i) {
            for (int j = 0; j < 8; ++j) {
                check_bit ^= (1 << j) & p.payload[i];
            }
        }

        memset(r.payload, 0, MAX_LEN);
        
        if (p.parity != check_bit) {
            printf("[%s] Error detected\n", argv[0]);
            sprintf(r.payload, "NACK");

        } else {
            printf("[%s] Message is correct\n", argv[0]);
            sprintf(r.payload, "ACK");
        }

        send_message(&r);
    }
    

    return 0;
}
