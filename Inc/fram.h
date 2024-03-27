#ifndef INC_FRAM_H_
#define INC_FRAM_H_

#include "stm32u5xx_hal.h"


#define GPIO_CS_Pin 									GPIO_PIN_12
#define GPIO_CS_Port 									GPIOE


#define CY15X102QN_TIMEOUT								100


// Error codes
#define CY15X102QN_OK                           		(0)
#define CY15X102QN_ERROR                        		(-1)


/***** WRITE enable control **************************************************/
#define CY15X102QN_WREN_CMD						 		0x06U   /* Set write enable latch */
#define CY15X102QN_WRDI_CMD						 		0x04U	/* Reset write enable latch */

/***** Register Commands *****************************************************/
#define CY15X102QN_RDSR_CMD						 		0x05U	/* Read status register */
#define CY15X102QN_WRSR_CMD						 		0x01U	/* Write status register */

/***** READ/WRITE MEMORY *****************************************************/
#define CY15X102QN_WRITE_CMD		                    0x02U   /* Write memory data */
#define CY15X102QN_READ_CMD								0x03U   /* Read memory data (max 40 MHZ)*/
#define CY15X102QN_FSTRD_CMD							0x0BU   /* Fast read memory data */

/***** Special sector memory access ******************************************/
#define CY15X102QN_SSWR_CMD		                    	0x42U   /* Special sector write */
#define CY15X102QN_SSRD_CMD								0x4BU   /* Special sector read */

/***** Identification and serial number **************************************/
#define CY15X102QN_RDID_CMD		                    	0x9FU   /* Read device ID */
#define CY15X102QN_RUID_CMD								0x4CU   /* Read unique ID */
#define CY15X102QN_WRSN_CMD		                    	0xC2U   /* Write serial number */
#define CY15X102QN_RDSN_CMD								0xC3U   /* Read serial number */

/***** Low power mode commands ***********************************************/
#define CY15X102QN_DPD_CMD		                    	0xBAU   /* Special sector write */
#define CY15X102QN_HBN_CMD								0xB9U   /* Special sector read */



/***** WRITE enable control **************************************************/
int32_t CY15X102QN_WriteEN(SPI_HandleTypeDef *hspi);
int32_t CY15X102QN_WriteDI(SPI_HandleTypeDef *hspi);

/***** Register Commands *****************************************************/
int32_t CY15X102QN_ReadSR(SPI_HandleTypeDef *hspi, uint8_t *statusReg);
int32_t CY15X102QN_WriteSR(SPI_HandleTypeDef *hspi, uint8_t statusReg);

/***** READ/WRITE MEMORY *****************************************************/
int32_t CY15X102QN_Write(SPI_HandleTypeDef *hspi, uint8_t *buf, uint32_t size, uint32_t addr);
int32_t CY15X102QN_Read(SPI_HandleTypeDef *hspi, uint8_t *buf, uint32_t size, uint32_t addr);
int32_t CY15X102QN_FastRead(SPI_HandleTypeDef *hspi, uint8_t *buf, uint32_t size, uint32_t addr);

/***** Special sector memory access ******************************************/
int32_t CY15X102QN_WriteSS(SPI_HandleTypeDef *hspi, uint8_t *buf, uint32_t size, uint8_t addr);
int32_t CY15X102QN_ReadSS(SPI_HandleTypeDef *hspi, uint8_t *buf, uint32_t size, uint8_t addr);

/***** Identification and serial number **************************************/
int32_t CY15X102QN_ReadDID(SPI_HandleTypeDef *hspi, uint8_t *deviceID);
int32_t CY15X102QN_ReadUID(SPI_HandleTypeDef *hspi, uint64_t* uniqueID);
int32_t CY15X102QN_WriteSN(SPI_HandleTypeDef *hspi, uint64_t serialNum);
int32_t CY15X102QN_ReadDSN(SPI_HandleTypeDef *hspi, uint64_t* serialNum);

/***** Low power mode commands ***********************************************/
// JUST PROTOTOTYPES BECAUSE YOU NEED TO HANDLE THE TIMINGS

//uint32_t CY15X102QN_EnterDPD(SPI_HandleTypeDef *hspi);
//uint32_t CY15X102QN_ExitDPD(SPI_HandleTypeDef *hspi);
//uint32_t CY15X102QN_EnterHBN(SPI_HandleTypeDef *hspi);
//uint32_t CY15X102QN_ExitHBN(SPI_HandleTypeDef *hspi);



#endif /* INC_FRAM_H_ */
