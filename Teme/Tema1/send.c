#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include "link_emulator/lib.h"
#include "general_utils.h"
#include "send_utils.h"

int main(int argc, char** argv) {
    init(HOST, PORT);

    if (argc != 4) {
        perror("Wrong number of arguments\n");
        exit(-1);
    }
  
    int speed = atoi(argv[2]);
    int delay = atoi(argv[3]);
    int file_desc = open(argv[1], O_RDONLY);
    int file_size = lseek(file_desc, 0, SEEK_END);
    short num_frames = file_size / MAX_FRAME;
    short window_size = speed * delay * 1000 / (sizeof(msg) * 8);
    frame_t f;
    msg t;

    if (num_frames * MAX_FRAME != file_size) {
        ++num_frames;
    }
    num_frames += 2;  // se trimit si numele fisierului si numarul de cadre

    // dimensiunea ferestrei nu trebuie sa depaseasca numarul de cadre trimise
    window_size = window_size < num_frames ? window_size : num_frames;

    // se aloca memorie pentru toate cadrele ce vor fi citite din fisier
    msg* messages = calloc(num_frames, sizeof(msg));

    // tine evidenta cadrelor trimise
    bool* sent_frames = calloc(num_frames, sizeof(bool));

    // contor de retrimiteri pentrus acelasi cadru
    uint8_t* max_resend = malloc(num_frames * sizeof(uint8_t));

    for (int i = 0; i < num_frames; ++i) {
        max_resend[i] = 3;
    }

    // se creeaza primul cadru, ce contine numele fisierului
    strcpy(f.payload, argv[1]);
    messages[0].len = strlen(argv[1]);
    f.frame_number = 0;
    memcpy(messages[0].payload, f.payload, MSGSIZE - 1);
    messages[0].payload[MSGSIZE - 1] = compute_crc(messages);

    // se creeaza al doilea cadru ce contine dimensiunea fisierului ce va fi
    // transmis si dimensiunea ferestrei
    sprintf(f.payload, "%hd", num_frames);
    sprintf(f.payload + strlen(f.payload) + 1, "%hd", window_size);
    messages[1].len = 5;
    f.frame_number = 1;
    memcpy(messages[1].payload, f.payload, MSGSIZE - 1);
    messages[1].payload[MSGSIZE - 1] = compute_crc(messages + 1);

    lseek(file_desc, 0, SEEK_SET);

    // se citesc datele din fisier
    for (short i = 2; i < num_frames; ++i) {
        make_frame(messages, file_desc, i);
    }

    // se trimite fereastra
    for (short i = 0; i < window_size; ++i) {
        send_message(messages + i);
    }

    int timeout_time = delay + 2;  // determinat empiric
    short last_frame = window_size - 1;  // ultimul cadru din fereastra

    // se trimit restul mesajelor de cate ori este nevoie
    for (short first_frame = 0; first_frame < num_frames;) {
        printf("[SEND] Expecting ACK %hd\n", first_frame);

        if (recv_message_timeout(&t, timeout_time) > 0) {
            if (!strcmp(t.payload, "ACK")) {
                // daca se primeste ACK, pachetul s-a trimis corect
                sent_frames[t.len] = true;
                printf("[SEND] Got ACK %d\n", t.len);

                // daca nu s-a primit ACK pe primul cadru, s-a produs o
                // reordonare sau o pierdere, deci se retrimit cadrele
                // pana la cel cu ACK de maximum 3 ori
                if (first_frame != t.len ) {
                    resend_frames(messages, sent_frames, max_resend,
                                  first_frame, t.len);
                }

                slide_window(messages, sent_frames, &first_frame,
                             &last_frame, num_frames);
            } else {
                // sau s-a primit NACK, deci pachetul s-a primit corupt,
                // deci se retrimite
                printf("[SEND] Got NACK on %d\n", t.len);
                send_message(messages + t.len);
            }
        } else {
            // la timeout, legatura de date e goala si trebuie retrimisa o noua
            // fereastra (sau ce a mai ramas din totalul de cadre)
            printf("[SEND] Reached timeout; resending from %hd to %hd\n",
                first_frame, last_frame);

            for (short i = first_frame; i <= last_frame; ++i) {
                if (!sent_frames[i]) {
                    printf("[SEND] Resending %hd\n", i);
                    send_message(messages + i);
                }
            }
        }
    }

    printf("[SEND] All sent. Transission is over\n");

    // se elibereaza memoria si se inchide fisierul
    free(sent_frames);
    free(messages);
    close(file_desc);
    free(max_resend);

    recv_message(&t);

    return 0;
}
