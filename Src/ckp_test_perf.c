#include "fram_ckp.h"
#include "ckp_test.h"
#include "ckp_mem.h"
#include "fram.h"
#include "flash.h"
#include "ckp_syscall.h"
#include "ckp_dwt.h"


#if EXPERIMENT == 0
	uint8_t bss_filler[2];

#elif EXPERIMENT == 1
	uint8_t bss_filler[1024];
#elif EXPERIMENT == 2
	uint8_t bss_filler[2048];
#elif EXPERIMENT == 3
	uint8_t bss_filler[3072];
#elif EXPERIMENT == 4
	uint8_t bss_filler[4096];

#elif EXPERIMENT == 5
	uint8_t bss_filler[1024] CONFIDENTIAL_BSS;
#elif EXPERIMENT == 6
	uint8_t bss_filler[2048] CONFIDENTIAL_BSS;
#elif EXPERIMENT == 7
	uint8_t bss_filler[3072] CONFIDENTIAL_BSS;
#elif EXPERIMENT == 8
	uint8_t bss_filler[4096] CONFIDENTIAL_BSS;
#endif


volatile int perf_enabled = 0;
volatile uint32_t perf_start;
volatile uint32_t perf_end;


static int test_full();

#if EXPERIMENT != 0
static void init_data() {
	bss_filler[0] = 1;
	bss_filler[1] = 2;
}
#endif



int test_perf(){
	// test_data_bss(); # does not work if you save stack
	if (test_full() != 0) {
		return -1;
	}
	return 0;
}



static int test_full()
{
#if ! MICROBENCHMARK
	int i = 0;

	HAL_Delay(1000); // used when not using debugger to allow memory to initialize

	// clean flash pages
	DWT_CNT_START;
	if (flash_start() != 0) {
		return -1;
	}
	if (flash_erase(CKP_PAGE_START, 3, 1, FLASH_TYPEERASE_PAGES) != 0) {
		return -1;
	}
	if (flash_end() != 0) {
		return -1;
	}
	DWT_CNT_END; DWT_CNT_LOG(TM_R_PAGE_WIPE);

#if EXPERIMENT != 0
	init_data();
#endif

	for(i=0; i<10; ++i) {
		DWT_CNT_START;
		checkpoint();
		DWT_CNT_END; DWT_CNT_LOG(TM_S_TOT);
	}

	checkpoint();

	perf_end = DWT_CNT_NOW;
	if (perf_enabled == 1) {
		dwt_log_time(TM_R_TOT2, DWT_DIFF(perf_end, perf_start));
	}

	DWT_CNT_START;
	restore();

#else
	int i = 0;

	HAL_Delay(1000); // used when not using debugger to allow memory to initialize

	// clean flash pages
	DWT_CNT_START;
	if (flash_start() != 0) {
		return -1;
	}
	if (flash_erase(CKP_PAGE_START, 3, 1, FLASH_TYPEERASE_PAGES) != 0) {
		return -1;
	}
	if (flash_end() != 0) {
		return -1;
	}
	DWT_CNT_END; DWT_CNT_LOG(TM_R_PAGE_WIPE);

#if EXPERIMENT != 0
	init_data();
#endif

	for(i=0; i<10; ++i) {
		checkpoint();
	}

	restore();
#endif
	return 0;
}


