#include "send_utils.h"

void make_frame(msg* messages, int file_desc, short i) {
    messages[i].len = read(file_desc, messages[i].payload, MAX_FRAME);
    memcpy(messages[i].payload + MAX_FRAME, (uint8_t*)&i, 2);

    // se calculeaza crc-ul ca fiind suma xor a octetilor mesajului
    messages[i].payload[MSGSIZE - 1] = compute_crc(messages + i);
}

void resend_frames(msg* messages, bool* sent_frames, uint8_t* max_resend,
				   short first_frame, int left_limit) {
	// se trimit toate cadrele care nu au primit ACK inca
    for (short i = first_frame; i < left_limit; ++i) {
        if (!sent_frames[i] && max_resend[i]) {
            printf("[SEND] Resending  %hd\n", i);
            send_message(messages + i);
            --max_resend[i];
        }
    }
}

void slide_window(msg* messages, bool* sent_frames, short* first_frame,
				  short* last_frame, short num_frames) {
	// fereastra va glisa pana va incepe cu un cadru ce nu a primit ACK
	for (; *first_frame < num_frames && sent_frames[*first_frame];
           ++*first_frame) {
        if (*last_frame < num_frames) {
            printf("[SEND] Sending %hd\n", *last_frame);
            send_message(messages + (*last_frame)++);
        }
    }
}