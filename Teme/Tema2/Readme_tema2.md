# Tema 2 Protocoale de Comunicatie

## Server

### Implementare

- informatiile despre ce mesaje trebuie transmise, carui client, despre clientii online si despre
mesajele ce trebuie salvate pentru SF sunt pastrate folosind `unordered_map`-uri sau
`unordered_set`-uri, deoarece aceasta implementare permite un acces eficient, in O(1) la fiecare
topic sau client necesar;

- pentru gestionarea tuturor inputurilor serverului (publisheri UDP, subscriberi TCP, stdin) se
foloseste multiplexarea I/O;

- se creeaza socketi pentru conexiunile UPD si TCP cu mentiunea ca scocketul TCP este unul pasiv, pe
care doar se asculta noi cereri de conexiuni de la subscriberi;

- serverul ruleaza pana cand se primeste de la tastatura comanda `exit`;

- serverul primeste mesajele de la publisherii UDP si le converteste in formatul pe care il va
transmite subscriberilor TCP (formatul cerut in enunt);

- daca topicul pe care s-a primit mesajul de la UDP este unul ale carui mesaje trebuie salvate (un
client TCP care avea SF pe acel topic este acum offline), acest lucru se realizeaza utilizandu-se
map-ul corespunzator;

- apoi, se trimite mesajul tuturor subscriberilor online care sunt abonati la topicul respectivului
mesaj;

- in cazul unei cerereri de conexiune TCP, aceasta se realizeaza, dupa care se verifica in
`unordered_map`-ul pentru topicuri salvate daca noul client nu a mai fost candva online, caz in care
i se reactualizeaza topicurile la care era abonat inainte de deconectare si i se trimit mesajele
pierdute de la topicurile pe care avea SF activ;

- cand vine o cerere de abonare, aceasta se executa, indiferent daca clientul TCP este sau nu deja
abonat la topicul pe care a venit cererea, deoarece `unordered_set` retine elemente unice;

- de asemenea, exista posibilitatea sa vina doua cereri de subscribe pe acelasi topic cu SF-uri
diferite; in acest caz, se ia in considerare ultima cerere, considerandu-se ca subscriberul s-a
"razgandit";

- la unsubscribe se cauta mai intai daca clientul TCP era abonat la topic pentru a nu crea intrari
inutile in `unordered_map`-uri;

- la deconectarea unui subscriber, se salveaza topicurile sale, impreuna cu SF si cu numarul
mesajului de la care trebuie facuta retransimisia in cazul in care acesta se reconecteaza.

### Tratare erori

- daca vreuna dintre operatiile critice esueaza (creare de socketi, apeluri de select, bind sau
listen), serverul se inchide, inchizandu-si si toate conexiunile la socketii sai activi;

- daca mesajul primit de la UDP este eronat (are un format gresit), acesta se ignora, iar executia
programului continua, iar la `stderr` se afiseaza un mesaj de eroare;

- se considera ca mesajele de la clientii UDP nu vor contine `\0` nici in campul de date, nici in
cel de topic.

## Subscriber

### Implementare

- similar serverului, se creeaza un socket de comunicare cu acesta, se dezactiveaza algoritmului lui
Nagle pentru el si se multiplexeaza cu tastatura pentru a primi comenzi de `subscribe`,
`unsubscribe` si `exit`;

- cand se primeste un mesaj de la server, acesta contine fix campurile pe care trebuie sa le afiseze
clientul TCP, ceea ce acesta face imediat;

### Tratare erori

- la fel ca la server, daca vreo operatiune critica esueaza, programul se incheie scriind la
`stderr` eroarea care i-a incheiat executia;

- fiecare comanda este procesata, iar in cazul in care este eronata (numar gresit de parametri,
comenzi diferite de cele permise, topic mai lung de 50 de caractere), se afiseaza un mesaj
corespunzator la `stderr` si comanda se ignora.
