/*
 * Copyright (c) 2014-2026, Arm Limited and Contributors. All rights reserved.
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Kryo 2xx Silver is a Cortex-A53 r0p4 with a Qualcomm MIDR (0x51AF8014,
 * variant 0xA, revision 0x4). Its ID registers are identical to the
 * Cortex-A53 r0p4 and it reports the 835769 and 843419 fixes in REVIDR_EL1
 * bits 7 and 8. The reset and power down sequences and the IMPLEMENTATION
 * DEFINED registers follow the Cortex-A53 TRM, so the register definitions
 * are shared with the Cortex-A53 driver.
 *
 * A single revision (rAp4) of the core exists. Qualcomm's variant and revision
 * encoding has no relation to the Cortex-A53 rNpM scheme, so the errata are
 * selected by build flag rather than by revision. Of the Cortex-A53 errata,
 * only 855873 (r0p3 and later) and 1530924 (all revisions) affect this core.
 * The Cortex-A53 build flags are reused since the erratum numbers are the same.
 *
 * TRM: https://support.arm.com/documentation/ddi0500/latest
 * SDEN: https://support.arm.com/documentation/epm048406/latest
 */
#include <arch.h>
#include <asm_macros.S>
#include <common/bl_common.h>
#include <cortex_a53.h>
#include <cpu_macros.S>
#include <plat_macros.S>
#include <lib/cpus/errata.h>

#include <qti_cpu.h>

cpu_reset_prologue qti_kryo2_silver

	/* ---------------------------------------------
	 * Disable intra-cluster coherency
	 * ---------------------------------------------
	 */
func qti_kryo2_silver_disable_smp
	sysreg_bit_clear CORTEX_A53_ECTLR_EL1, CORTEX_A53_ECTLR_SMP_BIT
	isb
	dsb	sy
	ret
endfunc qti_kryo2_silver_disable_smp

	/*
	 * Erratum 855873 applies to Cortex-A53 r0p3 and later, which have a
	 * chicken bit in CPUACTLR_EL1 to enable the hardware workaround.
	 */
workaround_reset_start qti_kryo2_silver, ERRATUM(855873), ERRATA_A53_855873
	sysreg_bit_set CORTEX_A53_CPUACTLR_EL1, CORTEX_A53_CPUACTLR_EL1_ENDCCASCI
workaround_reset_end qti_kryo2_silver, ERRATUM(855873)

check_erratum_chosen qti_kryo2_silver, ERRATUM(855873), ERRATA_A53_855873

	/*
	 * Erratum 1530924 applies to every Cortex-A53 revision. It has no
	 * workaround in the cpu, generic code (ERRATA_SPECULATIVE_AT) must take
	 * care.
	 */
check_erratum_chosen qti_kryo2_silver, ERRATUM(1530924), ERRATA_A53_1530924

add_erratum_entry qti_kryo2_silver, ERRATUM(1530924), ERRATA_A53_1530924

cpu_reset_func_start qti_kryo2_silver
	/* Enable the SMP bit. */
	sysreg_bit_set CORTEX_A53_ECTLR_EL1, CORTEX_A53_ECTLR_SMP_BIT
cpu_reset_func_end qti_kryo2_silver

func qti_kryo2_silver_core_pwr_dwn
	mov	x18, x30

	/* ---------------------------------------------
	 * Flush L1 caches.
	 * ---------------------------------------------
	 */
	mov	x0, #DCCISW
	bl	dcsw_op_level1

	/* ---------------------------------------------
	 * Come out of intra cluster coherency
	 * ---------------------------------------------
	 */
	mov	x30, x18
	b	qti_kryo2_silver_disable_smp
endfunc qti_kryo2_silver_core_pwr_dwn

func qti_kryo2_silver_cluster_pwr_dwn
	mov	x18, x30

	/* ---------------------------------------------
	 * Flush L1 caches.
	 * ---------------------------------------------
	 */
	mov	x0, #DCCISW
	bl	dcsw_op_level1

	/* ---------------------------------------------
	 * Disable the optional ACP.
	 * ---------------------------------------------
	 */
	bl	plat_disable_acp

	/* ---------------------------------------------
	 * Flush L2 caches.
	 * ---------------------------------------------
	 */
	mov	x0, #DCCISW
	bl	dcsw_op_level2

	/* ---------------------------------------------
	 * Come out of intra cluster coherency
	 * ---------------------------------------------
	 */
	mov	x30, x18
	b	qti_kryo2_silver_disable_smp
endfunc qti_kryo2_silver_cluster_pwr_dwn

	/* ---------------------------------------------
	 * This function provides Kryo 2xx Silver specific
	 * register information for crash reporting.
	 * It needs to return with x6 pointing to
	 * a list of register names in ascii and
	 * x8 - x15 having values of registers to be
	 * reported.
	 * ---------------------------------------------
	 */
.section .rodata.qti_kryo2_silver_regs, "aS"
qti_kryo2_silver_regs:  /* The ascii list of register names to be reported */
	.asciz	"cpuectlr_el1", "cpumerrsr_el1", "l2merrsr_el1", \
		"cpuactlr_el1", ""

func qti_kryo2_silver_cpu_reg_dump
	adr	x6, qti_kryo2_silver_regs
	mrs	x8, CORTEX_A53_ECTLR_EL1
	mrs	x9, CORTEX_A53_MERRSR_EL1
	mrs	x10, CORTEX_A53_L2MERRSR_EL1
	mrs	x11, CORTEX_A53_CPUACTLR_EL1
	ret
endfunc qti_kryo2_silver_cpu_reg_dump

declare_cpu_ops qti_kryo2_silver, QTI_KRYO2_SILVER_MIDR, \
	qti_kryo2_silver_reset_func, \
	qti_kryo2_silver_core_pwr_dwn, \
	qti_kryo2_silver_cluster_pwr_dwn
