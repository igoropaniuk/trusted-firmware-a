/*
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Debug-only dump of the CPU identification and IMPLEMENTATION DEFINED
 * control registers of the Kryo 2xx Silver core. Every line starts with
 * "KRYO-ID" so the UART log can be parsed mechanically.
 *
 * The implementation defined registers are read through the Cortex-A53
 * encodings; if they are not implemented at those encodings on Kryo the
 * reads trap to EL3 and the crash reporter prints the faulting address.
 */

#include <stdint.h>

#include <arch.h>
#include <arch_helpers.h>
#include <common/debug.h>
#include <cortex_a53.h>

#include <bruin_cpu_id_dump.h>

DEFINE_RENAME_SYSREG_READ_FUNC(kryo_revidr_el1, revidr_el1)
DEFINE_RENAME_SYSREG_READ_FUNC(kryo_aidr_el1, aidr_el1)
DEFINE_RENAME_SYSREG_READ_FUNC(kryo_clidr_el1, clidr_el1)
DEFINE_RENAME_SYSREG_READ_FUNC(kryo_actlr_el3, actlr_el3)
DEFINE_RENAME_SYSREG_READ_FUNC(kryo_cpuectlr_el1, CORTEX_A53_ECTLR_EL1)
DEFINE_RENAME_SYSREG_READ_FUNC(kryo_cpuactlr_el1, CORTEX_A53_CPUACTLR_EL1)
DEFINE_RENAME_SYSREG_READ_FUNC(kryo_l2actlr_el1, CORTEX_A53_L2ACTLR_EL1)
DEFINE_RENAME_SYSREG_READ_FUNC(kryo_l2ectlr_el1, CORTEX_A53_L2ECTLR_EL1)
DEFINE_RENAME_SYSREG_READ_FUNC(kryo_cpumerrsr_el1, CORTEX_A53_MERRSR_EL1)
DEFINE_RENAME_SYSREG_READ_FUNC(kryo_l2merrsr_el1, CORTEX_A53_L2MERRSR_EL1)

#define FIELD(reg, shift, mask)	((unsigned int)(((reg) >> (shift)) & (mask)))
#define BIT_OF(reg, pos)	FIELD(reg, pos, 0x1)
#define NIBBLE(reg, shift)	FIELD(reg, shift, 0xf)
#define TAG			"KRYO-ID "

