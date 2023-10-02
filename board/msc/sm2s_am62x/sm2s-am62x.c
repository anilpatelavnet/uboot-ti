// SPDX-License-Identifier: GPL-2.0+
/*
 * Board specific initialization for AM62x platforms
 *
 * Copyright (C) 2023 AVNET Embedded, MSC Technologies GmbH
 *
 */

#include <common.h>
#include <env.h>
#include <spl.h>
#include <init.h>
#include <video.h>
#include <splash.h>
#include <cpu_func.h>
#include <k3-ddrss.h>
#include <fdt_support.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <dm/uclass.h>
#include <net.h>
#include <asm/gpio.h>
#include <cpu_func.h>
#include "../common/boardinfo.h"
#include "../common/boardinfo_fdt.h"
#include "../common/spl.h"


DECLARE_GLOBAL_DATA_PTR;

const board_info_t *binfo = NULL;

#define ENV_FDTFILE_MAX_SIZE 		64

#define AM62X_MAX_DAUGHTER_CARDS	0

/* Daughter card presence detection signals */
enum {
	AM62X_LPSK_HSE_BRD_DET,
	AM62X_LPSK_BRD_DET_COUNT,
};

#if !defined(CONFIG_SPL_BUILD) && defined(CONFIG_ARM64)
static struct gpio_desc board_det_gpios[AM62X_LPSK_BRD_DET_COUNT];
#endif

/* Max number of MAC addresses that are parsed/processed per daughter card */
#define DAUGHTER_CARD_NO_OF_MAC_ADDR	0

#define board_is_am62x_skevm()   0
#define board_is_am62x_lp_skevm() 0
#define board_is_am62x_play()	0

#if CONFIG_IS_ENABLED(SPLASH_SCREEN)
static struct splash_location default_splash_locations[] = {
	{
		.name = "sf",
		.storage = SPLASH_STORAGE_SF,
		.flags = SPLASH_STORAGE_RAW,
		.offset = 0x700000,
	},
	{
		.name		= "mmc",
		.storage	= SPLASH_STORAGE_MMC,
		.flags		= SPLASH_STORAGE_FS,
		.devpart	= "1:1",
	},
};

int splash_screen_prepare(void)
{
	return splash_source_load(default_splash_locations,
				ARRAY_SIZE(default_splash_locations));
}
#endif

int board_init(void)
{
        binfo = bi_read();
        if (binfo == NULL) {
                printf("Warning: failed to initialize boardinfo!\n");
        }

	return 0;
}

int dram_init(void)
{
	return fdtdec_setup_mem_size_base();
}

int dram_init_banksize(void)
{
	return fdtdec_setup_memory_banksize();
}

#if defined(CONFIG_SPL_BUILD)
static int video_setup(void)
{
	if (CONFIG_IS_ENABLED(VIDEO)) {
		ulong addr;
		int ret;

		addr = gd->relocaddr;
		ret = video_reserve(&addr);
		if (ret)
			return ret;
		debug("Reserving %luk for video at: %08lx\n",
		      ((unsigned long)gd->relocaddr - addr) >> 10, addr);
		gd->relocaddr = addr;
	}

	return 0;
}

#define CTRLMMR_USB0_PHY_CTRL	0x43004008
#define CTRLMMR_USB1_PHY_CTRL	0x43004018
#define CORE_VOLTAGE		0x80000000

#define WKUP_CTRLMMR_DBOUNCE_CFG1 0x04504084
#define WKUP_CTRLMMR_DBOUNCE_CFG2 0x04504088
#define WKUP_CTRLMMR_DBOUNCE_CFG3 0x0450408c
#define WKUP_CTRLMMR_DBOUNCE_CFG4 0x04504090
#define WKUP_CTRLMMR_DBOUNCE_CFG5 0x04504094
#define WKUP_CTRLMMR_DBOUNCE_CFG6 0x04504098

