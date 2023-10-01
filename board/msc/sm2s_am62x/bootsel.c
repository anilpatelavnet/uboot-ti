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
#include <command.h>
#include <asm/io.h>
#include <errno.h>
#include <asm/io.h>
#include <asm/arch/sys_proto.h>
#include <asm-generic/gpio.h>
#include <asm/arch/hardware.h>
#include <dm/uclass.h>

struct boot_dev_descriptor {
	const char* name;
	const char* full_name;
};

static struct boot_dev_descriptor boot_dev[] = {
	[0] = { .name = "",     .full_name = "" },
	[1] = { .name = "sd",   .full_name = "carrier SD card" },
	[2] = { .name = "",     .full_name = "" },
	[3] = { .name = "",     .full_name = "" },
	[4] = { .name = "",     .full_name = "" },
	[5] = { .name = "net",  .full_name = "network/ethernet" },
	[6] = { .name = "emmc", .full_name = "module eMMC flash" },
	[7] = { .name = "usb",  .full_name = "USB" },
};

static int select_boot_dev(void)
{
	struct gpio_desc *desc;
	u8 bootsel = 0;
	int ret;

	ret = gpio_hog_lookup_name("boot_sel0", &desc);
	if (ret) {
		printf("Error: couldn't get 'boot_sel0' gpio, check your DT.\n");
		return (-ENODEV);
	}
	printf("boot_sel0 value : %d\n", dm_gpio_get_value(desc) << 0);

	bootsel |= dm_gpio_get_value(desc) << 0;

	ret = gpio_hog_lookup_name("boot_sel1", &desc);
	if (ret) {
		printf("Error: couldn't get 'boot_sel1' gpio, check your DT.\n");
		return (-ENODEV);
	}

	printf("boot_sel1 value : %d\n", dm_gpio_get_value(desc) << 1);

	bootsel |= dm_gpio_get_value(desc) << 1;

	ret = gpio_hog_lookup_name("boot_sel2", &desc);
	if (ret) {
		printf("Error: couldn't get 'boot_sel2' gpio, check your DT.\n");
		return (-ENODEV);
	}

	printf("boot_sel2 value : %d\n", dm_gpio_get_value(desc) << 2);

	bootsel |= dm_gpio_get_value(desc) << 2;

	if (bootsel > ARRAY_SIZE(boot_dev)-1 || strlen(boot_dev[bootsel].name) == 0) {
		printf("Warning: invalid boot device detected, check boot selector.\n");
		return (-ENODEV);
	}

	printf("Using %s as boot device ...\n", boot_dev[bootsel].full_name);

	env_set("bootdevstr", boot_dev[bootsel].full_name);
	env_set("bootdev", boot_dev[bootsel].name);

	return 0;
}

static int do_bootsel(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	if (select_boot_dev())
		return CMD_RET_FAILURE;

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	bootsel, 1, 1, do_bootsel,
	"read bootsel (gpios) and set corresponding 'bootdev' (and 'bootdevstr') value",
	""
);
