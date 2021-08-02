/*!
	@file   drv_eeprom.c
	@brief  <brief description here>
	@t.odo	-
	---------------------------------------------------------------------------

	MIT License
	Copyright (c) 2021 Ioannis Deligiannis, Federico Carnevale, Daniele Russo

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:
	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
 */
/******************************************************************************
 * Preprocessor Definitions & Macros
 ******************************************************************************/

/******************************************************************************
 * Includes
 ******************************************************************************/

#include "drv_eeprom.h"

#ifdef DRV_EEPROM_ENABLED
/******************************************************************************
 * Enumerations, structures & Variables
 ******************************************************************************/

static eeprom_t* eeprom_interfaces[4] = {NULL};
static uint32_t eeprom_interfaces_cnt = 0;
static uint8_t i2c_data[512];

/******************************************************************************
 * Declaration | Static Functions
 ******************************************************************************/

/******************************************************************************
 * Definition  | Static Functions
 ******************************************************************************/

/******************************************************************************
 * Definition  | Public Functions
 ******************************************************************************/
#pragma GCC push_options
#pragma GCC optimize ("O0")
i_status eeprom_initialize(eeprom_t* instance)
{
	if(instance == NULL)
		return I_ERROR;

	if(instance->mx_init != NULL)
		instance->mx_init();
	instance->init_sts = 1;
	instance->is_busy = 0;
	instance->has_error = 0;

	if(!(instance->mem_addr_sz == 1 || instance->mem_addr_sz == 2))
		instance->mem_addr_sz = 2;
	
	for(register int i=0;i<eeprom_interfaces_cnt;i++)
		if(eeprom_interfaces[i] == instance)
			return I_OK;

	eeprom_interfaces[eeprom_interfaces_cnt] = instance;
	eeprom_interfaces_cnt++;
	return I_OK;
}

i_status eeprom_deinitialize(eeprom_t* instance)
{
	if(instance == NULL)
		return I_ERROR;

	HAL_StatusTypeDef temp = HAL_I2C_DeInit(instance->handler);
	instance->init_sts=0;
	instance->is_busy=0;
	instance->has_error = 0;
	return temp == HAL_OK ? I_OK : I_ERROR;
}

i_status eeprom_read(eeprom_t* instance, uint16_t address, uint8_t * buffer, uint32_t size)
{
	if(instance == NULL)
		return I_ERROR;

	if(instance->is_busy != 0)
		return I_INPROGRESS;

	if(buffer == NULL || size == 0)
		return I_INVALID;

	if(HAL_I2C_IsDeviceReady(instance->handler,instance->dev_addr,0xFF,1) != HAL_OK)
		return I_ERROR;

	instance->is_busy = 1;
	instance->has_error = 0;

#ifdef DRV_EEPROM_USE_INTERRUPT_MODE
	HAL_StatusTypeDef temp =   HAL_I2C_Mem_Read_IT(instance->handler, instance->dev_addr, address, instance->mem_addr_sz, buffer, size);
#else

	HAL_StatusTypeDef temp = HAL_ERROR;

	if(instance->pg_sz == 0)
	{
		temp =  HAL_I2C_Mem_Read(instance->handler, instance->dev_addr, address, instance->mem_addr_sz, buffer, size, 256);
	}
	else
	{
		uint32_t b_sz = size;
		int b_pos = 0;
		int b_i_sz = 0;

		while (b_pos != b_sz)
		{
			do
			{
				b_i_sz++;

			} while ((address + b_pos + b_i_sz) % (instance ->pg_sz) != 0 && (b_i_sz + b_pos) < b_sz);

			if(HAL_I2C_IsDeviceReady(instance->handler,instance->dev_addr,0xFF,10) != HAL_OK)
				return I_ERROR;
			HAL_Delay(1);
			temp = HAL_I2C_Mem_Read(instance->handler, instance->dev_addr, address+b_pos, instance->mem_addr_sz, &buffer[b_pos], b_i_sz, 256);
			b_pos += b_i_sz;
			b_i_sz = 0;
		}
	}

	instance->is_busy = 0;
#endif

	return temp == HAL_OK ? I_OK : temp == HAL_ERROR ? I_ERROR :I_DEBUG_01;
}

i_status eeprom_wait_sync(eeprom_t* instance,uint8_t timeout)
{
	if(instance->is_busy == 0)
	{
		if(HAL_I2C_IsDeviceReady(instance->handler,instance->dev_addr,0xFF,1)== HAL_OK)
			return I_OK;
		return I_ERROR;
	}
	uint8_t time = timeout;

	while(eeprom_available(instance)!= I_OK)
	{

		HAL_Delay(1);
		time--;
		if(time == 0)
			return I_ERROR;
	}
	return I_OK;
}

