# X-NUCLEO-EEPRMA2 test (Expansion board)

Brief description of the six EEPROMs mounted on the board:
|**#**|**Memory Array Size**|**Max Speed**|**Package**|**Page Size**|**SPI**|**I2C**|
|:---:|:---:               |:---:              |   :---:   | :---:       | :---:| :---:|
|**U1**|2 Kb|400 KHz|UFDFPN8|16 bytes| |🟦|
|**U2**|256 Kb|1 MHz|TSSOP8|64 bytes| |🟦|
|**U3**|1 Mb|1 MHz|SO8 |256 bytes| |🟦|
|**U5**|4 K|5 MHz| UFDFPN8|16 bytes|🟦||
|**U6**| 256 Kb| 20 MHz|TSSOP8|64 bytes|🟦||
|**U7**| 4 Mb | 10MHz| SO8|512 bytes|🟦||


# Tested Hardware:
|**#**|**Microcontroller**|**EEPROM**|
|:---:|:---:              |:---:     |
|**1**|NUCLEO-**L5**52ZE-Q|U1, U2, U3|
|**2**|NUCLEO-**H7**45ZI-Q|U1, U2, U3|
|**3**|NUCLEO-**U5**75ZI-Q|U1, U2, U3|

Unit test file: [drv_eeprom.c.ut](https://github.com/energicamotor/stm32-drv-eeprom/blob/main/.tests/drv_eeprom.c.ut)