void spl_board_init(void)
{
	u32 val;

	/* Set USB0 PHY core voltage to 0.85V */
	val = readl(CTRLMMR_USB0_PHY_CTRL);
	val &= ~(CORE_VOLTAGE);
	writel(val, CTRLMMR_USB0_PHY_CTRL);

	/* Set USB1 PHY core voltage to 0.85V */
	val = readl(CTRLMMR_USB1_PHY_CTRL);
	val &= ~(CORE_VOLTAGE);
	writel(val, CTRLMMR_USB1_PHY_CTRL);

	/* We have 32k crystal, so lets enable it */
	val = readl(MCU_CTRL_LFXOSC_CTRL);
	val &= ~(MCU_CTRL_LFXOSC_32K_DISABLE_VAL);
	writel(val, MCU_CTRL_LFXOSC_CTRL);
	/* Add any TRIM needed for the crystal here.. */
	/* Make sure to mux up to take the SoC 32k from the crystal */
	writel(MCU_CTRL_DEVICE_CLKOUT_LFOSC_SELECT_VAL,
	       MCU_CTRL_DEVICE_CLKOUT_32K_CTRL);

	/* Setup debounce conf registers - arbitrary values. Times are approx */
	/* 1.9ms debounce @ 32k */
	writel(WKUP_CTRLMMR_DBOUNCE_CFG1, 0x1);
	/* 5ms debounce @ 32k */
	writel(WKUP_CTRLMMR_DBOUNCE_CFG2, 0x5);
	/* 20ms debounce @ 32k */
	writel(WKUP_CTRLMMR_DBOUNCE_CFG3, 0x14);
	/* 46ms debounce @ 32k */
	writel(WKUP_CTRLMMR_DBOUNCE_CFG4, 0x18);
	/* 100ms debounce @ 32k */
	writel(WKUP_CTRLMMR_DBOUNCE_CFG5, 0x1c);
	/* 156ms debounce @ 32k */
	writel(WKUP_CTRLMMR_DBOUNCE_CFG6, 0x1f);

	video_setup();
	enable_caches();
	if (IS_ENABLED(CONFIG_SPL_SPLASH_SCREEN) && IS_ENABLED(CONFIG_SPL_BMP))
		splash_display();

	if (IS_ENABLED(CONFIG_SPL_ETH))
		/* Init DRAM size for R5/A53 SPL */
		dram_init_banksize();
}

#if defined(CONFIG_K3_AM64_DDRSS)
static void fixup_ddr_driver_for_ecc(struct spl_image_info *spl_image)
{
	struct udevice *dev;
	int ret;

	dram_init_banksize();

	ret = uclass_get_device(UCLASS_RAM, 0, &dev);
	if (ret)
		panic("Cannot get RAM device for ddr size fixup: %d\n", ret);

	ret = k3_ddrss_ddr_fdt_fixup(dev, spl_image->fdt_addr, gd->bd);
	if (ret)
		printf("Error fixing up ddr node for ECC use! %d\n", ret);
}
#else
static void fixup_memory_node(struct spl_image_info *spl_image)
{
	u64 start[CONFIG_NR_DRAM_BANKS];
	u64 size[CONFIG_NR_DRAM_BANKS];
	int bank;
	int ret;

	dram_init();
	dram_init_banksize();

	for (bank = 0; bank < CONFIG_NR_DRAM_BANKS; bank++) {
		start[bank] =  gd->bd->bi_dram[bank].start;
		size[bank] = gd->bd->bi_dram[bank].size;
	}

	/* dram_init functions use SPL fdt, and we must fixup u-boot fdt */
	ret = fdt_fixup_memory_banks(spl_image->fdt_addr, start, size,
				     CONFIG_NR_DRAM_BANKS);
	if (ret)
		printf("Error fixing up memory node! %d\n", ret);
}
#endif

void spl_perform_fixups(struct spl_image_info *spl_image)
{
#if defined(CONFIG_K3_AM64_DDRSS)
	fixup_ddr_driver_for_ecc(spl_image);
#else
	fixup_memory_node(spl_image);
#endif
}
#endif

