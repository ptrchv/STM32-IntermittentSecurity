#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#include <stdint.h>

int32_t flash_start(void);
int32_t flash_end(void);
int32_t flash_erase(uint32_t FirstPage, uint32_t NbOfPages, uint32_t BankNumber, uint32_t EraseType);
int32_t flash_write_qw(uint32_t sbuf, uint32_t len, uint32_t addr, uint32_t WriteType);
void reset_hal_flash_data(void);

#endif /* INC_FLASH_H_ */
