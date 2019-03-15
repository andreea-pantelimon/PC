# Lab 4

- se implementeaza un protocol *START-STOP* cu fereastra glisanta;

- fereastra este doar in `send.c`.

## send.c

- primeste ca parametru `BDP`, definit in `run_experiment.sh` ca fiind `DELAY * SPEED * 1000` (deoarece `DELAY` se masoara in *Mbps*, iar `SPEED` in *ms*);

- fereastra contine numarul maxim de cadre (de lungime 1400 bytes) pe care aceasta il poate contine;

- initial se trimit cadrele din fereastra pentru a se umple latimea de banda, fara sa se astepte `ACK`

- apoi, se trimite cate un cadru dupa ce se primeste un `ACK`, pana s-au trimis toate cadrele (`COUNT - window_size` cadre);

- la final, se asteapta `ACK`-urile pentru cadrele care inca nu au primit raspuns, echivalente pentru primele `window_size` cadre;

- nu conteaza ce contine un cadru, deoarece se urmareste doar transmiterea de mesaje, care nu sunt corupte de legatura de date.

## recv.c

- doar primeste cadre si trimite `ACK` atunci cand primeste unul.
