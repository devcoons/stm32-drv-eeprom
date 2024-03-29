/*!
	@file   drv_eeprom.h
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

#ifndef DRIVERS_INC_DRV_EEPROM_H_
	#define DRIVERS_INC_DRV_EEPROM_H_

/******************************************************************************
* Includes
******************************************************************************/

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#if __has_include("i2c.h")
	#include "i2c.h"
	#define DRV_EEPROM_ENABLED
#endif

#if __has_include("../drv_eeprom_config.h")
	#include "../drv_eeprom_config.h"
#else
	#warning "Missing drv_eeprom_config.h. The library is disabled"
	#undef DRV_EEPROM_ENABLED
#endif

#ifdef DRV_EEPROM_ENABLED
/******************************************************************************
* Enumerations, structures & Variables
******************************************************************************/

#if !defined(ENUM_I_STATUS)
#define ENUM_I_STATUS
typedef enum
{
	I_OK 			= 0x00,
	I_INVALID 		= 0x01,
	I_EXISTS 		= 0x02,
	I_NOTEXISTS 		= 0x03,
	I_FAILED 		= 0x04,
	I_EXPIRED 		= 0x05,
	I_UNKNOWN 		= 0x06,
	I_INPROGRESS 		= 0x07,
	I_IDLE			= 0x08,
	I_FULL			= 0x09,
	I_EMPTY			= 0x0A,
	I_YES			= 0x0B,
	I_NO			= 0x0C,
	I_SKIP			= 0x0D,
	I_LOCKED 		= 0x0E,
	I_INACTIVE 		= 0x0F,
	I_ACTIVE 		= 0x10,
	I_READY		 	= 0x11,
	I_WAIT 			= 0x12,
	I_OVERFLOW 		= 0x13,
	I_CONTINUE 		= 0x14,
	I_STOPPED 		= 0x15,
	I_WARNING 		= 0x16,
	I_SLEEP 		= 0x17,
	I_DEEPSLEEP 		= 0x18,
	I_STANDBY 		= 0x19,
	I_GRANTED 		= 0x1A,
	I_DENIED 		= 0x1B,
	I_DEBUG_01 		= 0xE0,
	I_DEBUG_02 		= 0xE1,
	I_DEBUG_03 		= 0xE2,
	I_DEBUG_04 		= 0xE3,
	I_DEBUG_05 		= 0xE4,
	I_DEBUG_06 		= 0xE5,
	I_DEBUG_07 		= 0xE6,
	I_DEBUG_08 		= 0xE7,
	I_DEBUG_09 		= 0xE8,
	I_DEBUG_10 		= 0xE9,
	I_DEBUG_11 		= 0xEA,
	I_DEBUG_12 		= 0xEB,
	I_DEBUG_13 		= 0xEC,
	I_DEBUG_14 		= 0xED,
	I_DEBUG_15 		= 0xEE,
	I_DEBUG_16 		= 0xEF,
	I_MEMALIGNED		= 0xFC,
	I_MEMUNALIGNED		= 0xFD,
	I_NOTIMPLEMENTED 	= 0xFE,
	I_ERROR 		= 0xFF
}i_status;
#endif

typedef struct
{
	uint8_t init_sts;
	volatile uint8_t is_busy;
	volatile uint8_t has_error;
	uint8_t  mem_addr_sz;
	uint16_t dev_addr;		// Address of the eeprom
	uint16_t pg_sz;
	uint32_t mem_sz;
	I2C_HandleTypeDef * handler;
	void(*mx_init)();
}eeprom_t;

/******************************************************************************
* Declaration | Public Functions
******************************************************************************/

i_status eeprom_initialize(eeprom_t* instance);
i_status eeprom_deinitialize(eeprom_t* instance);

i_status eeprom_read(eeprom_t* instance, uint16_t address, uint8_t * buffer, uint32_t size);
i_status eeprom_write(eeprom_t* instance, uint16_t address, uint8_t * buffer, uint32_t size);
i_status eeprom_erase(eeprom_t* instance);

i_status eeprom_available(eeprom_t* instance);
i_status eeprom_wait_sync(eeprom_t* instance, uint8_t timeout);

/******************************************************************************
* EOF - NO CODE AFTER THIS LINE
******************************************************************************/
#endif
#endif
