/*
 * Protocoale de comunicatii:
 * Laborator 6: UDP
 * client mini-server de backup fisiere
 */
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "helpers.h"


void usage(char *file) {
	fprintf(stderr, "Usage: %s ip_server port_server file\n", file);
	exit(0);
}

/*
 * Utilizare: ./client ip_server port_server nume_fisier_trimis
 */
int main(int argc, char **argv) {
	if (argc != 4)
		usage(argv[0]);

	int fd, sock, num_bytes;
	struct sockaddr_in to_station;
	char buff[BUFLEN];
	memset(buff, 0, BUFLEN);

	sock = socket(PF_INET, SOCK_DGRAM, 0); // descriptorul fisierului

	to_station.sin_family = AF_INET;
	to_station.sin_port = htons (atoi(argv[2]));
	inet_aton(argv[1], &(to_station.sin_addr));

	fd = open(argv[3], O_RDONLY);

	sprintf(buff, "%s", argv[3]);
	sendto(sock, buff, strlen(buff), 0, (struct sockaddr *)&to_station, sizeof(to_station));
	memset(buff, 0, BUFLEN);

	while ((num_bytes = read(fd, buff, BUFLEN - 1))) {
		sendto(sock, buff, num_bytes, 0, (struct sockaddr *)&to_station, sizeof(to_station));
		memset(buff, 0, BUFLEN);
	}

	memset(buff, 0, BUFLEN);
	sprintf(buff, "FINISH");
	sendto(sock, buff, 7, 0, (struct sockaddr *)&to_station, sizeof(to_station));

	close(sock);
	close(fd);

	return 0;
}
