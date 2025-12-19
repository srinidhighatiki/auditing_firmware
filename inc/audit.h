#ifndef INC_AUDIT_H_
#define INC_AUDIT_H_

#include "stdint.h"
#include "stdbool.h"
int compute_sha256(uint8_t *message, uint32_t message_len);

void bin_to_hex();

void f_write(uint32_t StartSectorAddress, uint32_t *Data, uint16_t numberofwords);

char* hashing(uint32_t StartSectorAddress, uint32_t *RxBuf, uint16_t numberofwords);

void write_hash();

bool compare();

bool audit();

#endif /*INC_AUDIT_H_*/