#ifdef CONFIG_TI_I2C_BOARD_DETECT
int do_board_detect(void)
{
	int ret;

	ret = ti_i2c_eeprom_am6_get_base(CONFIG_EEPROM_BUS_ADDRESS,
					 CONFIG_EEPROM_CHIP_ADDRESS);

	if (ret) {
		printf("EEPROM not available at %d, trying to read at %d\n",
			CONFIG_EEPROM_CHIP_ADDRESS, CONFIG_EEPROM_CHIP_ADDRESS + 1);
		ret = ti_i2c_eeprom_am6_get_base(CONFIG_EEPROM_BUS_ADDRESS,
						 CONFIG_EEPROM_CHIP_ADDRESS + 1);
		if (ret)
			pr_err("Reading on-board EEPROM at 0x%02x failed %d\n",
				CONFIG_EEPROM_CHIP_ADDRESS + 1, ret);
	}
	return ret;
}

int checkboard(void)
{
	struct ti_am6_eeprom *ep = TI_AM6_EEPROM_DATA;

	if (!do_board_detect())
		printf("Board: %s rev %s\n", ep->name, ep->version);

	return 0;
}
#endif

#ifdef CONFIG_BOARD_LATE_INIT
static void setup_board_eeprom_env(void)
{
	char *name = "sm2s_am62x";

	set_board_info_env_am6(name);
}

static void setup_serial(void)
{
#if 0
	struct ti_am6_eeprom *ep = TI_AM6_EEPROM_DATA;
	unsigned long board_serial;
	char *endp;
	char serial_string[17] = { 0 };

	if (env_get("serial#"))
		return;

	board_serial = simple_strtoul(ep->serial, &endp, 16);
	if (*endp != '\0') {
		pr_err("Error: Can't set serial# to %s\n", ep->serial);
		return;
	}

	snprintf(serial_string, sizeof(serial_string), "%016lx", board_serial);
	env_set("serial#", serial_string);
#endif
}


int board_late_init(void)
{
#if 0
	if (IS_ENABLED(CONFIG_TI_I2C_BOARD_DETECT)) {
		struct ti_am6_eeprom *ep = TI_AM6_EEPROM_DATA;

		setup_board_eeprom_env();
		setup_serial();
		/*
		 * The first MAC address for ethernet a.k.a. ethernet0 comes from
		 * efuse populated via the am654 gigabit eth switch subsystem driver.
		 * All the other ones are populated via EEPROM, hence continue with
		 * an index of 1.
		 */
		board_ti_am6_set_ethaddr(1, ep->mac_addr_cnt);

	}
#endif

#ifdef CONFIG_ENV_IS_IN_MMC
        board_late_mmc_env_init();
#endif

        if (binfo) {
                const char *fdt;
                char buff[ENV_FDTFILE_MAX_SIZE];

#if defined(CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG)
                env_set("bi_company", bi_get_company(binfo));
                env_set("bi_form_factor", bi_get_form_factor(binfo));
                env_set("bi_platform", bi_get_platform(binfo));
                env_set("bi_processor", bi_get_processor(binfo));
                env_set("bi_feature", bi_get_feature(binfo));
                env_set("bi_serial", bi_get_serial(binfo));
                env_set("bi_revision", bi_get_revision(binfo));
#endif

                fdt = env_get("fdt_module");
                if ( (fdt == NULL) || (!strcmp(fdt, "undefined")) ) {
                        snprintf(buff, ENV_FDTFILE_MAX_SIZE, "msc/am62xx/%s-%s-%s-%s-%s-module.dtb",
                                        bi_get_company(binfo), bi_get_form_factor(binfo), bi_get_platform(binfo),
                                        bi_get_processor(binfo), bi_get_feature(binfo));
                        env_set("fdt_module", buff);
						env_set("name_fdt", buff);
						env_set("default_device_tree", buff);
                }
        }

	return 0;
}
#endif

int board_fixup_fdt(void *fdt)
{
        if (binfo)
                return bi_fixup_fdt(binfo, fdt, NULL, NULL);

        return 0;
}

