#ifndef RECV_UTILS_H_
#define RECV_UTILS_H_

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <unistd.h> 
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10001
#define MAX_FRAMES 1000  // numarul maxim de cadre posibile (din enunt)

void send_ACK(msg* t, int received_frame);

void send_NACK(msg* t, int missing_frame);

int open_file(char* received_name, int len);

void write_frames(msg* window, bool* received_frames, short *expected_frame,
                  short total_frames, short window_size, int file_desc);

#endif  // RECV_UTILS_H_
