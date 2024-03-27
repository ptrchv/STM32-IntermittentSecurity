#include "fram.h"
#include "ckp_mem.h"


static uint8_t cmd DRIVER_BSS;
static uint8_t addr_be[3] DRIVER_BSS;
static uint8_t dummy DRIVER_DATA = 0;


/***** WRITE enable control **************************************************/
int32_t CY15X102QN_WriteEN(SPI_HandleTypeDef *hspi)
{
	cmd = CY15X102QN_WREN_CMD;

	HAL_GPIO_WritePin(GPIO_CS_Port, GPIO_CS_Pin, GPIO_PIN_RESET);

	if (HAL_SPI_Transmit(hspi, &cmd, 1, CY15X102QN_TIMEOUT) != HAL_OK) {
		return CY15X102QN_ERROR;
	}

	HAL_GPIO_WritePin(GPIO_CS_Port, GPIO_CS_Pin, GPIO_PIN_SET);

	return CY15X102QN_OK;
}

int32_t CY15X102QN_WriteDI(SPI_HandleTypeDef *hspi)
{
	cmd = CY15X102QN_WRDI_CMD;

	HAL_GPIO_WritePin(GPIO_CS_Port, GPIO_CS_Pin, GPIO_PIN_RESET);

	if (HAL_SPI_Transmit(hspi, &cmd, 1, CY15X102QN_TIMEOUT) != HAL_OK) {
		return CY15X102QN_ERROR;
	}

	HAL_GPIO_WritePin(GPIO_CS_Port, GPIO_CS_Pin, GPIO_PIN_SET);

	return CY15X102QN_OK;
}


/***** Register Commands *****************************************************/
int32_t CY15X102QN_ReadSR(SPI_HandleTypeDef *hspi, uint8_t *statusReg)
{
	cmd = CY15X102QN_RDSR_CMD;

	HAL_GPIO_WritePin(GPIO_CS_Port, GPIO_CS_Pin, GPIO_PIN_RESET);

	if (HAL_SPI_Transmit(hspi, &cmd, 1, CY15X102QN_TIMEOUT) != HAL_OK) {
		return CY15X102QN_ERROR;
	}

	if (HAL_SPI_Receive(hspi, statusReg, 1, CY15X102QN_TIMEOUT) != HAL_OK) {
		return CY15X102QN_ERROR;
	}

	HAL_GPIO_WritePin(GPIO_CS_Port, GPIO_CS_Pin, GPIO_PIN_SET);

	return CY15X102QN_OK;
}


int32_t CY15X102QN_WriteSR(SPI_HandleTypeDef *hspi, uint8_t statusReg)
{
	if (CY15X102QN_WriteEN(hspi) != CY15X102QN_OK)
	{
		return CY15X102QN_ERROR;
	}

	cmd = CY15X102QN_WRSR_CMD;

	HAL_GPIO_WritePin(GPIO_CS_Port, GPIO_CS_Pin, GPIO_PIN_RESET);

	if (HAL_SPI_Transmit(hspi, &cmd, 1, CY15X102QN_TIMEOUT) != HAL_OK) {
		return CY15X102QN_ERROR;
	}

	if (HAL_SPI_Transmit(hspi, (uint8_t*)&statusReg, 1, CY15X102QN_TIMEOUT) != HAL_OK) {
		return CY15X102QN_ERROR;
	}

	HAL_GPIO_WritePin(GPIO_CS_Port, GPIO_CS_Pin, GPIO_PIN_SET);

	return CY15X102QN_OK;

}


/***** READ/WRITE MEMORY *****************************************************/
int32_t CY15X102QN_Write(SPI_HandleTypeDef *hspi, uint8_t *buf, uint32_t size, uint32_t addr)
{
	if (CY15X102QN_WriteEN(hspi) != CY15X102QN_OK)
	{
		return CY15X102QN_ERROR;
	}

	cmd = CY15X102QN_WRITE_CMD;

	addr &= 0x3FFFF;
	addr_be[0] = *((uint8_t*)&addr + 2);
	addr_be[1] = *((uint8_t*)&addr + 1);
	addr_be[2] = *((uint8_t*)&addr + 0);


	HAL_GPIO_WritePin(GPIO_CS_Port, GPIO_CS_Pin, GPIO_PIN_RESET);

	if (HAL_SPI_Transmit(hspi, &cmd, 1, CY15X102QN_TIMEOUT) != HAL_OK) {
		return CY15X102QN_ERROR;
	}

	if (HAL_SPI_Transmit(hspi, (uint8_t*)addr_be, 3, CY15X102QN_TIMEOUT) != HAL_OK) {
		return CY15X102QN_ERROR;
	}

	if (HAL_SPI_Transmit(hspi, buf, size, CY15X102QN_TIMEOUT) != HAL_OK) {
		return CY15X102QN_ERROR;
	}

	HAL_GPIO_WritePin(GPIO_CS_Port, GPIO_CS_Pin, GPIO_PIN_SET);

	return CY15X102QN_OK;
}

