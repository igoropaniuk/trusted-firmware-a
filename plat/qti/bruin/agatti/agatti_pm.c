/*
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <arch_helpers.h>
#include <common/debug.h>
#include <drivers/delay_timer.h>
#include <lib/mmio.h>
#include <lib/psci/psci.h>
#include <plat/common/platform.h>

#include <platform_def.h>
#include <qti_plat.h>

#define CPU_PWR_CTL			0x4
#define APC_PWR_GATE_CTL		0x14

#define CPU_PWR_CTL_CLAMP		BIT_32(0)
#define CPU_PWR_CTL_CORE_MEM_CLAMP	BIT_32(1)
#define CPU_PWR_CTL_CORE_MEM_HS		BIT_32(3)
#define CPU_PWR_CTL_CORE_RST		BIT_32(4)
#define CPU_PWR_CTL_COREPOR_RST		BIT_32(5)
#define CPU_PWR_CTL_CORE_PWRD_UP	BIT_32(7)
#define CPU_PWR_CTL_CORE_MEM_RET_N	BIT_32(9)

#define APC_PWR_GATE_CTL_HS_EN		BIT_32(0)
#define APC_PWR_GATE_CTL_HS_CNT(cnt)	((cnt) << 24)

/*
 * The MSM8916 ACS power-up sequence, plus CORE_MEM_RET_N: without it the
 * core memories stay in retention and the core hangs when it enables its
 * caches. CPU_OFF leaves the core powered in WFI, so a subsequent CPU_ON
 * must preserve its power and retention controls while asserting reset.
 */
static void agatti_cpu_boot(uintptr_t acs)
{
	uint32_t pwr_ctl;

	pwr_ctl = mmio_read_32(acs + CPU_PWR_CTL);
	if ((pwr_ctl & CPU_PWR_CTL_CORE_PWRD_UP) != 0U) {
		/* Keep the reset hold time of the cold power-up sequence. */
		pwr_ctl |= CPU_PWR_CTL_CORE_RST | CPU_PWR_CTL_COREPOR_RST;
		mmio_write_32(acs + CPU_PWR_CTL, pwr_ctl);
		dsb();
		udelay(6);
		pwr_ctl &= ~(CPU_PWR_CTL_CORE_RST | CPU_PWR_CTL_COREPOR_RST);
		mmio_write_32(acs + CPU_PWR_CTL, pwr_ctl);
		dsb();
		return;
	}

	pwr_ctl = CPU_PWR_CTL_CLAMP | CPU_PWR_CTL_CORE_MEM_CLAMP |
		  CPU_PWR_CTL_CORE_RST | CPU_PWR_CTL_COREPOR_RST;
	mmio_write_32(acs + CPU_PWR_CTL, pwr_ctl);
	dsb();

	mmio_write_32(acs + APC_PWR_GATE_CTL, APC_PWR_GATE_CTL_HS_EN |
		      APC_PWR_GATE_CTL_HS_CNT(16));
	dsb();
	udelay(2);

	pwr_ctl &= ~CPU_PWR_CTL_CORE_MEM_CLAMP;
	mmio_write_32(acs + CPU_PWR_CTL, pwr_ctl);
	dsb();

	pwr_ctl |= CPU_PWR_CTL_CORE_MEM_HS | CPU_PWR_CTL_CORE_MEM_RET_N;
	mmio_write_32(acs + CPU_PWR_CTL, pwr_ctl);
	dsb();
	udelay(2);

	pwr_ctl &= ~CPU_PWR_CTL_CLAMP;
	mmio_write_32(acs + CPU_PWR_CTL, pwr_ctl);
	dsb();
	udelay(2);

	pwr_ctl &= ~(CPU_PWR_CTL_CORE_RST | CPU_PWR_CTL_COREPOR_RST);
	mmio_write_32(acs + CPU_PWR_CTL, pwr_ctl);
	dsb();

	pwr_ctl |= CPU_PWR_CTL_CORE_PWRD_UP;
	mmio_write_32(acs + CPU_PWR_CTL, pwr_ctl);
	dsb();
}

static int agatti_pwr_domain_on(u_register_t mpidr)
{
	int core_pos = plat_core_pos_by_mpidr(mpidr);

	if (core_pos < 0) {
		return PSCI_E_INVALID_PARAMS;
	}

	agatti_cpu_boot(AGATTI_APCS_ALIAS_ACS(core_pos));

	return PSCI_E_SUCCESS;
}

static void agatti_pwr_domain_on_finish(const psci_power_state_t *target_state)
{
	plat_qti_gic_pcpu_init();
	plat_qti_gic_cpuif_enable();
}

/*
 * The core stays powered in WFI after CPU_OFF: power collapse needs the
 * SAW SPM, which is not set up. CPU_ON resets it through the ACS.
 */
static void agatti_pwr_domain_off(const psci_power_state_t *target_state)
{
	plat_qti_gic_cpuif_disable();
}

static void agatti_cpu_standby(plat_local_state_t cpu_state)
{
	dsb();
	wfi();
}

static int agatti_validate_power_state(unsigned int power_state,
				       psci_power_state_t *req_state)
{
	if ((psci_get_pstate_type(power_state) != PSTATE_TYPE_STANDBY) ||
	    (psci_get_pstate_pwrlvl(power_state) != QTI_PWR_LVL0)) {
		return PSCI_E_INVALID_PARAMS;
	}

	req_state->pwr_domain_state[QTI_PWR_LVL0] = QTI_LOCAL_STATE_STB;

	return PSCI_E_SUCCESS;
}

static void __dead2 agatti_ps_hold_drop(void)
{
	mmio_write_32(QTI_PS_HOLD_REG, 0U);
	mdelay(1000);
	ERROR("PS_HOLD did not take effect\n");
	panic();
}

static void __dead2 agatti_system_off(void)
{
	qti_pmic_prepare_shutdown();
	agatti_ps_hold_drop();
}

static void __dead2 agatti_system_reset(void)
{
	qti_pmic_prepare_reset();
	agatti_ps_hold_drop();
}

static const plat_psci_ops_t agatti_psci_ops = {
	.cpu_standby		= agatti_cpu_standby,
	.pwr_domain_on		= agatti_pwr_domain_on,
	.pwr_domain_on_finish	= agatti_pwr_domain_on_finish,
	.pwr_domain_off		= agatti_pwr_domain_off,
	.validate_power_state	= agatti_validate_power_state,
	.system_off		= agatti_system_off,
	.system_reset		= agatti_system_reset,
};

int plat_setup_psci_ops(uintptr_t sec_entrypoint,
			const plat_psci_ops_t **psci_ops)
{
	/* The reset vector registers hold the entry address in words. */
	mmio_write_32(AGATTI_APCS_COMMON_RVBARADDR_LO,
		      (uint32_t)(sec_entrypoint >> 2));
	mmio_write_32(AGATTI_APCS_COMMON_RVBARADDR_HI,
		      (uint32_t)(sec_entrypoint >> 34));

	*psci_ops = &agatti_psci_ops;

	return 0;
}
