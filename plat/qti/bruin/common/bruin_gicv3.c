/*
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <drivers/arm/gicv3.h>

#include <qti_plat.h>

/* No secure interrupts are claimed yet: everything is left to the NS world. */
const interrupt_prop_t *plat_qti_get_interrupt_props(unsigned int *num_props)
{
	*num_props = 0U;
	return NULL;
}
