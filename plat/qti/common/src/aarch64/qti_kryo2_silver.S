/*
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <arch.h>
#include <asm_macros.S>
#include <cpu_macros.S>

#include <qti_cpu.h>

/* Kryo 2xx Silver is a Cortex-A53 with a Qualcomm MIDR. */
declare_cpu_ops	qti_kryo2_silver, QTI_KRYO2_SILVER_MIDR,	\
		cortex_a53_reset_func,			\
		cortex_a53_core_pwr_dwn,		\
		cortex_a53_cluster_pwr_dwn
