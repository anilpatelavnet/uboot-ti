#
# Copyright (C) 2023 AVNET Embedded
#
# SPDX-License-Identifier:      GPL-2.0+
#

obj-y						+= string.o
obj-$(CONFIG_DECODE_FEATURE)			+= feature.o
obj-y						+= mmc.o
obj-$(CONFIG_$(SPL_TPL_)BOARDINFO_EEPROM)	+= boardinfo.o
obj-$(CONFIG_$(SPL_TPL_)BOARDINFO_EEPROM)	+= boardinfo_fdt.o
obj-$(CONFIG_$(SPL_TPL_)CMD_FDTLOAD)		+= fdtload.o
obj-$(CONFIG_$(SPL_TPL_)CMD_FDTLOAD)		+= fdt_env.o
obj-$(CONFIG_IMX8M)				+= mx8m_common.o
obj-$(CONFIG_IMX9)				+= mx9_common.o
obj-y						+= som_variant.o

