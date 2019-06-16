# Lab 7 - TCP

- tot o aplicatie de tip *client-server*;

- la server se conecteaza 2 clienti *TCP* care interschimba mesaje prin intermediu acestuia;

- serverul afiseaza si el mesajele primite de la cei 2 clienti;

## server

- cum inca nu s-a facut multiplexare, fiecare client este tratat intr-un `if` separat (e jegos da'
atata s-a putut);

- se dezactiveaza [algoritmul lui Nagle](https://en.wikipedia.org/wiki/Nagle%27s_algorithm) ca sa nu
apara concatenari/spargeri de mesaje;

## clienti

- ordinea initiala a mesajelor este `client`, `client2`;

- un mesaj nu se primeste pana cand nu se trimite urmatorul mesaj.
