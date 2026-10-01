/*
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <arch.h>
#include <asm_macros.S>

	.globl	plat_my_core_pos
	.globl	plat_qti_core_pos_by_mpidr

	/* -----------------------------------------------------
	 * unsigned int plat_qti_core_pos_by_mpidr(u_register_t mpidr)
	 * CorePos = (ClusterId * 4) + CoreId, with ClusterId in
	 * MPIDR[15:8] and CoreId in MPIDR[7:0] (no MT on bruin).
	 * Clobbers: x0, x1.
	 * -----------------------------------------------------
	 */
func plat_qti_core_pos_by_mpidr
	and	x1, x0, #MPIDR_CPU_MASK
	and	x0, x0, #MPIDR_CLUSTER_MASK
	add	x0, x1, x0, LSR #6
	ret
endfunc plat_qti_core_pos_by_mpidr

func plat_my_core_pos
	mrs	x0, mpidr_el1
	b	plat_qti_core_pos_by_mpidr
endfunc plat_my_core_pos
