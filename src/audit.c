
#include "audit.h"
#include "flash.h"
#include "stm32f4xx_hal.h"
#include "string.h"
#include "stdio.h"
#include "cmox_crypto.h"
#include "stdbool.h"

uint8_t HASH_BIN[32], COMPARE[32];
char HASH_HEX[65];

uint32_t FLAG = 0x08060000;
uint32_t HASH_IN_FLASH = 0x08060004; // this is address of flash memory where hash digest will be stored

uint32_t Buffer;

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

char* hashing(uint32_t StartSectorAddress, uint32_t *RxBuf, uint16_t numberofwords)
{
	uint32_t num_bytes = numberofwords*4;

	Flash_Read_Data(StartSectorAddress, RxBuf, numberofwords);

	if(!compute_sha256((uint8_t*)RxBuf,num_bytes)) return NULL;

	bin_to_hex();


	return HASH_HEX;
}

void write_hash()
{
	uint32_t flag;
	flag = 0x00000000;
	Flash_Write_Data(FLAG, &flag, 1);
	Flash_Write_Data(HASH_IN_FLASH, (uint32_t*)HASH_BIN, 8);
}

bool compare()
{
	Flash_Read_Data(HASH_IN_FLASH,(uint32_t*) COMPARE, 8);
	for(int i = 0; i< 32; i++){
		if (COMPARE[i] != HASH_BIN[i]) return false;
	}
	return true;
}
bool audit()
{
	Flash_Read_Data(FLAG, &Buffer, 1);
	if (Buffer != 0x00000000){
		write_hash();
		return true;
	}
	else return compare();
}

/*This returned HASH_HEX needs to be sent to Blockchain*/
