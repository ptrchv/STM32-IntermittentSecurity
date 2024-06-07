/* ... */

/* Includes -----------------------------------------------------------------*/
#include <ckp_dwt.h>
#include "main.h"
#include "ckp_test.h"
#include "ckp_mem.h"
#include "fram_ckp.h"

/* ... */

/* Private variables ---------------------------------------------------------*/

// Delete all variable declarations and add the two below
SPI_HandleTypeDef hspi1 DRIVER_BSS;
UART_HandleTypeDef huart3 DRIVER_BSS;

/* ... */

/* Private function prototypes -----------------------------------------------*/

// REMOVE declarations of MX_AES_Init, MX_SAES_AES_Init and MX_RNG_Init
// Leave all the rest

/* ... */

int main(void)
{
  /* ... */

  /* Initialize all configured peripherals */
  
  // REMOVE calls to MX_AES_Init, MX_SAES_AES_Init and MX_RNG_Init
  // Leave all the rest
  
  /* USER CODE BEGIN 2 */
  
  // Add the following code

  /* log memory structure information */
  dwt_log_time(TM_G_SIZE_DATA, CKP_EDATA - CKP_SDATA);
  dwt_log_time(TM_G_SIZE_BSS, CKP_EBSS - CKP_SBSS);
  dwt_log_time(TM_G_SIZE_DATACONF, CKP_EDATA_CONF - CKP_SDATA_CONF);
  dwt_log_time(TM_G_SIZE_BSSCONF, CKP_EBSS_CONF - CKP_SBSS_CONF);

  dwt_counter_init(); /* setup DWT cycle counter */
  crypto_Init();      /* configure cryto unit and checkpoint key */
  test_perf();	      /* run the performance test */

  /* USER CODE END 2 */

  /* ... */
}

/* ... */ 

// REMOVE definitions MX_AES_Init, MX_SAES_AES_Init and MX_RNG_Init


/* ... */


