#include "fram_ckp.h"
#include "fram.h"
#include "ckp_mem.h"
#include "flash.h"

#define TIMEOUT_VALUE 	  0xFF
#define PLAINTEXT_SIZE    4 /* Plaintext size in Words */
#define KEY_SIZE          4

extern SPI_HandleTypeDef hspi1;

static RNG_HandleTypeDef hrng DRIVER_BSS;
static CRYP_HandleTypeDef hcryp DRIVER_BSS;
// AES
static uint32_t pKeyAES[4] = {0xC939CC13,0x397C1D37,0xDE6AE0E1,0xCB7C423C};
static uint32_t pInitVectAES[4] DRIVER_DATA = {0xB3D8CC01,0x7CBB89B3,0x9E0F67E2,0x00000002};
// SAES
static uint32_t pInitVectSAES[4] = {0x00010203,0x04050607,0x08090A0B,0x0C0D0E0F};
static uint32_t Encryptedkey[4];

// information for correct checkpointing
static volatile uint32_t stack_len = 0;
static volatile uint32_t flash_addr = CKP_FLASH_START;
static volatile uint32_t fram_addr = 0;
static uint32_t fram_stack_ptr;


int32_t crypto_Init(void);
static int32_t crypto_AuthEncrypt(uint8_t* header, uint32_t header_size, uint8_t* buf_in, uint32_t buf_size, uint8_t* buf_out, uint32_t tag[]);
static int32_t crypto_AuthDecrypt(uint8_t* header, uint32_t header_size, uint8_t* buf_in, uint32_t buf_size, uint8_t* buf_out, uint32_t tag[]);

static int32_t generateIV(uint32_t* IV);
static int32_t saveNonce(uint32_t* IV, uint32_t fram_addr, uint32_t addr);
static void restoreNonce(uint32_t* IV, uint32_t* fram_addr, uint32_t addr);
static int32_t findNonce(uint32_t* addr);
static inline void updateFramAddr(volatile uint32_t* fram_addr);


static int32_t RNG_Init(void);
static int32_t SAES_Shared_Init(void);
static int32_t GCM_AES_Shared_Init(void);
static void copy_mem(uint32_t addr_src, uint32_t addr_dest, uint32_t size);


uint8_t buf_data_before[300] DRIVER_BSS;
uint8_t buf_data_after[300] DRIVER_BSS;


uint8_t crypto_data_before[300] DRIVER_BSS;
uint8_t crypto_data_after[300] DRIVER_BSS;


uint32_t tags_before[12] DRIVER_BSS;
uint32_t tags_after[12] DRIVER_BSS;