void bruin_cpu_id_dump(void)
{
	unsigned long long midr = read_midr_el1();
	unsigned long long mpidr = read_mpidr_el1();
	unsigned long long revidr = read_kryo_revidr_el1();
	unsigned long long pfr0 = read_id_aa64pfr0_el1();
	unsigned long long pfr1 = read_id_aa64pfr1_el1();
	unsigned long long dfr0 = read_id_aa64dfr0_el1();
	unsigned long long dfr1 = read_id_aa64dfr1_el1();
	unsigned long long isar0 = read_id_aa64isar0_el1();
	unsigned long long isar1 = read_id_aa64isar1_el1();
	unsigned long long isar2 = read_id_aa64isar2_el1();
	unsigned long long mmfr0 = read_id_aa64mmfr0_el1();
	unsigned long long mmfr1 = read_id_aa64mmfr1_el1();
	unsigned long long mmfr2 = read_id_aa64mmfr2_el1();
	unsigned long long afr0 = read_id_aa64afr0_el1();
	unsigned long long afr1 = read_id_aa64afr1_el1();
	unsigned long long ctr = read_ctr_el0();
	unsigned long long clidr = read_kryo_clidr_el1();
	unsigned long long aidr = read_kryo_aidr_el1();
	unsigned long long actlr3 = read_kryo_actlr_el3();
	unsigned long long cpuectlr, cpuactlr, l2actlr, l2ectlr;
	unsigned long long cpumerrsr, l2merrsr;

	NOTICE(TAG "BEGIN\n");
	NOTICE(TAG "MPIDR_EL1=0x%llx\n", mpidr);
	NOTICE(TAG "MIDR_EL1=0x%llx impl=0x%x var=0x%x arch=0x%x part=0x%x rev=0x%x\n",
	       midr, FIELD(midr, MIDR_IMPL_SHIFT, 0xff),
	       NIBBLE(midr, MIDR_VAR_SHIFT), NIBBLE(midr, 16),
	       FIELD(midr, MIDR_PN_SHIFT, 0xfff), NIBBLE(midr, 0));
	NOTICE(TAG "REVIDR_EL1=0x%llx bit7_835769_fixed=%u bit8_843419_fixed=%u\n",
	       revidr, BIT_OF(revidr, 7), BIT_OF(revidr, 8));
	NOTICE(TAG "AIDR_EL1=0x%llx\n", aidr);

	NOTICE(TAG "ID_AA64PFR0_EL1=0x%llx CSV3=%u CSV2=%u AMU=%u SVE=%u RAS=%u GIC=%u AdvSIMD=%u FP=%u EL3=%u EL2=%u EL1=%u EL0=%u\n",
	       pfr0, NIBBLE(pfr0, 60), NIBBLE(pfr0, ID_AA64PFR0_CSV2_SHIFT),
	       NIBBLE(pfr0, ID_AA64PFR0_AMU_SHIFT), NIBBLE(pfr0, ID_AA64PFR0_SVE_SHIFT),
	       NIBBLE(pfr0, ID_AA64PFR0_RAS_SHIFT), NIBBLE(pfr0, ID_AA64PFR0_GIC_SHIFT),
	       NIBBLE(pfr0, 20), NIBBLE(pfr0, 16), NIBBLE(pfr0, 12), NIBBLE(pfr0, 8),
	       NIBBLE(pfr0, 4), NIBBLE(pfr0, 0));
	NOTICE(TAG "ID_AA64PFR1_EL1=0x%llx MTE=%u SSBS=%u BT=%u\n",
	       pfr1, NIBBLE(pfr1, 8), NIBBLE(pfr1, 4), NIBBLE(pfr1, 0));
	NOTICE(TAG "ID_AA64DFR0_EL1=0x%llx PMSVer_SPE=%u CTX_CMPs=%u WRPs=%u BRPs=%u PMUVer=%u TraceVer=%u DebugVer=%u\n",
	       dfr0, NIBBLE(dfr0, ID_AA64DFR0_PMS_SHIFT), NIBBLE(dfr0, 28),
	       NIBBLE(dfr0, 20), NIBBLE(dfr0, 12), NIBBLE(dfr0, ID_AA64DFR0_PMUVER_SHIFT),
	       NIBBLE(dfr0, ID_AA64DFR0_TRACEVER_SHIFT), NIBBLE(dfr0, 0));
	NOTICE(TAG "ID_AA64DFR1_EL1=0x%llx\n", dfr1);
	NOTICE(TAG "ID_AA64ISAR0_EL1=0x%llx CRC32=%u Atomic=%u SHA2=%u SHA1=%u AES=%u\n",
	       isar0, NIBBLE(isar0, 16), NIBBLE(isar0, 20), NIBBLE(isar0, 12),
	       NIBBLE(isar0, 8), NIBBLE(isar0, 4));
	NOTICE(TAG "ID_AA64ISAR1_EL1=0x%llx GPI=%u GPA=%u API=%u APA=%u\n",
	       isar1, NIBBLE(isar1, ID_AA64ISAR1_GPI_SHIFT),
	       NIBBLE(isar1, ID_AA64ISAR1_GPA_SHIFT),
	       NIBBLE(isar1, ID_AA64ISAR1_API_SHIFT),
	       NIBBLE(isar1, ID_AA64ISAR1_APA_SHIFT));
	NOTICE(TAG "ID_AA64ISAR2_EL1=0x%llx APA3=%u GPA3=%u\n",
	       isar2, NIBBLE(isar2, 12), NIBBLE(isar2, 8));
	NOTICE(TAG "ID_AA64MMFR0_EL1=0x%llx\n", mmfr0);
	NOTICE(TAG "ID_AA64MMFR1_EL1=0x%llx\n", mmfr1);
	NOTICE(TAG "ID_AA64MMFR2_EL1=0x%llx\n", mmfr2);
	NOTICE(TAG "ID_AA64AFR0_EL1=0x%llx ID_AA64AFR1_EL1=0x%llx\n", afr0, afr1);
	NOTICE(TAG "CTR_EL0=0x%llx CLIDR_EL1=0x%llx\n", ctr, clidr);
	NOTICE(TAG "ACTLR_EL3=0x%llx\n", actlr3);

	/*
	 * Cortex-A53 IMPLEMENTATION DEFINED registers. Values reflect what XBL
	 * and the TF-A reset handler have already programmed, not reset values.
	 */
	cpuectlr = read_kryo_cpuectlr_el1();
	NOTICE(TAG "CPUECTLR_EL1=0x%llx SMPEN=%u FPU_RET=%u CPU_RET=%u\n",
	       cpuectlr, BIT_OF(cpuectlr, 6), FIELD(cpuectlr, 3, 0x7),
	       FIELD(cpuectlr, 0, 0x7));
	cpuactlr = read_kryo_cpuactlr_el1();
	NOTICE(TAG "CPUACTLR_EL1=0x%llx ENDCCASCI_855873=%u RADIS=%u L1RADIS=%u DTAH_836870=%u L1PCTL=%u\n",
	       cpuactlr, BIT_OF(cpuactlr, CORTEX_A53_CPUACTLR_EL1_ENDCCASCI_SHIFT),
	       FIELD(cpuactlr, CORTEX_A53_CPUACTLR_EL1_RADIS_SHIFT, 0x3),
	       FIELD(cpuactlr, CORTEX_A53_CPUACTLR_EL1_L1RADIS_SHIFT, 0x3),
	       BIT_OF(cpuactlr, CORTEX_A53_CPUACTLR_EL1_DTAH_SHIFT),
	       FIELD(cpuactlr, CORTEX_A53_CPUACTLR_EL1_L1PCTL_SHIFT, 0x7));
	l2actlr = read_kryo_l2actlr_el1();
	NOTICE(TAG "L2ACTLR_EL1=0x%llx ENABLE_UNIQUECLEAN=%u DISABLE_CLEAN_PUSH_826319=%u\n",
	       l2actlr, BIT_OF(l2actlr, 14), BIT_OF(l2actlr, 3));
	l2ectlr = read_kryo_l2ectlr_el1();
	NOTICE(TAG "L2ECTLR_EL1=0x%llx L2_RET=%u\n", l2ectlr, FIELD(l2ectlr, 0, 0x7));
	cpumerrsr = read_kryo_cpumerrsr_el1();
	l2merrsr = read_kryo_l2merrsr_el1();
	NOTICE(TAG "CPUMERRSR_EL1=0x%llx L2MERRSR_EL1=0x%llx\n", cpumerrsr, l2merrsr);
	NOTICE(TAG "END\n");
}
