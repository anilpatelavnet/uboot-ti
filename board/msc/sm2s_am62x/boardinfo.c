/*
 * Copyright (C) 2023 Avnet Embedded
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <common.h>
#include <malloc.h>
#include <errno.h>
#include <dm.h>
#include <i2c.h>
#include <i2c_eeprom.h>
#include "../common/boardinfo.h"

int read_boardinfo(int offset, uint8_t *buffer, int size)
{
	struct udevice *eeprom;
	int ret;

	ret = i2c_get_chip_for_busnum(I2C_GP, BI_EEPROM_I2C_ADDR, 2, &eeprom);
	if (ret < 0) {
		puts("ID eeprom not found\n");
		return ret;
	}

	return i2c_eeprom_read(eeprom, offset, buffer, size);
}

int write_boardinfo(int offset, uint8_t *buffer, int size)
{
	struct udevice *eeprom;
	int ret;

	ret = i2c_get_chip_for_busnum(I2C_GP, BI_EEPROM_I2C_ADDR, 2, &eeprom);
	if (ret < 0) {
		puts("ID eeprom not found\n");
		return ret;
	}

	return i2c_eeprom_write(eeprom, offset, buffer, size);
}

const char* bi_get_form_factor(const board_info_t *bi)
{
	return "sm2s";
}

const char* bi_get_platform(const board_info_t *bi)
{
	return "am62x";
}

const char* bi_get_processor(const board_info_t *bi)
{
	return "qc";
}