int32_t CY15X102QN_Read(SPI_HandleTypeDef *hspi, uint8_t *buf, uint32_t size, uint32_t addr)
{

	cmd = CY15X102QN_READ_CMD;

	addr &= 0x3FFFF;
	addr_be[0] = *((uint8_t*)&addr + 2);
	addr_be[1] = *((uint8_t*)&addr + 1);
	addr_be[2] = *((uint8_t*)&addr + 0);

	HAL_GPIO_WritePin(GPIO_CS_Port, GPIO_CS_Pin, GPIO_PIN_RESET);

	if (HAL_SPI_Transmit(hspi, &cmd, 1, CY15X102QN_TIMEOUT) != HAL_OK) {
		return CY15X102QN_ERROR;
	}

	if (HAL_SPI_Transmit(hspi, (uint8_t*)&addr_be, 3, CY15X102QN_TIMEOUT) != HAL_OK) {
		return CY15X102QN_ERROR;
	}

	if (HAL_SPI_Receive(hspi, buf, size, CY15X102QN_TIMEOUT) != HAL_OK) {
		return CY15X102QN_ERROR;
	}

	HAL_GPIO_WritePin(GPIO_CS_Port, GPIO_CS_Pin, GPIO_PIN_SET);

	return CY15X102QN_OK;

}

int32_t CY15X102QN_FastRead(SPI_HandleTypeDef *hspi, uint8_t *buf, uint32_t size, uint32_t addr)
{
	cmd = CY15X102QN_FSTRD_CMD;

	addr &= 0x3FFFF;
	addr_be[0] = *((uint8_t*)&addr + 2);
	addr_be[1] = *((uint8_t*)&addr + 1);
	addr_be[2] = *((uint8_t*)&addr + 0);

	HAL_GPIO_WritePin(GPIO_CS_Port, GPIO_CS_Pin, GPIO_PIN_RESET);

	if (HAL_SPI_Transmit(hspi, &cmd, 1, CY15X102QN_TIMEOUT) != HAL_OK) {
		return CY15X102QN_ERROR;
	}

	if (HAL_SPI_Transmit(hspi, (uint8_t*)&addr_be, 3, CY15X102QN_TIMEOUT) != HAL_OK) {
		return CY15X102QN_ERROR;
	}

	if (HAL_SPI_Transmit(hspi, (uint8_t*)&dummy, 1, CY15X102QN_TIMEOUT) != HAL_OK) {
		return CY15X102QN_ERROR;
	}

	if (HAL_SPI_Receive(hspi, buf, size, CY15X102QN_TIMEOUT) != HAL_OK) {
		return CY15X102QN_ERROR;
	}

	HAL_GPIO_WritePin(GPIO_CS_Port, GPIO_CS_Pin, GPIO_PIN_SET);

	return CY15X102QN_OK;
}

/***** Special sector memory access ******************************************/
int32_t CY15X102QN_WriteSS(SPI_HandleTypeDef *hspi, uint8_t *buf, uint32_t size, uint8_t addr)
{
	if (CY15X102QN_WriteEN(hspi) != CY15X102QN_OK)
	{
		return CY15X102QN_ERROR;
	}

	cmd = CY15X102QN_SSWR_CMD;

	addr_be[0] = 0;
	addr_be[1] = 0;
	addr_be[2] = addr;

	HAL_GPIO_WritePin(GPIO_CS_Port, GPIO_CS_Pin, GPIO_PIN_RESET);

	if (HAL_SPI_Transmit(hspi, &cmd, 1, CY15X102QN_TIMEOUT) != HAL_OK) {
		return CY15X102QN_ERROR;
	}

	if (HAL_SPI_Transmit(hspi, (uint8_t*)addr_be, 3, CY15X102QN_TIMEOUT) != HAL_OK) {
		return CY15X102QN_ERROR;
	}

	if (HAL_SPI_Transmit(hspi, buf, size, CY15X102QN_TIMEOUT) != HAL_OK) {
		return CY15X102QN_ERROR;
	}

	HAL_GPIO_WritePin(GPIO_CS_Port, GPIO_CS_Pin, GPIO_PIN_SET);

	return CY15X102QN_OK;

}

