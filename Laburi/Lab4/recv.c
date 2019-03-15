#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "lib.h"

#define HOST "127.0.0.1"
#define PORT 10001

int main(void) {
	printf("[RECEIVER] Starting.\n");
	init(HOST, PORT);

	msg r;
	
	for (int i = 0; i < COUNT; i++) {
		if (recv_message(&r) < 0) {
			perror("[RECEIVER] Receive error. Exiting.\n");
			return -1;
		} else {
			printf("[RECEIVER] Received message\n");
		}
		
		if (send_message(&r) < 0) {
			perror("[RECEIVER] Send ACK error. Exiting.\n");
			return -1;
		}
	}

	printf("[RECEIVER] Finished receiving..\n");
	return 0;
}