i_status eeprom_write(eeprom_t* instance, uint16_t address, uint8_t * buffer, uint32_t size)
{
	if(instance == NULL)
		return I_ERROR;

	if(instance->init_sts == 0)
		return I_ERROR;

	if(instance->is_busy != 0)
		return I_INPROGRESS;

	if(buffer == NULL || size == 0)
		return I_INVALID;

	if(HAL_I2C_IsDeviceReady(instance->handler,instance->dev_addr,0xFF,10) != HAL_OK)
		return I_ERROR;

	instance->is_busy = 1;
	instance->has_error = 0;

#ifdef DRV_EEPROM_USE_INTERRUPT_MODE
	HAL_StatusTypeDef temp =  HAL_I2C_Mem_Write_IT(instance->handler, instance->dev_addr, address, instance->mem_addr_sz, buffer, size);
#else
	HAL_StatusTypeDef temp = HAL_ERROR;

	if(instance->pg_sz == 0)
	{
		temp =  HAL_I2C_Mem_Write(instance->handler, instance->dev_addr, address, instance->mem_addr_sz, buffer, size, 25);
	}
	else
	{
		uint32_t b_sz = size;
		int b_pos = 0;
		int b_i_sz = 0;
		while (b_pos != b_sz)
		{
			do
			{
				b_i_sz++;
			} while ((address + b_pos + b_i_sz) % (instance->pg_sz) != 0 && (b_i_sz + b_pos) < b_sz);


			if(HAL_I2C_IsDeviceReady(instance->handler,instance->dev_addr,0xFF,1) != HAL_OK)
				return I_ERROR;

			temp = HAL_I2C_Mem_Write(instance->handler, instance->dev_addr, address+b_pos, instance->mem_addr_sz,&buffer[b_pos], b_i_sz, 100);
			HAL_Delay(1);
			if(temp != HAL_OK)
			{
				if(HAL_I2C_IsDeviceReady(instance->handler,instance->dev_addr,0xFF,1) != HAL_OK)
					return I_ERROR;
				temp = HAL_I2C_Mem_Write(instance->handler, instance->dev_addr, address+b_pos,instance->mem_addr_sz,&buffer[b_pos], b_i_sz, 100);
				if(temp != HAL_OK)
					break;
			}
			
			if(temp != HAL_OK)
				break;
			b_pos += b_i_sz;
			b_i_sz = 0;
		}
	}

	instance->is_busy = 0;
#endif
	return temp == HAL_OK ? I_OK : temp == HAL_ERROR ? I_ERROR :I_DEBUG_01;
}

i_status eeprom_erase(eeprom_t* instance)
{
	memset(i2c_data,0x00,512);

	if(instance == NULL)
		return I_ERROR;

	if(instance->init_sts == 0)
		return I_ERROR;

	if(instance->is_busy != 0)
		return I_ERROR;

	if(HAL_I2C_IsDeviceReady(instance->handler,instance->dev_addr,0xFF,1) != HAL_OK)
		return I_ERROR;

	uint32_t mem_address = 0;
	uint32_t eraseable_sz = instance->pg_sz == 0 ? 8 : (instance->pg_sz <=512 ? instance->pg_sz : 8);

	while(mem_address != instance->mem_sz)
	{
		instance->has_error = 0;

		if(eeprom_write(instance, mem_address, i2c_data, eraseable_sz) !=I_OK)
		{
			return I_ERROR;
		}
		mem_address = mem_address + eraseable_sz;

#ifdef DRV_EEPROM_USE_INTERRUPT_MODE
		uint32_t timeout = 15;
		while(eeprom_available(instance)!= I_OK)
		{
			HAL_Delay(1);
			timeout--;
			if(timeout == 0)
				return I_ERROR;
		}
#endif
	}

	return I_OK;
}

i_status eeprom_available(eeprom_t* instance)
{
	return instance->is_busy == 0 ? I_OK : I_INPROGRESS;
}
#pragma GCC pop_options
/******************************************************************************
 * Definition  | Callback Functions
 ******************************************************************************/

#ifdef DRV_EEPROM_USE_INTERRUPT_MODE

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	static eeprom_t* current_eeprom = NULL;

	for(register int i=0;i<eeprom_interfaces_cnt;i++)
		if(eeprom_interfaces[i]->is_busy == 1)
			current_eeprom = eeprom_interfaces[i];

	if(current_eeprom == NULL)
		return;


	current_eeprom->is_busy = 0;
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	static eeprom_t* current_eeprom = NULL;

	for(register int i=0;i<eeprom_interfaces_cnt;i++)
		if(eeprom_interfaces[i]->is_busy == 1)
			current_eeprom = eeprom_interfaces[i];

	if(current_eeprom == NULL)
		return;


	current_eeprom->is_busy = 0;
}


void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
	static eeprom_t* current_eeprom = NULL;

	for(register int i=0;i<eeprom_interfaces_cnt;i++)
		if(eeprom_interfaces[i]->is_busy == 1)
			current_eeprom = eeprom_interfaces[i];

	if(current_eeprom == NULL)
		return;

	current_eeprom->has_error = 1;
	current_eeprom->is_busy = 0;
}

void HAL_I2C_AbortCpltCallback(I2C_HandleTypeDef *hi2c)
{
	static eeprom_t* current_eeprom = NULL;

	for(register int i=0;i<eeprom_interfaces_cnt;i++)
		if(eeprom_interfaces[i]->is_busy == 1)
			current_eeprom = eeprom_interfaces[i];

	if(current_eeprom == NULL)
		return;

	current_eeprom->has_error = 1;
	current_eeprom->is_busy = 0;
}

#endif

/******************************************************************************
 * EOF - NO CODE AFTER THIS LINE
 ******************************************************************************/
#endif
