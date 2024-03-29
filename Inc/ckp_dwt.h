#ifndef __CKP_DWT_H
#define __CKP_DWT_H

#include <stdint.h>

int32_t test_cycle_counter(void);
int32_t dwt_counter_init(void);
int32_t dwt_log_time(uint32_t code, uint32_t time);
int32_t dwt_log_start(void);
int32_t dwt_log_end(void);


#define DWT_CNT_NOW (DWT->CYCCNT)
#define DWT_DIFF(end, start)  ((end) >= (start) ? (end) - (start) : (~((start) - (end))) + 1)


extern volatile uint32_t dwt_start;
extern volatile uint32_t dwt_end;


#define DWT_CNT_START		(dwt_start = DWT->CYCCNT)
#define DWT_CNT_END			(dwt_end = DWT->CYCCNT)
#define DWT_CNT_LOG(X)		(dwt_log_time((X), DWT_DIFF(dwt_end, dwt_start)))

// CMD
#define TM_LOG_START			0x12345678
#define TM_LOG_END				0x87654321


// GENERAL
#define TM_G_TEST_NUM			1

#define TM_G_SIZE_DATA			2
#define TM_G_SIZE_BSS			3
#define TM_G_SIZE_STACK			4

#define TM_G_SIZE_DATACONF		5
#define TM_G_SIZE_BSSCONF		6

#define TM_G_SIZE_DATADRVR		7
#define TM_G_SIZE_BSSDRVR		8


// SAVE
#define TM_S_TOT				101

#define TM_S_GEN_IV				102

#define TM_S_SPI_DATA			103
#define TM_S_SPI_BSS			104
#define TM_S_SPI_STACK			105
#define TM_S_SPI_CRYPTO			106
#define TM_S_SPI_TAGS			107

#define TM_S_AUTH_DATA			108
#define TM_S_AUTH_BSS			109
#define TM_S_AUTH_STACK			110

#define TM_S_CPY_DRIVERS		111

#define TM_S_FLASH_NONCE		112


// RESTORE
#define TM_R_TOT1				201
#define TM_R_TOT2				202


#define TM_R_DECRYPT_KEY		203
#define TM_R_PAGE_WIPE			204

#define TM_R_FLASH_NONCE		205

#define TM_R_SPI_DATA			206
#define TM_R_SPI_BSS			207
#define TM_R_SPI_STACK			208
#define TM_R_SPI_CRYPTO			209
#define TM_R_SPI_TAGS			210

#define TM_R_AUTH_DATA			211
#define TM_R_AUTH_BSS			212
#define TM_R_AUTH_STACK			213

#define TM_R_CPY_DRIVERS		214



#endif
