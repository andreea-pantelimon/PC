# Lab2

- se implementeaza protocolul **START-STOP** pentru transmiterea continutului unui fisier in bucati;

- dupa fiecare mesaj trimis se asteapta un raspuns (*ACK*), care contine din nou mesajul oferit.

## send.c

- se transmite numele fisierului catre `recv.c` si se asteapta *ACK*-ul;

- apoi, se va trimite dimensiunea fisierului, calculata cu `lseek`;

- in continuare se citeste continutul fisierului de input in bucati de `MAX_LEN - 1` elemente;

- aceste bucati sunt trimise pe rand la `recv.c`, iar urmatoarea bucata este citita si trimisa doar dupa ce se primeste *ACK*-ul.

## recv.c

- initial se primeste numele fisierului de input de la `send.c` si se creeaza si deschide fisierul cu acelasi nume concatenat cu `"-out"`;

- apoi, se primeste dimensiunea fisierului si se trimite *ACK*-ul corespunzator;

- cat timp mai sunt caractere de citit, se primesc bucati din fisier de la `send.c` si se scriu in fisierul de output;

- in ambele fisiere se reseteaza bufferle folosind `memset` dupa fiecare citire/scriere.
