#ifndef UTILS_H
#define UTILS_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "link_emulator/lib.h"

#define MAX_FRAME 1397

typedef struct __attribute__((__packed__)) {
    char payload[MAX_FRAME];
    short frame_number;
    char crc;
} frame_t;

char compute_crc(msg* t);

#endif  // UTILS_H