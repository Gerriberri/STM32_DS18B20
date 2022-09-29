/*
 * DS18B20.c
 *
 *  Created on: Dec 7, 2021
 *      Author: Lucas
 */

#include "DS18B20.h"

void DS18B20_delay_us(uint16_t us, TIM_HandleTypeDef htim)
{
	__HAL_TIM_SET_COUNTER(&htim,0);  // set the counter value a 0
  	while (__HAL_TIM_GET_COUNTER(&htim) < us);  // wait for the counter to reach the us input in the parameter
}

void DS18B20_Set_Pin_Output(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
  	GPIO_InitStruct.Pin = GPIO_Pin;
  	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void DS18B20_Set_Pin_Input(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
  	GPIO_InitStruct.Pin = GPIO_Pin;
  	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  	GPIO_InitStruct.Pull = GPIO_NOPULL;
  	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

uint8_t DS18B20_Start(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, TIM_HandleTypeDef htim)
{
	uint8_t Response = 0;
	DS18B20_Set_Pin_Output(GPIOx, GPIO_Pin);   // set the pin as output
	HAL_GPIO_WritePin (GPIOx, GPIO_Pin, 0);  // pull the pin low
	DS18B20_delay_us(480, htim);   // delay according to datasheet

	DS18B20_Set_Pin_Input(GPIOx, GPIO_Pin);    // set the pin as input
	DS18B20_delay_us(80, htim);    // delay according to datasheet

	if (!(HAL_GPIO_ReadPin(GPIOx, GPIO_Pin))) Response = 1;    // if the pin is low i.e the presence pulse is detected
	else Response = -1;

	DS18B20_delay_us(400, htim); // 480 us delay totally.

	return Response;
}

void DS18B20_Write(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, TIM_HandleTypeDef htim ,uint8_t Data)
{
	DS18B20_Set_Pin_Output(GPIOx, GPIO_Pin);  // set as output

	for (int i=0; i<8; i++)
	{

		if ((Data & (1<<i))!=0)  // if the bit is high
		{
			// write 1

			DS18B20_Set_Pin_Output(GPIOx, GPIO_Pin);  // set as output
			HAL_GPIO_WritePin(GPIOx, GPIO_Pin, 0);  // pull the pin LOW
			DS18B20_delay_us(1, htim);  // wait for 1 us

			DS18B20_Set_Pin_Input(GPIOx, GPIO_Pin);  // set as input
			DS18B20_delay_us(50, htim);  // wait for 60 us
		}

		else  // if the bit is low
		{
			// write 0

			DS18B20_Set_Pin_Output(GPIOx, GPIO_Pin);
			HAL_GPIO_WritePin (GPIOx, GPIO_Pin, 0);  // pull the pin LOW
			DS18B20_delay_us(50, htim);  // wait for 60 us

			DS18B20_Set_Pin_Input(GPIOx, GPIO_Pin);
		}
	}
}

uint8_t DS18B20_Read(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, TIM_HandleTypeDef htim)
{
	uint8_t value=0;

	DS18B20_Set_Pin_Input(GPIOx, GPIO_Pin);

	for (int i=0;i<8;i++)
	{
		DS18B20_Set_Pin_Output(GPIOx, GPIO_Pin);   // set as output

		HAL_GPIO_WritePin(GPIOx, GPIO_Pin, 0);  // pull the data pin LOW
		DS18B20_delay_us(1, htim);  // wait for > 1us

		DS18B20_Set_Pin_Input(GPIOx, GPIO_Pin);  // set as input
		if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin))  // if the pin is HIGH
		{
			value |= 1<<i;  // read = 1
		}
		DS18B20_delay_us(50, htim);  // wait for 60 us
	}
	return value;
}

void DS18B20_GetData(DS18B20_DataTypedef *DS18B20_Data, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, TIM_HandleTypeDef htim)
{
	uint8_t Presence = 0;
	uint8_t Temp_byte1, Temp_byte2;
	uint16_t TEMP;

	Presence = DS18B20_Start(GPIOx, GPIO_Pin, htim);
	HAL_Delay (1);
	DS18B20_Write(GPIOx, GPIO_Pin, htim, 0xCC);  // skip ROM
	DS18B20_Write(GPIOx, GPIO_Pin, htim, 0x44);  // convert T
	HAL_Delay(800);

	Presence = DS18B20_Start(GPIOx, GPIO_Pin, htim);
	HAL_Delay(1);
	DS18B20_Write(GPIOx, GPIO_Pin, htim, 0xCC);  // skip ROM
	DS18B20_Write(GPIOx, GPIO_Pin, htim, 0xBE);  // Read Scratch-pad

	Temp_byte1 = DS18B20_Read(GPIOx, GPIO_Pin, htim);
	Temp_byte2 = DS18B20_Read(GPIOx, GPIO_Pin, htim);
	TEMP = (Temp_byte2<<8)|Temp_byte1;
	DS18B20_Data->Temperature = (float)TEMP/16;
}


