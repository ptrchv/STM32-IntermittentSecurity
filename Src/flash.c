#include "flash.h"
#include "stm32u5xx_hal.h"


/*
 * disable cache, unlock access to flash control registers
 */
int32_t flash_start(void)
{
	if (HAL_ICACHE_Disable() != HAL_OK)
	{
		return -1;
	}
	HAL_FLASH_Unlock();
	return 0;
}

/*
 * lock access to flash control registers, enable cache
 */
int32_t flash_end(void)
{
	HAL_FLASH_Lock();
	if (HAL_ICACHE_Enable() != HAL_OK)
	{
		return -1;
	}
	return 0;
}

/*
Parameters:
- FirstPage: 0UL to ...
- NbofPages: 1UL to ...
- BankNumber: FLASH_BANK_1, FLASH_BANK_2, FLASH_BANK_BOTH
- EraseType: FLASH_TYPEERASE_PAGES, FLASH_TYPEERASE_PAGES_NS
*/
int32_t flash_erase(uint32_t FirstPage, uint32_t NbOfPages, uint32_t BankNumber, uint32_t EraseType)
{
	static FLASH_EraseInitTypeDef EraseInitStruct;
	EraseInitStruct.TypeErase   = EraseType;
	EraseInitStruct.Banks       = BankNumber;
	EraseInitStruct.Page        = FirstPage;
	EraseInitStruct.NbPages     = NbOfPages;

	uint32_t PageError = 0; //error page number

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK) // erase flash
	{
		// return HAL_FLASH_GetError();
		return -1;
	}

	return 0;
}


/*
Parameters:
- sbuf: address of buffer to write
- len: len of buffer
- addr: address in flash memory
- WriteType: FLASH_TYPEPROGRAM_QUADWORD, FLASH_TYPEPROGRAM_QUADWORD_NS
Info:
- the start address is aligned with a quad word
*/
int32_t flash_write_qw(uint32_t sbuf, uint32_t len, uint32_t addr, uint32_t WriteType)
{
	uint32_t buf;
	uint64_t qw[2] = {0x0, 0x0};
	uint32_t qw_ptr;

	buf = sbuf;
	while (buf < (sbuf + (len & ~0x0000000F)))
	{
		if (HAL_FLASH_Program(WriteType, addr, buf) == HAL_OK)
		{
			addr += 16;
			buf += 16;
		}
		else {
			return -1;
		}
	}

	if ((len & 0x0000000F) != 0)
	{
		qw_ptr = (uint32_t)(&qw);
		while (buf < (sbuf + len)) {
			*((uint8_t*)qw_ptr) = *((uint8_t*)buf);
			buf += 1;
			qw_ptr += 1;
		}
		if (HAL_FLASH_Program(WriteType, addr, (uint32_t)qw) != HAL_OK)
		{
			return -1;
		}
	}
	return 0;
}


/*
 * when saving .data, HAL_FLASH_Program checkpoints itself in an intermediated state
 * this function restores pFlash structure and unlocks it
 */
void reset_hal_flash_data(void)
{
	// Default pFlash configuration
	pFlash.ErrorCode = HAL_FLASH_ERROR_NONE;
	pFlash.ProcedureOnGoing = 0U;
	pFlash.Address = 0U;
	pFlash.Bank = FLASH_BANK_1;
	pFlash.Page = 0U;
	pFlash.NbPagesToErase = 0U;
	__HAL_UNLOCK(&pFlash);
}