int32_t write_ckp_fram(uint32_t stack_ptr)
{
	uint32_t IV_start = 0;
	uint32_t fram_ptr;							// initialize pointer to FRAM
	//uint32_t tags[12];						// space for 3 tags

	fram_ptr = fram_addr;						// get location of checkpoint
	stack_len = CKP_ESTACK - stack_ptr + 1; 	// update stack length


	if (generateIV(pInitVectAES) != 0) {
		return -1;
	}
	IV_start = pInitVectAES[2];


//	__disable_irq();
//	// copy driver status (must be done before any operation)
//	copy_mem(CKP_SDATA, CKP_SDATA_CONF, CKP_SDATA_CONF - CKP_SDATA);
//	copy_mem(CKP_SBSS, CKP_SBSS_CONF, CKP_SBSS_CONF - CKP_SBSS);
//	__enable_irq();


	copy_mem(CKP_EDATA_CONF, (uint32_t)buf_data_before, CKP_EDATA - CKP_EDATA_CONF);

	// reserve space for tags (3 tags of 128 bit (16 bytes) each)
	fram_ptr += 48;

	// save .data, generate tag
	if (CY15X102QN_Write(&hspi1, (uint8_t *)CKP_EDATA_CONF, CKP_EDATA - CKP_EDATA_CONF, fram_ptr) != 0) {
		return -1;
	}
	if (crypto_AuthEncrypt((uint8_t *)CKP_EDATA_CONF, CKP_EDATA - CKP_EDATA_CONF, (uint8_t *)CKP_SDATA_CONF, CKP_EDATA_CONF - CKP_SDATA_CONF, (uint8_t *)CKP_SCRYPTOBUF, &tags_before[0]) != 0) {
		return -1;
	}
	pInitVectAES[2] += 1;
	fram_ptr += (CKP_EDATA - CKP_EDATA_CONF);


	//copy_mem(CKP_SDATA_CONF, (uint32_t)buf_data_after, CKP_EDATA - CKP_SDATA_CONF);
	copy_mem(CKP_SCRYPTOBUF, (uint32_t)crypto_data_before, CKP_ECRYPTOBUF_DATA - CKP_SCRYPTOBUF);


	// save .bss, generate tag
	if (CY15X102QN_Write(&hspi1, (uint8_t *)CKP_EBSS_CONF, CKP_EBSS - CKP_EBSS_CONF, fram_ptr) != 0) {
		return -1;
	}
	if (crypto_AuthEncrypt((uint8_t *)CKP_EBSS_CONF, CKP_EBSS - CKP_EBSS_CONF, (uint8_t *)CKP_SBSS_CONF, CKP_EBSS_CONF - CKP_SBSS_CONF, (uint8_t *)CKP_ECRYPTOBUF_DATA, &tags_before[4]) != 0) {
		return -1;
	}
	pInitVectAES[2] += 1;
	fram_ptr += (CKP_EBSS - CKP_EBSS_CONF);

	// save .cryptobuf
	if (CY15X102QN_Write(&hspi1, (uint8_t *)CKP_SCRYPTOBUF, CKP_ECRYPTOBUF - CKP_SCRYPTOBUF, fram_ptr) != 0) {
		return -1;
	}
	fram_ptr += (CKP_ECRYPTOBUF - CKP_SCRYPTOBUF);

	// save stack, generate tag
	if (CY15X102QN_Write(&hspi1, (uint8_t *)stack_ptr, stack_len, fram_ptr) != 0) {
		return -1;
	}
	if (crypto_AuthEncrypt((uint8_t *)stack_ptr, stack_len, NULL, 0, NULL, &tags_before[8]) != 0) {
		return -1;
	}

	// save tags
	if (CY15X102QN_Write(&hspi1, (uint8_t *)tags_before, 48, 0) != 0) {
		return -1;
	}

	// save IV in flash
	pInitVectAES[2] = IV_start;
	if (saveNonce(pInitVectAES, fram_addr, flash_addr) != 0) {
		return -1;
	}

	// update checkpoint information
	flash_addr += 16;
	updateFramAddr(&fram_addr);

	return 0;
}


