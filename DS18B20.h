/*
 * DS18B20.h
 *
 *  Created on: Dec 7, 2021
 *      Author: Lucas
 */

#ifndef INC_DS18B20_H_
#define INC_DS18B20_H_


#include "main.h"

typedef struct
{
	float Temperature;
}DS18B20_DataTypedef;

void DS18B20_GetData(DS18B20_DataTypedef *DS18B20_Data, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, TIM_HandleTypeDef htim);

#endif /* INC_DS18B20_H_ */
