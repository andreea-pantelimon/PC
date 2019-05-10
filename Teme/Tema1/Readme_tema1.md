# Tema 1 PC: Fereastra glisanta

## Implementare

- protocolul este de tip **Selective repeat**;

- se profita de faptul ca dimensiunea fisierului este mica si acesta se salveaza
integral intr-un buffer in sender pentru a micsora timpul de executie (atunci
cand se trimit pachete, nu se mai asteapta sa se citeasca din fisier);

- asadar, toate pachetele vor fi gata pregatite in momentul transmisiei pe
retea;

- fereastra este implementata cu 2 indici in vectorul de cadre: unul de inceput
si altul de sfarsit al ferestrei, iar glisarea se face prin incrementarea
acestor 2 indici: `first_frame` si `last_frame`;

- orice *ACK* si *NACK* vine insotit de numarul pachetului care a fost primit sau
care este cerut;

- acest numar este stocat in campul len al mesajului, pentru a nu mai fi nevoie
de constructii si casturi aditionale.

## general_utils.c + general_utils.h

- nu mare lucru, doar ceea ce este comun atat pentru `send.c`, cat si pentru
`recv.c`: structura folosita pentru procesarea frameurilor si functia ce
calculeaza `crc`-ul unui pachet.

## send.c + send_utils.c + send_utils.h

- se proceseaza argumentele, se aloca memoria necesara;

- se formeaza primele 2 cadre, care contin numele fisierului de input,
resepectiv numarul de cadre trimise si dimensiunea ferestrei;

- se citesc datele si se pregatesc pachetele;

- se trimite prima fereastra pentru a "umple" legatura de date;

- se asteapta un mesaj timp de `delay + 2` milisecunde;

- valoarea aceasta a fost determinata empiric (intr-o formulare mai putin
pompoasa, am incercat fie sa adun o anumita valoare la `delay`, fie sa inmultesc
`delay` cu valori intre 1 si 2 si am pastrat varianta cu cei mai mici timpi);

- daca se primeste un *ACK*, se marcheaza pachetul corespunzator ca fiind primit,
iar daca *ACK-ul* a venit pe capatul din stanga al ferestrei, aceasta poate glisa,
caz in care se trimit noi pachete, pana cand `first_frame` ajunge din nou pe un
pachet lipsa;

- daca *ACK-ul* a venit pe un pachet diferit de capatul din stanga al ferestrei,
a avut loc o reordonare sau o pierdere (sau corupere) si se retrimit toate
pachetele neprimite de la cel asteptat pana la cel pe care s-a primit *ACK*, de
cel mult 3 ori fiecare;

- si numarul 3 a fost determinat empiric, cu aceeasi paranteza ca mai sus;

- *NACK* se primeste doar pentru pachete corupte, caz in care se retrimite
pachetul cerut de `recv.c`;

- in momentul in care s-a ajuns la *timeout*, legatura de date este (aproape)
goala, deci se trimite o noua fereastra, formata de aceasta data din toate
cadrele neprimite din vechea fereastra;

- la final se elibereaza memoria si se inchide fisierul transmis.

## recv.c + recv_utils.c + recv_utils.h

- s-au definit functii pentru a facilita trimiterea de mesaje de tip *ACK* sau
*NACK* catre `send.c`, dar si pentru crearea numelui si deschiderea fisierului;

- functionarea este similara oarecum cu cea din send.c si anume faptul ca este
folosit un index in bufferul unde sunt stocate datele din fisier:
`first_unreceived_frame` (numele spune tot);

- initial, numarul de cadre si dimensiunea ferestrei nu se stiu, de aceea sunt
initializate cu `1000`, urmand ca atunci cand se primeste pachetul 1, acestea sa
fie updatate la valorile reale primite de la sender;

- in momentul actualizarii, se si realoca memoria pentru bufferul de cadre si
cea pentru vectorul de lungimi;

- cand se primeste un pachet nou, se da *ACK* pe acel pachet si se cauta urmatorul
pachet neprimit;

- *NACK* se da doar in cazul unui pachet corupt, si prin acesta se cere
first_unreceived_frame, deoarece nu se poate sti numarul pachetului corupt;

- cand se primeste numele fisierului de input, se deschide cel de output;

- in momentul in care fereastra gliseaza, se scriu in fisier cadrele care au
primit *ACK* din aceasta;

- glisarea se face prin copierea unui pachet nou peste unul care deja a fost
scris, lucru posibil datorita senderului care nu va trimite niciodata pachete
din afara ferestrei sale, pentru a nu exista coliziuni de indici in fereastra
receiverului.
