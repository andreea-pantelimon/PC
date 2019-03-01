#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10000

int main(int argc,char** argv){
    init(HOST,PORT);
    msg t;
    int file_desc, size, chunk_size;

    memset(t.payload, 0, MAX_LEN);

    sprintf(t.payload, argv[1]);
    t.len = strlen(t.payload) + 1;
    send_message(&t);

    if (recv_message(&t) < 0){
        perror("receive error");
    }
    else {
        printf("[%s] Got reply with payload: %s\n", argv[0] , t.payload);
    }

    file_desc = open(argv[1], O_RDONLY);
    size = lseek(file_desc, 0, SEEK_END);

    sprintf(t.payload, "%d", size);
    t.len = strlen(t.payload) + 1;
    send_message(&t);

    lseek(file_desc, 0, SEEK_SET);

    if (recv_message(&t) < 0){
        perror("receive error");
    }
    else {
        printf("[%s] Got reply with payload: %s\n", argv[0] , t.payload);
    }

    while (chunk_size = read(file_desc, t.payload, MAX_LEN - 1)) {
        if (chunk_size < 0) {
            perror("Unable to read from input file\n");
        } else {
            t.len = chunk_size;
            send_message(&t);

            if (recv_message(&t) < 0){
                perror("receive error");
            }
            else {
                printf("[%s] Got reply with payload: ACK(%s)\n", argv[0] , t.payload);
            }

            memset(t.payload, 0, MAX_LEN);
        }
    }

    close(file_desc);

    return 0;
}