int32_t restore_ckp_fram(void)
{
	uint32_t cryptobuf_adr;
	uint32_t fram_ptr_init;
	uint32_t fram_ptr;			// initialize pointer to fram
	uint32_t IV_addr;
//	uint32_t tags[12];			// space for 3 tags


	// restore checkpoint information
	if (findNonce(&IV_addr) != 0) {
		return -1;
	}
	restoreNonce(pInitVectAES, &fram_ptr_init, IV_addr);
	fram_ptr = fram_ptr_init;

	// read tags
	if (CY15X102QN_Read(&hspi1, (uint8_t *)tags_after, 48, fram_ptr) != 0) {
		return -1;
	}
	fram_ptr += 48;

	// restore .cryptobuf
	cryptobuf_adr = fram_ptr + (CKP_EDATA - CKP_EDATA_CONF) + (CKP_EBSS - CKP_EBSS_CONF);
	if (CY15X102QN_Read(&hspi1, (uint8_t *)CKP_SCRYPTOBUF, CKP_ECRYPTOBUF - CKP_SCRYPTOBUF, cryptobuf_adr) != 0) {
		return -1;
	}

	// restore .data, generate tag
	if (CY15X102QN_Read(&hspi1, (uint8_t *)CKP_EDATA_CONF, CKP_EDATA - CKP_EDATA_CONF, fram_ptr) != 0) {
		return -1;
	}

	copy_mem(CKP_EDATA_CONF, (uint32_t)buf_data_after, CKP_EDATA - CKP_EDATA_CONF);
	copy_mem(CKP_SCRYPTOBUF, (uint32_t)crypto_data_after, CKP_ECRYPTOBUF_DATA - CKP_SCRYPTOBUF);


	if (crypto_AuthDecrypt((uint8_t*)CKP_EDATA_CONF, CKP_EDATA - CKP_EDATA_CONF, (uint8_t *)CKP_SCRYPTOBUF, CKP_EDATA_CONF - CKP_SDATA_CONF, (uint8_t *)CKP_SDATA_CONF, &tags_after[0]) != 0) {
		return -1;
	}
	pInitVectAES[2] += 1;
	fram_ptr += (CKP_EDATA - CKP_EDATA_CONF);

	// restore .bss, generate tag
	if (CY15X102QN_Read(&hspi1, (uint8_t *)CKP_EBSS_CONF, CKP_EBSS - CKP_EBSS_CONF, fram_ptr) != 0) {
		return -1;
	}
	if (crypto_AuthDecrypt((uint8_t*)CKP_EBSS_CONF, CKP_EBSS - CKP_EBSS_CONF, (uint8_t *)CKP_ECRYPTOBUF_DATA, CKP_EBSS_CONF - CKP_SBSS_CONF, (uint8_t *)CKP_SBSS_CONF, &tags_after[4]) != 0) {
		return -1;
	}
	pInitVectAES[2] += 1;
	fram_ptr += (CKP_EBSS - CKP_EBSS_CONF);

	// skip .cryptobuf
	fram_ptr += (CKP_ECRYPTOBUF - CKP_SCRYPTOBUF);


	// update journal information
	flash_addr = flash_addr + 16;
	fram_addr = fram_ptr_init;
	updateFramAddr(&fram_addr);


	//call function to make space for stack
	fram_stack_ptr = fram_ptr;
	restore_sp_fram(stack_len);

	return 0;
}


int32_t restore_ckp_fram_stack_drivers(void)
{
	if (CY15X102QN_Read(&hspi1, (uint8_t*)(CKP_ESTACK - stack_len + 1), stack_len, fram_stack_ptr) != 0) {
		return -1;
	}
	if (crypto_AuthDecrypt((uint8_t*)(CKP_ESTACK - stack_len + 1), stack_len, NULL, 0, NULL, &tags_after[8]) != 0) {
		return -1;
	}


//	 restore driver status
//	__disable_irq();
//	copy_mem(CKP_SDATA_CONF, CKP_SDATA, CKP_SDATA_CONF - CKP_SDATA);
//	copy_mem(CKP_SBSS_CONF, CKP_SBSS, CKP_SBSS_CONF - CKP_SBSS);
//	__enable_irq();

	return 0;
}


static int32_t saveNonce(uint32_t* IV, uint32_t fram_addr, uint32_t addr)
{
	uint32_t buf[4];

	buf[0] = *(IV);
	buf[1] = *(IV+1);
	buf[2] = *(IV+2);
	buf[3] = fram_addr;

	if (flash_start() != 0) {
		return -1;
	}
	if (flash_write_qw((uint32_t)buf, 16, addr, FLASH_TYPEPROGRAM_QUADWORD) != HAL_OK) {
		return -1;
	}
	if (flash_end() != 0) {
		return -1;
	}
	return 0;
}


static void restoreNonce(uint32_t* IV, uint32_t* fram_addr, uint32_t addr)
{
	*(IV) = *((uint32_t*)addr);
	*(IV+1) = *((uint32_t*)addr + 1);
	*(IV+2) = *((uint32_t*)addr + 2);
	*(IV+3) = 0x2;
	*fram_addr = *((uint32_t*)addr + 3);
}


static inline void updateFramAddr(volatile uint32_t* fram_addr)
{
	*fram_addr = (*fram_addr) == 0 ? CKP_FRAM_SIZE/2 : 0;
}


