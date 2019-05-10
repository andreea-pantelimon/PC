#include "recv_utils.h"

void send_ACK(msg* t, int received_frame) {
    // se trimite ACK
    // pentru simplitate, numarul pachetului pentru care se da ACK este
    // scris in campul `len` si i se adauga 1 ca sa nu crape link-ul
    t->len = received_frame;
    sprintf(t->payload, "ACK");
    send_message(t);
}

void send_NACK(msg* t, int missing_frame) {
    // se trimite NACK
    // tot, pentru simplitate, numarul pachetului cerut prin NACK este
    // scris in campul `len` si i se adauga 1 ca sa nu crape link-ul
    t->len = missing_frame;
    sprintf(t->payload, "NACK");
    send_message(t);
}

int open_file(char* received_name, int len) {
    // se creeaza numele noului fisier
    char* file_name = calloc((len + 6), sizeof(char));
    strcpy(file_name, "recv_");
    memcpy(file_name + 5, received_name, len);

    // se deschide fisierul si se scriu datele
    int file_desc = open(file_name, O_WRONLY | O_CREAT, 0644);

    free(file_name);

    return file_desc;
}

void write_frames(msg* window, bool* received_frames, short *expected_frame,
                  short total_frames, short window_size, int file_desc) {
    short index;

    // se scrie in fisier prima secventa de cadre primite
    for (; *expected_frame < total_frames && received_frames[*expected_frame];
           ++*expected_frame) {
        if (*expected_frame > 1) {
            index = *expected_frame % window_size;
            write(file_desc, window[index].payload,
                  window[index].len);
        }
    }
}