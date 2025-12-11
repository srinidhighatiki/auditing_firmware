#include "audit.h"
#include "flash.h"
#include "stm32f4xx_hal.h"
#include "string.h"
#include "stdio.h"
#include "cmox_crypto.h"


uint8_t HASH_BIN[32];
char HASH_HEX[65];

int compute_sha256(uint8_t *message, uint32_t message_len)
{
    cmox_hash_retval_t ret = cmox_hash_compute(
                                CMOX_SHA256_ALGO,
                                message, message_len,
                                HASH_BIN, sizeof(HASH_BIN),
                                NULL);

    if (ret == CMOX_HASH_SUCCESS) return 1;

    return 0;
}

void bin_to_hex(){
	for (int i = 0; i<32; i++){
		uint8_t byte = HASH_BIN[i];
		HASH_HEX[2*i] = "0123456789abcdef"[(byte >> 4) & 0xF];
		HASH_HEX[2*i + 1] = "0123456789abcdef"[byte & 0xF];
	}
	HASH_HEX[64] = '\0';
}

void f_write(uint32_t StartSectorAddress, uint32_t *Data, uint16_t numberofwords)
{
	Flash_Write_Data(StartSectorAddress, Data, numberofwords);

}

char* auditing(uint32_t StartSectorAddress, uint32_t *RxBuf, uint16_t numberofwords)
{
	uint32_t num_bytes = numberofwords*4;

	Flash_Read_Data(StartSectorAddress, RxBuf, numberofwords);

	if(!compute_sha256((uint8_t*)RxBuf,num_bytes)) return NULL;

	bin_to_hex();


	return HASH_HEX;
}

/*This returned HASH_HEX needs to be sent to Blockchain*/
