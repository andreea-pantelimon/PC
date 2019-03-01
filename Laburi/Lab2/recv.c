#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10001

int main(int argc,char** argv){
    msg r,t;
    init(HOST,PORT);
    int file_desc, size, remaining, chunk;
    char output_file[100];

    memset(r.payload, 0, MAX_LEN);
    memset(t.payload, 0, MAX_LEN);

    if (recv_message(&r) < 0){
        perror("Receive message");
        return -1;
    }

    printf("[%s] Got msg with payload: %s\n", argv[0], r.payload);

    strcat(r.payload, "-out");
    file_desc = open(r.payload, O_WRONLY | O_CREAT, 0777);

    sprintf(t.payload, "ACK(%s)", r.payload);
    t.len = strlen(t.payload) + 1;
    send_message(&t);

    if (recv_message(&r) < 0){
        perror("Receive message");
        return -1;
    }


    printf("[%s] Got msg with payload: %s\n", argv[0], r.payload);

    sprintf(t.payload, "ACK(%s)", r.payload);
    t.len = strlen(t.payload);
    send_message(&t);

    remaining = atoi(r.payload);
    printf("Input file size: %d \n", remaining);

    while (remaining) {
        if (recv_message(&r) < 0) {
            perror("Receive message");
            return -1;
        }

        printf("[%s] Got msg with payload: %s\n", argv[0], r.payload);

        chunk = write(file_desc, r.payload, r.len);

        send_message(&r);
        remaining -= r.len;
        
        memset(t.payload, 0, MAX_LEN);
    }

    close(file_desc);

    return 0;
}
