/*
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <arch.h>
#include <plat/common/platform.h>

#include <platform_def.h>
#include <qti_plat.h>

static const unsigned char bruin_power_domain_tree_desc[] = {
	PLAT_CLUSTER_COUNT,
	PLAT_CLUSTER0_CORE_COUNT,
};

const unsigned char *plat_get_power_domain_tree_desc(void)
{
	return bruin_power_domain_tree_desc;
}

int plat_core_pos_by_mpidr(u_register_t mpidr)
{
	if ((MPIDR_AFFLVL3_VAL(mpidr) != 0U) ||
	    (MPIDR_AFFLVL2_VAL(mpidr) != 0U) ||
	    (MPIDR_AFFLVL1_VAL(mpidr) >= PLAT_CLUSTER_COUNT) ||
	    (MPIDR_AFFLVL0_VAL(mpidr) >= PLAT_CLUSTER0_CORE_COUNT)) {
		return -1;
	}

	return (int)plat_qti_core_pos_by_mpidr(mpidr);
}
