#ifndef INC_FRAM_CKP_H_
#define INC_FRAM_CKP_H_

#include <stdint.h>

int32_t write_ckp_fram(uint32_t stack_ptr);
int32_t restore_ckp_fram(void);
int32_t restore_sp_fram(uint32_t stack_len);

int32_t crypto_Init();

#endif /* INC_FRAM_CKP_H_ */
