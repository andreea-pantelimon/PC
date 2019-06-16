# Lab 8 - multiplexare

- functioneaza similar cu laboratorul anterior, dar pot fi mai mult de 2 clienti;

- din acest motiv, este nevoie de multiplexarea conexiunilor si a mesajelor care vin atat pe socketii
serverului cat si pe cei ai clientilor;

## server

- se multiplexeaza socketii activi asociati conexiunilor la clientii *TCP*, cel pasiv pe care vin
conexiunile si stdin pe care vine comanda `exit` care inchide toate conexiunile si termina programul;

- se afiseaza mesaje de conectare/deconectare client;

- cand  un client se deconecteaza, cei ramasi sunt anuntati de acest lucru de catre server.

## client

- multiplexeaza `stdin` si socketul prin care se face conexiunea la server;

- transmite un mesaj care va fi primit prin intermediul serverului de toti ceilalti clienti.
