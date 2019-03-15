#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "lib.h"

#define HOST "127.0.0.1"
#define PORT 10000

int main(int argc, char *argv[]) {
	msg t;

	printf("[SENDER] Starting.\n");
	init(HOST, PORT);

	printf("[SENDER]: BDP=%d\n", atoi(argv[1]));

	int BDP = atoi(argv[1]);

	int window_size = BDP / (MSGSIZE * 8);
	printf("window size: %d\n", window_size);

	for (int i = 0; i < window_size; i++) {
		memset(t.payload, 0, MSGSIZE);
		t.len = MSGSIZE;

		send_message(&t);
	}

	for (int i = 0; i < COUNT - window_size; i++) {
		if (recv_message(&t) < 0) {
			perror("[SENDER] Receive error. Exiting.\n");
			return -1;
		} else {
			printf("[SENDER] Received ACK %s\n", t.payload);
		}

		memset(t.payload, 0, MSGSIZE);
		t.len = MSGSIZE;

		if (send_message(&t) < 0) {
			perror("[SENDER] Send error. Exiting.\n");
			return -1;
		}
	}


	for (int i = 0; i < window_size; i++) {
		if (recv_message(&t) < 0) {
			perror("[SENDER] Receive error. Exiting.\n");
			return -1;
		} else {
			printf("[SENDER] Received ACK %s\n", t.payload);
		}
	}

	printf("[SENDER] Job done, all sent.\n");

	return 0;
}
