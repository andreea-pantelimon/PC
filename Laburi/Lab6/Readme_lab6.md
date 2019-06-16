# Lab 6 - UDP

- se implementeaza un model *client - server*;

- e si o introducere in programarea cu **socketi**;

- se implementeaza in echipe de 2: unul face clientul, celalalt face serverul;

## server

- deschide conexiunea *UDP* si asteapta numele fisierului de la client;

- dupa ce il primeste, creeaza si deschide un fisier cu numele primit si asteapta continutul sau;

- conventia este ca dupa terminarea fisierului, clientul sa-i trimita serverului stringul `"FINISHED"`,
moment in care serverul inchide socektul *UDP* si file descriptorul pentru fisier.

## client

- initial trimite numele fisierului, ca serverul sa stie unde sa scrie;

- apoi, cat timp poate citi, clientul trimite prin *UDP* cate un pachet cu continutul fisierului;

- la final, dupa ce fisierul a fost trimis, clientul mai trimite un ultim mesaj de sfarsit de
transmisiune: `"FINISHED"`.
