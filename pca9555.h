/**
 * --------------------------------------------------------------------------
 * pca9555.h
 *
 *  Created on: Aug 16, 2020
 *      Author: Jack Lestrohan (c) Cobalt Audio - 2020
 * --------------------------------------------------------------------------
 */

#ifndef INC_PCA9555_H_
#define INC_PCA9555_H_

#include "i2c.h"

// Addresses (A0-A2)
#define PCA9555_ADD_20			0x20
#define PCA9555_ADD_21			0x21
#define PCA9555_ADD_22			0x22
#define PCA9555_ADD_23			0x23
#define PCA9555_ADD_24			0x24
#define PCA9555_ADD_25			0x25
#define PCA9555_ADD_26			0x26
#define PCA9555_ADD_27			0x27

/* Command Bytes */
#define PCA9555_CB_INPUTS_PORTS			0x00
#define PCA9555_CB_OUTPUTS_PORTS		0x02
#define PCA9555_CB_POL_INVERT_PORTS		0x04
#define PCA9555_CB_CONFIG_PORTS			0x06

typedef enum {
	PCA9555_POLARITY_NORMAL,
	PCA9555_POLARITY_INVERTED
} PCA9555_PinPolarity_t;

typedef enum {
	PCA9555_PIN_OUTPUT_MODE,
	PCA9555_PIN_INPUT_MODE
} PCA9555_PinMode_t;

typedef enum {
	PCA9555_BIT_RESET,
	PCA9555_BIT_SET
} PCA9555_BitValue_t;

/* pca9555 struct type for multi-instances handling */
typedef struct {
	I2C_HandleTypeDef*	hi2c;
	uint16_t			addr;
} PCA9555_HandleTypeDef;

HAL_StatusTypeDef pca9555_init(PCA9555_HandleTypeDef *hdev, I2C_HandleTypeDef *hi2c, uint16_t addr);
HAL_StatusTypeDef pca9555_DigitalWrite(PCA9555_HandleTypeDef *hdev, uint8_t pin, GPIO_PinState pinState);
HAL_StatusTypeDef pca9555_digitalRead(PCA9555_HandleTypeDef *hdev, uint8_t pin, uint8_t *data);
HAL_StatusTypeDef pca9555_pinMode(PCA9555_HandleTypeDef *hdev, uint8_t pin, PCA9555_PinMode_t mode, PCA9555_PinPolarity_t polarity);

#endif /* INC_PCA9555_H_ */
