# Lab3 - detectare de erori

- se va folosi un bit de paritate pentru a testa corectitudinea pachetelor trimise

## lib.h

- se defineste structura `pkt` care contine pe langa payload un `int` care reprezinta bitul de paritate.

## send.c

- se va trimite acelasi mesaj de 20 de ori;

- se citeste payloadul intr-un `pkt` si i se calculeaza bitul de paritate;

- se copiaza tot `pkt`-ul in payloadul ce va fi trimis byte cu byte folosind `memcpy` (asa se va copia si intul de paritate) si se trimite mesajul copiat.

## recv.c

- se primeste un `msg`, al carui payload este copiat intrun `pkt`, invers decat in send;

- se verifica bitul de paritate si daca este corect se trimite `"ACK"`, altfel se trimite `"NACK"`.
