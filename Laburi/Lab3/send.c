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
    pkt p;
    int file_desc, chunk_size;

    file_desc = open(argv[1], O_RDONLY);

    for (int i = 0; i < 20; ++i) {
        lseek(file_desc, 0, SEEK_SET);
        memset(t.payload, 0, MAX_LEN);
        chunk_size = read(file_desc, p.payload, 1395);
    
        if (chunk_size < 0) {
            perror("Unable to read from input file\n");
        } else {
            t.len = chunk_size;
            p.parity = 0;

            for (int i = 0; i < chunk_size; ++i) {
                for (int j = 0; j < 8; ++j) {
                    p.parity ^= (1 << j) & p.payload[i];
                }
            }
            memcpy(t.payload, p.payload, MAX_LEN);

            printf("[%s] Sending payload: %s\n", argv[0], t.payload);

            send_message(&t);
            recv_message(&t);

            printf("[%s] Recieved response: %s\n", argv[0], t.payload);
        }
    }

    close(file_desc);

    return 0;
}
