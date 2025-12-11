#include "flash.h"
#include "stm32f4xx_hal.h"
#include "string.h"
#include "stdio.h"

static uint32_t GetSector(uint32_t Address)
{
	if (Address < 0x08004000) return FLASH_SECTOR_0;
	if (Address < 0x08008000) return FLASH_SECTOR_1;
	if (Address < 0x0800C000) return FLASH_SECTOR_2;
	if (Address < 0x08010000) return FLASH_SECTOR_3;
	if (Address < 0x08020000) return FLASH_SECTOR_4;
	if (Address < 0x08040000) return FLASH_SECTOR_5;
	if (Address < 0x08060000) return FLASH_SECTOR_6;
	return FLASH_SECTOR_7;
}

uint32_t Flash_Write_Data (uint32_t StartSectorAddress, uint32_t *Data, uint16_t numberofwords)
{
	static FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t SECTORError;

	/*unlock flash*/

	HAL_FLASH_Unlock();

	/*erase flash area*/

	uint32_t StartSector = GetSector(StartSectorAddress);
	uint32_t EndSectorAddress = StartSectorAddress + numberofwords*4 - 1;
	uint32_t EndSector = GetSector(EndSectorAddress);

	EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector = StartSector;
	EraseInitStruct.NbSectors = (EndSector - StartSector) + 1;

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK)
	{
		HAL_FLASH_Lock();
		return HAL_FLASH_GetError();
	}

	for(uint16_t i = 0; i<numberofwords; i++)
	{
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, StartSectorAddress, Data[i]) != HAL_OK)
		{
			HAL_FLASH_Lock();
			return HAL_FLASH_GetError();
		}
		StartSectorAddress += 4;
	}

	/*Lock Flash*/
	HAL_FLASH_Lock();

	return 0;

}


void Flash_Read_Data (uint32_t StartSectorAddress, uint32_t *RxBuf, uint16_t numberofwords)
{
	for(int i = 0; i < numberofwords; i++)
	{
		RxBuf[i] = *(__IO uint32_t*)StartSectorAddress;
		StartSectorAddress += 4;
	}
}

void Convert_To_Str(uint32_t *Data, uint32_t num_words, char *Buf)
{
	for (size_t i = 0; i < (num_words*4); i++)
	{
		Buf[i] = (Data[i/4] >> (8 *(i % 4))) & 0xFF;
	}
	Buf[num_words*4] = '\0';
}
