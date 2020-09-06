/**
 * --------------------------------------------------------------------------
 * pca9555.c
 *
 *  Created on: Aug 16, 2020
 *      Author: Jack Lestrohan (c) Cobalt Audio - 2020
 * --------------------------------------------------------------------------
 */

#include "pca9555.h"

/* max delay given to all I2C stuff to initialize itself */
#define I2C_READYNESS_DELAY		500

#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

/**
 * Reads a given register
 * @param hdev
 * @param addr
 * @return
 */
static HAL_StatusTypeDef pca9555_readRegister(PCA9555_HandleTypeDef *hdev, uint8_t addr, uint16_t *data) {
	// read the current GPINTEN
	HAL_StatusTypeDef ret;
	uint8_t datahigh = 0;
	uint8_t datalow = 0;

	/* reading the low byte */
	ret = HAL_I2C_Mem_Read(hdev->hi2c, hdev->addr, addr, 1, &datahigh, 1, HAL_MAX_DELAY);
	if (ret != HAL_OK) return ret;

	/* reading the high byte */
	ret = HAL_I2C_Mem_Read(hdev->hi2c, hdev->addr, addr+1, 1, &datalow, 1, HAL_MAX_DELAY);
	if (ret != HAL_OK) return ret;

	/* builds the 16 bits value */
	*data = (datahigh << 8) | datalow;

	return ret;
}

/**
 * Writes a value to the given register
 * @param hdev MCP23017_HandleTypeDef struct to the aimed interface
 * @param regAddr Register Address
 * @param regValue Value to write to
 * @return
 */
static HAL_StatusTypeDef pca9555_writeRegister(PCA9555_HandleTypeDef *hdev, uint8_t regAddr, uint16_t regValue)
{
	HAL_StatusTypeDef ret;
	uint8_t lowb = lowByte(regValue);
	uint8_t highb = highByte(regValue);

	ret = HAL_I2C_Mem_Write(hdev->hi2c, hdev->addr, regAddr, 1, (uint8_t*) &lowb, 1, HAL_MAX_DELAY);
	if (ret != HAL_OK) return ret;

	ret = HAL_I2C_Mem_Write(hdev->hi2c, hdev->addr, regAddr+1, 1, (uint8_t*) &highb, 1, HAL_MAX_DELAY);
	if (ret != HAL_OK) return ret;

	return HAL_OK;
}

/**
 * Helper to update a single bit of an A/B register.
 * 		Reads the current register value
 * 		Writes the new register value
 * @param hdev
 * @param pin
 * @param pValue
 * @param portAaddr
 * @param portBaddr
 */
static HAL_StatusTypeDef pca9555_updateRegisterBit(PCA9555_HandleTypeDef *hdev, uint8_t pin, PCA9555_BitValue_t bitvalue, uint8_t commandByteAddr)
{
	uint16_t regValue;
	HAL_StatusTypeDef ret;

	ret = pca9555_readRegister(hdev, commandByteAddr, &regValue);
	if (ret != HAL_OK) return ret;

	// set the value for the particular bit
	bitWrite(regValue, pin, bitvalue);

	ret = pca9555_writeRegister(hdev, commandByteAddr, lowByte(regValue));
	if (ret != HAL_OK) return ret;

	ret = pca9555_writeRegister(hdev, commandByteAddr+1, highByte(regValue));
	if (ret != HAL_OK) return ret;

	return HAL_OK;
}

/**
 * main initialization routine
 * @param hdev
 * @param hi2c
 * @param addr
 * @return
 */
HAL_StatusTypeDef pca9555_init(PCA9555_HandleTypeDef *hdev, I2C_HandleTypeDef *hi2c, uint16_t addr)
{
	HAL_StatusTypeDef ret;

	hdev->hi2c = hi2c;
	hdev->addr = addr << 1;

	ret = HAL_I2C_IsDeviceReady(hi2c, hdev->addr, 20, I2C_READYNESS_DELAY);
	if (ret != HAL_OK) return ret;

	return HAL_OK;
}

/**
 * Writes to a given pin
 * @param hdev
 * @param pin
 * @param data
 * @return
 */
HAL_StatusTypeDef pca9555_DigitalWrite(PCA9555_HandleTypeDef *hdev, uint8_t pin, GPIO_PinState pinState)
{
	uint16_t data;
	HAL_StatusTypeDef ret;

	// read the current GPIO output latches
	//uint8_t regAddr = pca9555_commandByteForPin(pin, PCA9555_CB_OUTPUTS_PORTS);
	ret = pca9555_readRegister(hdev, PCA9555_CB_OUTPUTS_PORTS, &data);
	if (ret != HAL_OK) return ret;

	// set the pin and direction
	bitWrite(data, pin, pinState);

	// write the new GPIO
	//regAddr = pca9555_commandByteForPin(pin, PCA9555_CB_OUTPUTS_PORTS);
	ret = pca9555_writeRegister(hdev, PCA9555_CB_OUTPUTS_PORTS, data);
	if (ret != HAL_OK) return ret;

	return HAL_OK;
}

/**
 * Reads a given pin
 * @param hdev
 * @param pin
 * @return
 */
HAL_StatusTypeDef pca9555_digitalRead(PCA9555_HandleTypeDef *hdev, uint8_t pin, uint8_t *pinValue)
{
	uint16_t regValue;
	HAL_StatusTypeDef ret = pca9555_readRegister(hdev, PCA9555_CB_INPUTS_PORTS, &regValue);
	if (ret != HAL_OK) return ret;

	*pinValue = bitRead(regValue, pin);

	return HAL_OK;
}

/**
 *
 * @param hdev
 * @param pin
 * @param mode
 * @param polarity
 * @return
 */
HAL_StatusTypeDef pca9555_pinMode(PCA9555_HandleTypeDef *hdev, uint8_t pin, PCA9555_PinMode_t mode, PCA9555_PinPolarity_t polarity)
{
	HAL_StatusTypeDef ret;

	/* first we set the right polarity */
	ret = pca9555_updateRegisterBit(hdev, pin, (polarity == PCA9555_POLARITY_INVERTED), PCA9555_CB_POL_INVERT_PORTS);
	if (ret != HAL_OK) return ret;

	/* let's setup the pin direction now */
	ret = pca9555_updateRegisterBit(hdev, pin, (mode == PCA9555_PIN_INPUT_MODE), PCA9555_CB_CONFIG_PORTS);
	if (ret != HAL_OK) return ret;

	return HAL_OK;
}