int32_t CY15X102QN_ReadSS(SPI_HandleTypeDef *hspi, uint8_t *buf, uint32_t size, uint8_t addr)
{
	cmd = CY15X102QN_SSRD_CMD;

	addr_be[0] = 0;
	addr_be[1] = 0;
	addr_be[2] = addr;

	HAL_GPIO_WritePin(GPIO_CS_Port, GPIO_CS_Pin, GPIO_PIN_RESET);

	if (HAL_SPI_Transmit(hspi, &cmd, 1, CY15X102QN_TIMEOUT) != HAL_OK) {
		return CY15X102QN_ERROR;
	}

	if (HAL_SPI_Transmit(hspi, (uint8_t*)&addr_be, 3, CY15X102QN_TIMEOUT) != HAL_OK) {
		return CY15X102QN_ERROR;
	}

	if (HAL_SPI_Receive(hspi, buf, size, CY15X102QN_TIMEOUT) != HAL_OK) {
		return CY15X102QN_ERROR;
	}

	HAL_GPIO_WritePin(GPIO_CS_Port, GPIO_CS_Pin, GPIO_PIN_SET);

	return CY15X102QN_OK;
}

/***** Identification and serial number **************************************/
int32_t CY15X102QN_ReadDID(SPI_HandleTypeDef *hspi, uint8_t *deviceID)
{
	cmd = CY15X102QN_RDID_CMD;

	HAL_GPIO_WritePin(GPIO_CS_Port, GPIO_CS_Pin, GPIO_PIN_RESET);

	if (HAL_SPI_Transmit(hspi, &cmd, 1, CY15X102QN_TIMEOUT) != HAL_OK) {
		return CY15X102QN_ERROR;
	}

	if (HAL_SPI_Receive(hspi, deviceID, 9, CY15X102QN_TIMEOUT) != HAL_OK) {
		return CY15X102QN_ERROR;
	}

	HAL_GPIO_WritePin(GPIO_CS_Port, GPIO_CS_Pin, GPIO_PIN_SET);

	return CY15X102QN_OK;
}

int32_t CY15X102QN_ReadUID(SPI_HandleTypeDef *hspi, uint64_t* uniqueID)
{
	cmd = CY15X102QN_RUID_CMD;

	HAL_GPIO_WritePin(GPIO_CS_Port, GPIO_CS_Pin, GPIO_PIN_RESET);

	if (HAL_SPI_Transmit(hspi, &cmd, 1, CY15X102QN_TIMEOUT) != HAL_OK) {
		return CY15X102QN_ERROR;
	}

	if (HAL_SPI_Receive(hspi, (uint8_t*)uniqueID, 8, CY15X102QN_TIMEOUT) != HAL_OK) {
		return CY15X102QN_ERROR;
	}

	HAL_GPIO_WritePin(GPIO_CS_Port, GPIO_CS_Pin, GPIO_PIN_SET);

	return CY15X102QN_OK;
}

int32_t CY15X102QN_WriteSN(SPI_HandleTypeDef *hspi, uint64_t serialNum)
{
	if (CY15X102QN_WriteEN(hspi) != CY15X102QN_OK)
	{
		return CY15X102QN_ERROR;
	}

	cmd = CY15X102QN_WRSN_CMD;

	HAL_GPIO_WritePin(GPIO_CS_Port, GPIO_CS_Pin, GPIO_PIN_RESET);

	if (HAL_SPI_Transmit(hspi, &cmd, 1, CY15X102QN_TIMEOUT) != HAL_OK) {
		return CY15X102QN_ERROR;
	}

	if (HAL_SPI_Transmit(hspi, (uint8_t*)&serialNum, 8, CY15X102QN_TIMEOUT) != HAL_OK) {
		return CY15X102QN_ERROR;
	}

	HAL_GPIO_WritePin(GPIO_CS_Port, GPIO_CS_Pin, GPIO_PIN_SET);

	return CY15X102QN_OK;
}

int32_t CY15X102QN_ReadDSN(SPI_HandleTypeDef *hspi, uint64_t* serialNum)
{
	cmd = CY15X102QN_RDSN_CMD;

	HAL_GPIO_WritePin(GPIO_CS_Port, GPIO_CS_Pin, GPIO_PIN_RESET);

	if (HAL_SPI_Transmit(hspi, &cmd, 1, CY15X102QN_TIMEOUT) != HAL_OK) {
		return CY15X102QN_ERROR;
	}

	if (HAL_SPI_Receive(hspi, (uint8_t*)serialNum, 8, CY15X102QN_TIMEOUT) != HAL_OK) {
		return CY15X102QN_ERROR;
	}

	HAL_GPIO_WritePin(GPIO_CS_Port, GPIO_CS_Pin, GPIO_PIN_SET);

	return CY15X102QN_OK;
}






