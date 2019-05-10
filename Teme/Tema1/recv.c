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
#include "recv_utils.h"

int main (void) {
    init(HOST, PORT);

    msg r;
    frame_t *f;
    int file_desc;
    short total_frames = MAX_FRAMES;
    short expected_frame = 0;  // primul cadru care nu a venit
    short window_size = MAX_FRAMES;
    
    msg* window = malloc(window_size * sizeof(msg));
    bool* received_frames = calloc(total_frames, sizeof(bool));
    // short int index;

    while (expected_frame < total_frames) {
    	if (recv_message(&r) > 0) {
            f = (frame_t*)r.payload;

            printf("[RECV] Expecting frame %hd\n", expected_frame);
            printf("[RECV] Got frame %hd\n", f->frame_number);

    		if (compute_crc(&r) == f->crc) {
                printf("[RECV] Frame is correct\n");

                if (f->frame_number == 0 && !received_frames[0]) {
                    // s-a primit primul cadru, deci se deschide fisierul
                    // de output
                    file_desc = open_file(r.payload, r.len);
                }

    			if (f->frame_number == 1 && !received_frames[1]) {
    				// s-a primit numarul de cadre, deci se suprascrie
    				// total_frames si se realoca vectorii de cadre si lungimi
    				total_frames = atoi(r.payload);
                    window_size = atoi(r.payload + strlen(r.payload) + 1);

                    printf("[RECV] Frames: %hd;", total_frames);
                    printf(" Window size: %hd\n", window_size);

                    window = realloc(window, window_size * sizeof(msg));
                    received_frames = realloc(received_frames,
                        total_frames * sizeof(bool));
    			}

                // daca nu s-a mai primit cadrul curent
    			if (!received_frames[f->frame_number]) {
					// se salveaza cadrul primit in memorie
                    printf("[RECV] New frame\n");

                    // se pune cadrul in fereastra si se marcheaza primirea
                    // acestuia
                    window[f->frame_number % window_size] = r;
                    received_frames[f->frame_number] = true;

					// se cauta urmatorul cadru care lipseste
                    // si se scrie in fisier totul pana la acesta
                    write_frames(window, received_frames, &expected_frame,
                                 total_frames, window_size, file_desc);

                    // pentru a se tine corect evidenta pachetelor in send.c,
                    // se trimite ACK cu numarul pachetului primit
                    send_ACK(&r, f->frame_number);
    			} else {
                    printf("[RECV] Frame already received\n");
                }
    		} else {
    			// s-a primit un pachet gresit, deci se trimite NACK cu numarul
    			// ultimului pachet neprimit
                printf("[RECV] Frame is corrupted\n");
    			send_NACK(&r, expected_frame);
    		}
    	}
    }

    // se elibereaza memoria si se inchide fisierul de output
    free(received_frames);
    free(window);
    close(file_desc);

    printf("[RECV] All written\n");

    send_message(&r);

    return 0;
}
