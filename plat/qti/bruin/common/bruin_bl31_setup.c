/*
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>

#include <bl31/bl31.h>
#include <common/bl_common.h>
#include <common/debug.h>
#include <common/desc_image_load.h>
#include <drivers/console.h>
#include <drivers/generic_delay_timer.h>
#include <drivers/qti/chipinfo/chipinfo.h>
#include <drivers/qti/smem/smem.h>
#include <plat/common/platform.h>

#include <bruin_cpu_id_dump.h>
#include <platform_def.h>
#include <qti_plat.h>
#include <qti_uart_console.h>

static console_t bruin_console;
static entry_point_info_t bl32_image_ep_info;
static entry_point_info_t bl33_image_ep_info;

void bl31_early_platform_setup2(u_register_t arg0, u_register_t arg1,
				u_register_t arg2, u_register_t arg3)
{
	qti_console_uart_register(&bruin_console, PLAT_QTI_UART_BASE);
	console_set_scope(&bruin_console, CONSOLE_FLAG_BOOT |
			  CONSOLE_FLAG_RUNTIME | CONSOLE_FLAG_CRASH);

	bl31_params_parse_helper(arg0, &bl32_image_ep_info,
				 &bl33_image_ep_info);
}

void bl31_plat_arch_setup(void)
{
	qti_setup_page_tables(BL31_START, BL31_END - BL31_START,
			      BL_CODE_BASE, BL_CODE_END,
			      BL_RO_DATA_BASE, BL_RO_DATA_END);
	enable_mmu_el3(0);
}

void bl31_platform_setup(void)
{
	bruin_cpu_id_dump();

	generic_delay_timer_init();
	plat_qti_gic_driver_init();
	plat_qti_gic_init();

	qti_smem_init();
	if (qti_chipinfo_init() != CHIPINFO_SUCCESS) {
		WARN("ChipInfo initialization error\n");
	}
}

entry_point_info_t *bl31_plat_get_next_image_ep_info(uint32_t type)
{
	entry_point_info_t *ep;

	assert(sec_state_is_valid(type) != 0);
	ep = (type == SECURE) ? &bl32_image_ep_info : &bl33_image_ep_info;

	return (ep->pc != 0U) ? ep : NULL;
}

unsigned int plat_get_syscnt_freq2(void)
{
	return PLAT_SYSCNT_FREQ;
}
