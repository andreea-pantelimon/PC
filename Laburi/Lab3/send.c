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
    file_desc = open(argv[1], O_RDONLY);
    chunk_size = read(file_desc, t.payload, MAX_LEN - 2);
    
    if (chunk_size < 0) {
        perror("Unable to read from input file\n");
    } else {
        t.len = chunk_size;

        for (int i = 0; i < chunk_size; ++i) {
            for (int j = 0; j < 8; ++j) {
                t.payload[MAX_LEN - 1] ^= (1 << j) & t.payload[i];
            }
        }

        printf("[%s] Sending payload %s\n", argv[0], t.payload);
        send_message(&t);
    }

    close(file_desc);

    return 0;
}
