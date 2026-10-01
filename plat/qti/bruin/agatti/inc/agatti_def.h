/*
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef AGATTI_DEF_H
#define AGATTI_DEF_H

#include <common_def.h>
#include <qti_board_def.h>

/* One cluster of four Kryo 2xx Silver (Cortex-A53 based) cores. */
#define PLAT_CLUSTER_COUNT			1
#define PLAT_CLUSTER0_CORE_COUNT		4
#define PLATFORM_CORE_COUNT			PLAT_CLUSTER0_CORE_COUNT
#define PLAT_NUM_PWR_DOMAINS			(PLAT_CLUSTER_COUNT + \
						 PLATFORM_CORE_COUNT)
#define PLAT_MAX_PWR_LVL			MPIDR_AFFLVL1

#define QTI_PWR_LVL0				MPIDR_AFFLVL0
#define QTI_PWR_LVL1				MPIDR_AFFLVL1

#define QTI_LOCAL_STATE_RUN			0
#define QTI_LOCAL_STATE_STB			1
#define QTI_LOCAL_STATE_RET			2
#define QTI_LOCAL_STATE_OFF			3

#define PLAT_MAX_RET_STATE			QTI_LOCAL_STATE_RET
#define PLAT_MAX_OFF_STATE			QTI_LOCAL_STATE_OFF

#define MAX_MMAP_REGIONS			(PLAT_QTI_MMAP_ENTRIES)
#define PLAT_PHY_ADDR_SPACE_SIZE		(1ull << 36)
#define PLAT_VIRT_ADDR_SPACE_SIZE		(1ull << 36)

#define ARM_CACHE_WRITEBACK_SHIFT		6
#define CACHE_WRITEBACK_GRANULE			(1 << ARM_CACHE_WRITEBACK_SHIFT)

/* GICv3 (no GICR_PWRR, so not a GIC-600). */
#define QTI_GICD_BASE				0x0f200000
#define QTI_GICR_BASE				0x0f300000
#define QTI_GICC_BASE				0x0

/* Memory-mapped timer (QTimer) CNTCTLBase and its number of frames */
#define QTI_QTMR_AC_BASE			0x0f120000
#define QTI_QTMR_FRAMES				7

/* GENI serial engine register offsets used by qti_uart_console.S. */
#define GENI4_CFG				0x0
#define GENI4_IMAGE_REGS			0x100
#define GENI4_DATA				0x600
#define GENI_STATUS_REG				(GENI4_CFG + 0x00000040)
#define GENI_STATUS_M_GENI_CMD_ACTIVE_MASK	(0x1)
#define UART_TX_TRANS_LEN_REG			(GENI4_IMAGE_REGS + 0x00000170)
#define GENI_M_CMD0_REG				(GENI4_DATA + 0x00000000)
#define GENI_TX_FIFOn_REG			(GENI4_DATA + 0x00000100)
#define GENI_M_CMD_TX				(0x08000000)

/* Debug UART left configured by XBL (QUPv3 serial engine 4). */
#define PLAT_QTI_UART_BASE			0x04a90000

#define QTI_PS_HOLD_REG				0x0440b000

/* APCS per-CPU power controller (ACS) and the cluster reset vector. */
#define AGATTI_APCS_ALIAS_ACS(cpu)		(0x0f188000 + (cpu) * 0x10000)
#define AGATTI_APCS_COMMON_RVBARADDR_LO		0x0f1d1108
#define AGATTI_APCS_COMMON_RVBARADDR_HI		0x0f1d110c

/* PMIC arbiter v5 and the PM4125 PON on SID 0. */
#define PLAT_QTI_SPMI_ARB_CORE_BASE		0x01c40000
#define PLAT_QTI_SPMI_ARB_CHNLS_BASE		0x01e00000
#define PON_PS_HOLD_RESET_CTL			0x85a
#define PON_PS_HOLD_RESET_CTL2			0x85b

/*
 * Every peripheral sits below the pIMEM aperture. BL2's window in IMEM
 * (0x0c100000) lies inside this range and is mapped as memory on top.
 */
#define QTI_DEVICE_BASE				0x00001000
#define QTI_DEVICE_SIZE				(0x10000000 - QTI_DEVICE_BASE)

/* Secure part of the pIMEM aperture that the stock QSEE runs from. */
#define QTI_PIMEM_BASE				0x10000000
#define QTI_PIMEM_LIMIT				0x10500000

#define QTI_SMEM_BASE				0x46000000
#define QTI_SMEM_SIZE				0x00200000

#define QTI_SOC_VERSION_MASK			U(0xFFFF)
#define QTI_SOC_REVISION_REG			0x003c8000
#define QTI_SOC_REVISION_MASK			U(0xFFFF)

#define PLAT_INT_ID_CPU_WAKEUP_SGI		(0x8)
#define PLAT_INT_ID_RESET_SGI			(0xf)

#endif /* AGATTI_DEF_H */
