/*
 * Copyright (C) 2022 Avnet Embedded
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

#include <asm/arch/sys_proto.h>
#include "mx9_common.h"

const char* mx9_get_plat_str(void)
{
	switch(get_cpu_type()) {
		case MXC_CPU_IMX93:
			return "imx93";
	}
	return "N/A";
}

const char* mx9_get_proc_str(void)
{
	switch(get_cpu_type()) {
		case MXC_CPU_IMX93:
			return "imx93";
	}
	return "N/A";
}
