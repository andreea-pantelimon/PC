#include "general_utils.h"

char compute_crc(msg* t) {
	// se returneaza crc-ul unui anumit mesaj

	char crc = 0;

	for (short i = 0; i < MSGSIZE - 1; ++i) {
		crc ^= t->payload[i];
	}

	return crc;
}