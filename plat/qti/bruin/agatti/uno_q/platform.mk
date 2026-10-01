#
# Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
#
# SPDX-License-Identifier: BSD-3-Clause
#

# Makefile for the Agatti (QRB2210) based Arduino UNO Q.

PLAT_PATH				:=	plat/qti
CHIPSET					:=	agatti

# XBL loads BL2 as the TZ image; BL2 loads the rest from the FIP.
RESET_TO_BL2				:=	1
COLD_BOOT_SINGLE_CPU			:=	1
PROGRAMMABLE_RESET_ADDRESS		:=	1

SEPARATE_CODE_AND_RODATA		:=	1
USE_COHERENT_MEM			:=	0
WARMBOOT_ENABLE_DCACHE_EARLY		:=	1

# Kryo 2xx Silver is a Cortex-A53 r0p4: no SPE, no SVE, no branch predictor
# CVEs.
ENABLE_SPE_FOR_NS			:=	0
ENABLE_SVE_FOR_NS			:=	0
WORKAROUND_CVE_2017_5715		:=	0
WORKAROUND_CVE_2022_23960		:=	0

# Of the Cortex-A53 errata only 855873 (r0p3 and later) and 1530924 (all
# revisions) apply. The silicon reports the 835769 and 843419 fixes in
# REVIDR_EL1, so the toolchain workarounds are not needed either.
ERRATA_A53_855873			:=	1
ERRATA_A53_1530924			:=	1

PLAT_XLAT_TABLES_DYNAMIC		:=	1
$(eval $(call add_define,PLAT_XLAT_TABLES_DYNAMIC))

PLAT_INCLUDES		:=	-Iinclude/plat/common/					\
				-I${PLAT_PATH}/bruin/${CHIPSET}/inc			\
				-I${PLAT_PATH}/bruin/${CHIPSET}/${PLAT}/inc		\
				-I${PLAT_PATH}/common/inc				\
				-I${PLAT_PATH}/common/inc/$(ARCH)

include lib/xlat_tables_v2/xlat_tables.mk
PLAT_BL_COMMON_SOURCES	+=	common/desc_image_load.c				\
				plat/common/aarch64/crash_console_helpers.S		\
				$(PLAT_PATH)/bruin/common/$(ARCH)/bruin_helpers.S	\
				$(PLAT_PATH)/common/src/$(ARCH)/qti_kryo2_silver.S	\
				$(PLAT_PATH)/common/src/$(ARCH)/qti_uart_console.S	\
				$(PLAT_PATH)/common/src/qti_common.c			\
				${XLAT_TABLES_LIB_SRCS}

BL2_SOURCES		+=	drivers/io/io_fip.c					\
				drivers/io/io_memmap.c					\
				drivers/io/io_storage.c					\
				$(PLAT_PATH)/common/src/$(ARCH)/qti_bl2_helpers.S	\
				$(PLAT_PATH)/common/src/qti_bl2_setup.c			\
				$(PLAT_PATH)/common/src/qti_image_desc.c		\
				$(PLAT_PATH)/common/src/qti_io_storage.c

include drivers/arm/gic/v3/gicv3.mk
BL31_SOURCES		+=	drivers/delay_timer/delay_timer.c			\
				drivers/delay_timer/generic_delay_timer.c		\
				plat/common/plat_gicv3.c				\
				${GICV3_SOURCES}					\
				plat/common/plat_psci_common.c				\
				$(PLAT_PATH)/common/src/pm_ps_hold.c			\
				$(PLAT_PATH)/common/src/qti_gic_v3.c			\
				$(PLAT_PATH)/common/src/qti_syscall.c			\
				$(PLAT_PATH)/common/src/spmi_arb.c			\
				$(PLAT_PATH)/bruin/common/bruin_bl31_setup.c		\
				$(PLAT_PATH)/bruin/common/bruin_gicv3.c			\
				$(PLAT_PATH)/bruin/common/bruin_topology.c		\
				$(PLAT_PATH)/bruin/${CHIPSET}/${CHIPSET}_pm.c		\
				drivers/qti/accesscontrol/access_control_stub.c

include drivers/qti/smem/smem.mk
include drivers/qti/chipinfo/chipinfo.mk