static int32_t generateIV(uint32_t* IV)
{
	if (HAL_RNG_GenerateRandomNumber(&hrng, IV) != HAL_OK) {
		return -1;
	}
	if (HAL_RNG_GenerateRandomNumber(&hrng, IV + 1) != HAL_OK) {
		return -1;
	}
	if (HAL_RNG_GenerateRandomNumber(&hrng, IV + 2) != HAL_OK) {
		return -1;
	}
	*(IV+3) = 0x2;
	return 0;
}


static int32_t findNonce(uint32_t* addr)
{
	uint32_t i, spage, upper, lower, pos;
	int found;

	// find first used page
	found = 0; i =0;
	while (i < NUM_PAGES && !found) {
		spage = CKP_FLASH_START + i*FLASH_PAGE_SIZE;
		if (*((uint32_t*)spage) != ~0x0UL) {
			found = 1;
		}
		i++;
	}

	// if no used page found, return error
	if (!found) {
		return -1;
	}

	// check if you just started next page
	if ((i < NUM_PAGES) && (*((uint32_t*)(spage + FLASH_PAGE_SIZE)) !=  ~0x0UL)) {
		spage = spage + FLASH_PAGE_SIZE;
		*addr = spage;
		return 0;
	}

	// find checkpoint in page
	lower = spage;
	upper = spage + FLASH_PAGE_SIZE;

	while (lower <= upper) {
		pos = (((upper - lower) / 2) & ~0x3) + lower;
		if (*((uint32_t*)pos) == ~0x0UL) {
			upper = pos;
		}
		else {
			if ((*((uint32_t*)pos) != ~0x0UL) && (*((uint32_t*)(pos+4)) == ~0x0UL)) {
				*addr = pos - 12;
				return 0;
			}
			else {
				lower = pos + 4;
			}
		}
	}
	return -1;
}


int32_t crypto_Init(void)
{
	if (RNG_Init() != 0) {
		return -1;
	}

	if (SAES_Shared_Init() != 0) {
		return -1;
	}

	if (HAL_CRYPEx_EncryptSharedKey(&hcryp, pKeyAES, Encryptedkey, 0, TIMEOUT_VALUE) != HAL_OK)	{
		return -1;
	}

	if (HAL_CRYPEx_DecryptSharedKey(&hcryp, Encryptedkey, 0, TIMEOUT_VALUE) != HAL_OK) {
		return -1;
	}

	if (GCM_AES_Shared_Init() != 0) {
		return -1;
	}
	return 0;
}


static int32_t crypto_AuthEncrypt(uint8_t* header, uint32_t header_size, uint8_t* buf_in, uint32_t buf_size, uint8_t* buf_out, uint32_t tag[])
{
	hcryp.Init.pInitVect = (uint32_t *)pInitVectAES;
	hcryp.Init.Header = (uint32_t *)header;
	hcryp.Init.HeaderSize = header_size;

	if (HAL_CRYP_Encrypt(&hcryp, (uint32_t *)buf_in, buf_size, (uint32_t*)buf_out, TIMEOUT_VALUE) != HAL_OK) {
		return -1;
	}

	if (HAL_CRYPEx_AESGCM_GenerateAuthTAG(&hcryp, (uint32_t*)tag, TIMEOUT_VALUE) != HAL_OK) {
		return -1;
	}

	return 0;
}


static int32_t crypto_AuthDecrypt(uint8_t* header, uint32_t header_size, uint8_t* buf_in, uint32_t buf_size, uint8_t* buf_out, uint32_t tag[])
{
	int32_t i;
	volatile uint32_t tag_check[4];

	for(i=0; i<4; i++) {
			tag_check[i] = 0;
		}

	hcryp.Init.pInitVect = (uint32_t *)pInitVectAES;
	hcryp.Init.Header = (uint32_t *)header;
	hcryp.Init.HeaderSize = header_size;

	if (HAL_CRYP_Decrypt(&hcryp, (uint32_t *)buf_in, buf_size, (uint32_t*)buf_out, TIMEOUT_VALUE) != HAL_OK) {
		return -1;
	}

	if (HAL_CRYPEx_AESGCM_GenerateAuthTAG(&hcryp, (uint32_t*)tag_check, TIMEOUT_VALUE) != HAL_OK) {
		return -1;
	}

	for(i=0; i<4; i++) {
		if (tag[i] != tag_check[i]){
			return -1;
		}
	}

	return 0;
}


