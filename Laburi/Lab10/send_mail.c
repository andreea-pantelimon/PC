#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

/*
 * Macro de verificare a erorilor
 * Exemplu:
 *     int fd = open(file_name, O_RDONLY);
 *     DIE(fd == -1, "open failed");
 */
#define DIE(assertion, call_description)	\
	do {									\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",	\
					__FILE__, __LINE__);	\
			perror(call_description);		\
			exit(EXIT_FAILURE);				\
		}									\
	} while(0)

#define SMTP_PORT       25
#define POP3_PORT       1100
#define MAXLEN          1000
#define SERVER_IP       "3.94.74.82"
#define USER_BASE64     "ZGY1OTBhOTRkMzVmMjg="
#define PASS_BASE64     "YzU0MDc2NzA0ODQzMTU="
#define NEWLINE         "\r\n"

/**
 * Trimite o comanda si asteapta raspuns de la server.
 * Comanda trebuie sa fie in buffer-ul sendbuf.
 * Sirul expected contine inceputul raspunsului pe care trebuie
 * sa-l trimita serverul in caz de succes (de exemplu codul 250).
 * Daca raspunsul semnaleaza o eroare, se iese din program.
 */
void send_command(int sockfd, char sendbuf[], char *expected) {
	char recvbuf[MAXLEN];
	int nbytes;
	char CRLF[3];

	CRLF[0] = 13;
	CRLF[1] = 10;
	CRLF[2] = 0;
	strcat(sendbuf, CRLF);

	printf("Trimit: %s", sendbuf);
	nbytes = write(sockfd, sendbuf, strlen(sendbuf));
	DIE(nbytes < 0, "send_command write");

	nbytes = read(sockfd, recvbuf, MAXLEN - 1);
	DIE(nbytes < 0, "send_command read");

	recvbuf[nbytes] = 0;
	printf("Am primit: %s", recvbuf);

	if (strstr(recvbuf, expected) != recvbuf) {
		printf("Am primit mesaj de eroare de la server!\n");
		exit(-1);
	}
}

void handle_smtp() {
	int sockfd;
	struct sockaddr_in servaddr;
	char sendbuf[MAXLEN];
	char recvbuf[MAXLEN];

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Eroare la creare socket.\n");
		exit(-1);
	}

	// TODO: completat informatii despre server-ul SMTP in variabila servaddr
	inet_aton(SERVER_IP,&servaddr.sin_addr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SMTP_PORT);
	// conectare la server-ul SMTP
	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) {
		printf("Eroare la conectare\n");
		exit(-1);
	}

	// citire raspuns initial de la server
	int n = read(sockfd, recvbuf, MAXLEN -1);
	DIE(n < 0, "read");
	printf("Am primit: %s\n", recvbuf);

	// trimis prima comanda la server
	sprintf(sendbuf, "HELO Teo");
	send_command(sockfd, sendbuf, "250");

	// TODO: trimis cerere de log in si citit raspuns
	sprintf(sendbuf, "AUTH LOGIN");
	send_command(sockfd, sendbuf, "334");
        
	// TODO: trimis nume utilizator base 64 si citit raspuns
	sprintf(sendbuf, USER_BASE64);
	send_command(sockfd, sendbuf, "334");

	// TODO: trimis parola base 64 si citit raspuns
	sprintf(sendbuf, PASS_BASE64);
	send_command(sockfd, sendbuf, "235");
	
	// TODO: trimis sursa mail si citit raspuns
	sprintf(sendbuf, "MAIL FROM: <me@me.com>");
	send_command(sockfd, sendbuf, "250");
	
	// TODO: trimis destinatie mail si citit raspuns
	sprintf(sendbuf, "RCPT TO: <ceva@labpc.com>");
	send_command(sockfd, sendbuf, "250");
	
	// TODO: trimis comanda de inceput de date si citit raspuns
	sprintf(sendbuf, "DATA");
	send_command(sockfd, sendbuf, "354");
	
	// TODO: trimis mail-ul propriu-zis si citit raspuns
	sprintf(sendbuf, "Subject: Lab10 PC" NEWLINE
                    "From: Teo Dutu <teodor@dutu.com>" NEWLINE
                    "To: Andrei Ionescu <ceva@altceva.com>" NEWLINE
                    "Mime-Version: 1.0" NEWLINE
                    "Content-Type: multipart/mixed; boundary=rrr" NEWLINE NEWLINE
                    "--rrr" NEWLINE
                    "Content-Type: text/plain" NEWLINE NEWLINE
                    "**** PSD mai putin generic" NEWLINE NEWLINE
                    "--rrr" NEWLINE
                    "Content-Type: text/plain" NEWLINE
                    "Content-Disposition: attachment; filename=\"textfile.txt\"" NEWLINE NEWLINE
                    "Text file" NEWLINE
                    "--rrr" NEWLINE
                    "." NEWLINE);
	send_command(sockfd, sendbuf, "250");
	close(sockfd);
}

void handle_pop3(char* mail_num) {
	int sockfd;
	struct sockaddr_in servaddr;
	char sendbuf[MAXLEN];
	char recvbuf[MAXLEN];

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Eroare la creare socket.\n");
		exit(-1);
	}

	// TODO: completat informatii despre server-ul POP3 in variabila servaddr
	inet_aton(SERVER_IP,&servaddr.sin_addr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(POP3_PORT);
	// conectare la server-ul POP3
	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) {
		printf("Eroare la conectare\n");
		exit(-1);
	}

	// citire raspuns initial de la server
	int n = read(sockfd, recvbuf, MAXLEN -1);
	DIE(n < 0, "read");
	printf("Am primit: %s\n", recvbuf);

	// TODO: trimis cerere de log in si citit raspuns
	sprintf(sendbuf, "AUTH LOGIN");
	send_command(sockfd, sendbuf, "+");
	// TODO: trimis nume utilizator base 64 si citit raspuns
	sprintf(sendbuf, USER_BASE64);
	send_command(sockfd, sendbuf, "+");

	// TODO: trimis parola base 64 si citit raspuns
	sprintf(sendbuf, PASS_BASE64);
	send_command(sockfd, sendbuf, "+OK");
	// TODO: trimis cerere de listare a mail-urilor si citit raspuns
	sprintf(sendbuf, "LIST");
	send_command(sockfd, sendbuf, "+OK");
	
	sprintf(sendbuf, "RETR %s", mail_num);
	send_command(sockfd, sendbuf, "+OK");

	close(sockfd);
}

int main(int argc, char **argv) {
	// handle_smtp();
	handle_pop3(argv[1]);
}
