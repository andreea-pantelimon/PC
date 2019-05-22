# Tema 3 PC - Client HTTP

## Implementare

- s-a incercat uniformizarea implementarii fiecarui task, fiind particularizati doar parametrii
oferiti functiilor in functie de task;

- s-au pastrat din laboratorul de *HTTP* functiile de compunere a requesturilor, de scriere si de
citire de la socket, iar din laboratorul de TCP s-a preluat `define`-ul `DIE` modificat pentru a
nu afisa nume de fisier sau numar de linie si redenumit `ASSERT`;

- la fiecare parsare a unui *JSON* se incearca sa se citeasca toate campurile ce pot exista in acesta;

- in cazul in care acestea nu sunt prezente in *JSON-ul* curent, respectivele campuri din structura
`json_t` vor fi `NULL`;

- nu se afiseaza mesaje intermediare (in afara de mesaje de eroare), ci doar mesajul final al
serverului, care marcheaza finalul temei;

- intrucat ultimul task presupune 2 cereri *HTTP*, acesta este considerat 2 taskuri distincte, deci
se considera ca sunt in total 6 astfel de taskuri.

- pe partea de coding style am facut un compromis fata de standardul *ANSI C*: am respectat o limita
de 100 caractere pe linie, in loc de 80, din cauza functiilor lungi din `parson.c` si a mesajelor
de eroare destul de lungi.

## Functionare

- pentru fiecare task, initial se verifica particularizarile sale (daca exista);

- apoi, se sterg vechile mesaje catre si de la server si se deschide o noua conexiune catre IP-ul
necesar si pe portul specificat fie in enuntul temei, fie preluat din *JSON-ul* de la task-ul
precedent;

- in continuare, se creeaza noul mesaj pe baza a ce s-a preluat din raspunsul precedent, a
cookie-urilor si a *JWT-ului*, mesaj care se trimite la server, dupa care se primeste un raspuns;

- in cazul taskurilor 1 - 4, urmatorul pas este acela de a elibera memoria ce stocheaza vechiul
JSON si sa se parseze cel nou primit, creindu-se noi cookieuri si noi parametri pentru taskul
urmator;

- la final, se inchide conexiunea si se reia procesul pentru urmatorul task;

- dupa ce se termina task-urile, se afiseaza ultimul raspuns de la server, cel pentru finalul temei
si se elibereaza toata memoria folosita.
