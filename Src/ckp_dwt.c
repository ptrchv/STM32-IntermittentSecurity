#include "stm32u5xx_hal.h"
#include "ckp_dwt.h"
#include "ckp_mem.h"


static volatile uint32_t *DWT_LAR = (uint32_t *)(0xFB0UL + DWT_BASE); //not mapped in core_cm33.h DWT_Type structure (?)

extern UART_HandleTypeDef huart3;



volatile uint32_t dwt_start DRIVER_BSS;
volatile uint32_t dwt_end DRIVER_BSS;




int32_t dwt_counter_init(void)
{
	//uint32_t dwt_ctrl, dwt_cnt;

//	dwt_ctrl = DWT->CTRL;
//	dwt_cnt = DWT->CYCCNT;

	if ((DWT->CTRL &= 0x02000000) != 0) // counter is implemented? (DWT_CTRL.NOCYCCNT==0)
	{
		return -1;
	}

		DCB->DEMCR |= 0x01000000;  	// global enable of DWT (DEMCR.TRCENA=1)
	//	dwt_ctrl = DWT->CTRL;
	//	dwt_cnt = DWT->CYCCNT;
	//
		*DWT_LAR = 0xC5ACCE55; 	   	// unlock CoreSight Software Lock control for the DWT (set sequence in DWT_LAR)
	//	dwt_ctrl = DWT->CTRL;
	//	dwt_cnt = DWT->CYCCNT;
	//
		DWT->CTRL &= ~0x00800000;	// enable counter for secure (DWT_CTRL.CYCDISS = 1)
	//	dwt_ctrl = DWT->CTRL;
	//	dwt_cnt = DWT->CYCCNT;

	DWT->CYCCNT = 0; 			// reset the counter
	//	dwt_ctrl = DWT->CTRL;
	//	dwt_cnt = DWT->CYCCNT;

	DWT->CTRL |= 1 ; 			// enable the counter
	//	dwt_ctrl = DWT->CTRL;
	//	dwt_cnt = DWT->CYCCNT;

	return 0;
}

int32_t dwt_log_start(void) {
	uint32_t code = TM_LOG_START;
	if (HAL_UART_Transmit(&huart3, (uint8_t*)(&code), 4, 1000) != HAL_OK) {
		return -1;
	}
	return 0;
}


int32_t dwt_log_end(void) {
	uint32_t code = TM_LOG_END;
	if (HAL_UART_Transmit(&huart3, (uint8_t*)(&code), 4, 1000) != HAL_OK) {
		return -1;
	}
	return 0;
}


int32_t dwt_log_time(uint32_t code, uint32_t time)
{
	if (HAL_UART_Transmit(&huart3, (uint8_t*)(&code), 4, 1000) != HAL_OK) {
			return -1;
	}

	if (HAL_UART_Transmit(&huart3, (uint8_t*)(&time), 4, 1000) != HAL_OK) {
			return -1;
	}

	return 0;
}


int32_t test_cycle_counter(void) {
	uint32_t start, end, time;

	if (dwt_counter_init() != 0) {
		return -1;
	}


	time = 0;
	while (1) {
		start = DWT_CNT_NOW;
		if (HAL_UART_Transmit(&huart3, (uint8_t*)(&time), 4, 1000) != HAL_OK){
			break;
		}
		if (HAL_UART_Transmit(&huart3, (uint8_t*)(&start), 4, 1000) != HAL_OK){
			break;
		}
		end = DWT_CNT_NOW;
		time = DWT_DIFF(end, start); // handle possible counter overflow
		HAL_Delay(100);

	}

	return 0; // just to use it

}
