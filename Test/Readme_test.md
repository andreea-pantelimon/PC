# Colocviu laborator

- se implementeaza o aplicatie client server in care serverul mediaza mesajele criptate dintre 2
clienti TCP;

- clientii cripiteaza cu cheie partajata obtinuta prin algoritmul [Diffie-Hellman](https://en.wikipedia.org/wiki/Diffie%E2%80%93Hellman_key_exchange);

- criptarea se face prin [cifru Caesar](https://en.wikipedia.org/wiki/Caesar_cipher), unde deplasamentul
il reprezinta cheia partajata a celor 2 clienti;

- pentru o comunicare si pentru o folosire eficienta a resurselor, se foloseste *multiplexare I/O*,
colocviul fiind o modificare a [laboratorului 8](https://github.com/teodutu/PC/tree/master/Laburi/Lab8).

## clienti

- se genereaza numarul `g^x mod N`, iar dupa apelul de `connect`, acest numar este transmis in clar
catre server;

- atunci cand se primeste primul mesaj pe conexiunea TCP, acesta e interpretat ca fiind `g^y mod N`,
cu care e calculata cheia partajata `K`;

- incepand de la al doilea mesaj TCP, clientii pot comunica: decriptarea reprezinta criptarea cu chiea
`26 - K`;

- un client inchide conexiunea cu comanda `exit` citita de la `stdin`.

## server

- mediaza conexiunea intre cei 2 clienti;

- nu cunoaste cheia partajata;

- memoreaza primul mesaj al fiecarui client pentru a i-l trimite celuilalt in momentul in care acesta
se conecteaza si trimite si el numarul sau.
