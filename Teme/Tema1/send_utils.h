#ifndef SEND_UTILS_H_
#define SEND_UTILS_H_

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "general_utils.h"

#define HOST "127.0.0.1"
#define PORT 10000

typedef unsigned char uint8_t;

void make_frame(msg* messages, int file_desc, short int i);

void resend_frames(msg* messages, bool* sent_frames, uint8_t* max_resend,
				   short first_frame, int left_limit);

void slide_window(msg* messages, bool* sent_frames, short* first_frame,
				  short* last_frame, short num_frames);

#endif  // SEND_UTILS_H_