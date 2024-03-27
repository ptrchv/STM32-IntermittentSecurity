#ifndef __CKP_MEM_H
#define __CKP_MEM_H

#include "stm32u5xx_hal.h"

/**** Memory attribute definitions *******/
#define CONFIDENTIAL_DATA __attribute__((section (".data.confidential")))
#define CONFIDENTIAL_BSS  __attribute__((section (".bss.confidential")))

#define DRIVER_DATA __attribute__((section (".data.driver")))
#define DRIVER_BSS 	__attribute__((section (".bss.driver")))

/**** Checkpoint flash memory area *******/
#define CKP_FLASH_START 0x0C0BE000UL // start address of checkpoint memory area (must be page aligned)
#define CKP_FLASH_END 	0xC0FDFFFUL  // end of checkpoint memory area

#define CKP_PAGE_START  ((CKP_FLASH_START - FLASH_BASE) / FLASH_PAGE_SIZE)		   // number of first ckp page
//#define NUM_PAGES		((CKP_FLASH_END - CKP_FLASH_START + 1) / FLASH_PAGE_SIZE)  // number of ckp pages
#define NUM_PAGES		3

/***** Checkpoing fram memory area *****/
#define CKP_FRAM_SIZE	0x200000	// 2M of fram

/********* linker script symbols (address of this variables is the real value) ****************/
extern uint32_t _sdata; 			// start of data section
extern uint32_t _edata; 			// end of data section
extern uint32_t _sbss; 				// start of bss section
extern uint32_t _ebss; 				// end of bss section
extern uint32_t _estack; 			// end of stack

extern uint32_t _sdata_conf; 		//start of confidential .data
extern uint32_t _edata_conf; 		//end of confidential .data
extern uint32_t _sbss_conf;			//start of confidential .bss
extern uint32_t _ebss_conf;			//end of confidential .bss

extern uint32_t _scryptobuf; 		// start of crypto buffer
extern uint32_t _ecryptobuf; 		//end of crypto buffer
extern uint32_t _ecryptobuf_data; 	//end of encrypted .data in crypto buffer

/********* extract address from linker symbols ****************/
#define CKP_SDATA 				((uint32_t)&_sdata)
#define CKP_EDATA				((uint32_t)&_edata)
#define CKP_SBSS				((uint32_t)&_sbss)
#define CKP_EBSS				((uint32_t)&_ebss)
#define CKP_ESTACK				((uint32_t)&_estack)

#define CKP_SDATA_CONF	 		((uint32_t)&_sdata_conf)
#define CKP_EDATA_CONF	 		((uint32_t)&_edata_conf)
#define CKP_SBSS_CONF			((uint32_t)&_sbss_conf)
#define CKP_EBSS_CONF			((uint32_t)&_ebss_conf)

#define CKP_SCRYPTOBUF			((uint32_t)&_scryptobuf)
#define CKP_ECRYPTOBUF			((uint32_t)&_ecryptobuf)
#define CKP_ECRYPTOBUF_DATA		((uint32_t)&_ecryptobuf_data)



#endif /* INC_CKP_MEM_H_ */