// Optimization -> copy word insted of byte
void copy_mem(uint32_t addr_src, uint32_t addr_dest, uint32_t size)
{
	int idx = 0;
	while (idx < size)
	{
		*((uint8_t*)addr_dest + idx) = *((uint8_t*)addr_src + idx);
		idx++;
	}
}


static int32_t RNG_Init(void)
{
  hrng.Instance = RNG;
  hrng.Init.ClockErrorDetection = RNG_CED_ENABLE;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    return -1;
  }
  return 0;
}


static int32_t SAES_Shared_Init(void)
{
	hcryp.Instance = SAES;
	// CRYP_ConfigTypeDef
	hcryp.Init.DataType = CRYP_NO_SWAP;
	hcryp.Init.KeySize = CRYP_KEYSIZE_128B;
	//hcryp.Init.pKey = ?;
	hcryp.Init.pInitVect = (uint32_t *)pInitVectSAES;		// init vector for CBC
	hcryp.Init.Algorithm = CRYP_AES_CBC;
	// hcryp.Init.Header = ?;
	// hcryp.Init.B0 = ?;
	// hcryp.Init.HeaderSize = ?;
	// hcryp.Init.DataWidthUnit = ?
	// hcryp.Init.HeaderWidthUnit = ?
	hcryp.Init.KeyIVConfigSkip = CRYP_KEYIVCONFIG_ALWAYS;
	hcryp.Init.KeyMode = CRYP_KEYMODE_SHARED;
	hcryp.Init.KeySelect = CRYP_KEYSEL_HW;
	hcryp.Init.KeyProtection = CRYP_KEYPROT_DISABLE;
	if (HAL_CRYP_Init(&hcryp) != HAL_OK)
	{
		return -1;
	}
	return 0;
}


//configured with bytes
static int32_t GCM_AES_Shared_Init(void)
{
  hcryp.Instance = AES;
  // CRYP_ConfigTypeDef
  hcryp.Init.DataType = CRYP_DATATYPE_8B;					// 32 bit NOT SWAPPED (probably endianness, STM is LE)
  hcryp.Init.KeySize = CRYP_KEYSIZE_128B;					// 128 bit required by GCM */
  // hcryp_sh.Init.pKey = ?
  hcryp.Init.pInitVect = (uint32_t *)pInitVectAES;			// 128 bit, read as BE, not influenced by swap selection
  hcryp.Init.Algorithm = CRYP_AES_GCM_GMAC;
  //hcryp_sh.Init.Header = (uint32_t *)HeaderAES;				// Authenticated data (non encrypted)
  // hcryp_sh.Init.B0 = ?;										// ONLY for CCM
  //hcryp_sh.Init.HeaderSize = 14;								// header size in "DataWidthUnit"
  hcryp.Init.DataWidthUnit = CRYP_DATAWIDTHUNIT_BYTE;		// (word or byte)
  hcryp.Init.HeaderWidthUnit = CRYP_HEADERWIDTHUNIT_BYTE;	// (word or byte)
  hcryp.Init.KeyIVConfigSkip = CRYP_KEYIVCONFIG_ALWAYS;		// CRYP_KEYIVCONFIG_ONCE to process single message in multiple calls
  hcryp.Init.KeyMode = CRYP_KEYMODE_SHARED;					// NORMAL/SHARED/WRAPPED
  // hcryp_sh.Init.KeySelect = ?								// NORMAL/HW/SW/HSW (USER/HWkey/BOOTkey/HWxorBOOT), SAES only
  // hcryp_sh.Init.KeyProtection = ?;							// ENABLE/DISABLE, SAES only

  __HAL_RCC_AES_CLK_ENABLE();	// IMPORTANT: HAL_CRYPT_Init() won't do it (probably)
  	  	  	  	  	  	  	  	// Could also be placed in HAL_CRYP_MspInit(), in STM32U5xx_hal_msp.c (probably is better)
  if (HAL_CRYP_Init(&hcryp) != HAL_OK)
  {
    return -1;
  }
  return 0;
}
