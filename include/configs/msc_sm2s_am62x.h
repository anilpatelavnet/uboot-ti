/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Configuration header file for K3 AM625 SoC family
 *
 * Copyright (C) 2020-2022 Texas Instruments Incorporated - https://www.ti.com/
 *	Suman Anna <s-anna@ti.com>
 */

#ifndef __CONFIG_MSC_SM2S_AM62X_H
#define __CONFIG_MSC_SM2S_AM62X_H

#include <config_distro_bootcmd.h>

/* DDR Configuration */
#define CFG_SYS_SDRAM_BASE1             0x880000000

/* Now for the remaining common defines */
#include <configs/ti_armv7_common.h>


#define CONFIG_SYS_I2C_SPEED            100000
#define I2C_GP          0
#define I2C_PM          1
#define I2C_DEV         2
#define I2C_LCD         3
#define I2C_CAM         4
#define BI_EEPROM_I2C_ADDR      0x50
#define PMIC_I2C_ADDR           0x30

/* NAND Driver config */
#define CFG_SYS_NAND_BASE            0x51000000

#define CFG_SYS_NAND_ECCPOS		{ 2, 3, 4, 5, 6, 7, 8, 9, \
					 10, 11, 12, 13, 14, 15, 16, 17, \
					 18, 19, 20, 21, 22, 23, 24, 25, \
					 26, 27, 28, 29, 30, 31, 32, 33, \
					 34, 35, 36, 37, 38, 39, 40, 41, \
					 42, 43, 44, 45, 46, 47, 48, 49, \
					 50, 51, 52, 53, 54, 55, 56, 57, }

#define CFG_SYS_NAND_ECCSIZE         512

#define CFG_SYS_NAND_ECCBYTES        14
/*-- end NAND config --*/

#define BOOTDEV_SD \
	"boot_sd=" \
		"setenv bootpart 1:2;" \
		"setenv mmcdev 1;" \
		"run envboot; run distro_bootcmd;" \
		"\0"


#define BOOTDEV_EMMC \
	"boot_emmc=" \
		"setenv bootpart 0:2;" \
		"setenv mmcdev 0;" \
		"run envboot; run distro_bootcmd;" \
		"\0"


#endif /* __CONFIG_MSC_SM2S_AM62X_H */
