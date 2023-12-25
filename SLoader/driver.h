#pragma once

#include <ntddk.h>

#pragma warning(disable:28182 6387)

#define DEVICE_NAME L"\\Device\\SLoader"
#define SYM_LINK_NAME L"\\??\\SLoaderCtl"

#define PATCHCILOAD CTL_CODE(FILE_DEVICE_UNKNOWN, 0x810, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define LOADING CTL_CODE(FILE_DEVICE_UNKNOWN, 0x820, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define LOADINGEX CTL_CODE(FILE_DEVICE_UNKNOWN, 0x830, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define UNLOAD CTL_CODE(FILE_DEVICE_UNKNOWN, 0x840, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define ZW_QUERY_SYS_INFO 'ISQZ'

#define _64R8U(p) *(PUINT64)(p)
#define _64R4U(p) *(PULONG)(p)

// sys peculiarity code
#define CHECK_BLOCK_SIZE 0x8
static UCHAR WIN10LTSC_21H2_19044_MARK[] = { 0x8B ,0x0D ,0x84 ,0xAB ,0xFD ,0xFF ,0x83 ,0xE1 };
static UCHAR WIN11_23H2_22631_MARK[] = { 0x8B ,0x0D ,0xF8 ,0xC7 ,0xFD ,0xFF ,0xBB ,0x00 };

// Fn DriverEntry
typedef NTSTATUS(*FnDriverEntry)(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPathy);

// WINDOWS 10 LTSC SYS CLASS
typedef enum _SYSTEM_INFORMATION_CLASS {
	SystemBasicInformation,	// 0n0
	SystemProcessorInformation,	// 0n1
	SystemPerformanceInformation,	// 0n2
	SystemTimeOfDayInformation,	// 0n3
	SystemPathInformation,	// 0n4
	SystemProcessInformation,	// 0n5
	SystemCallCountInformation,	// 0n6
	SystemDeviceInformation,	// 0n7
	SystemProcessorPerformanceInformation,	// 0n8
	SystemFlagsInformation,	// 0n9
	SystemCallTimeInformation,	// 0n10
	SystemModuleInformation,	// 0n11
	SystemLocksInformation,	// 0n12
	SystemStackTraceInformation,	// 0n13
	SystemPagedPoolInformation,	// 0n14
	SystemNonPagedPoolInformation,	// 0n15
	SystemHandleInformation,	// 0n16
	SystemObjectInformation,	// 0n17
	SystemPageFileInformation,	// 0n18
	SystemVdmInstemulInformation,	// 0n19
	SystemVdmBopInformation,	// 0n20
	SystemFileCacheInformation,	// 0n21
	SystemPoolTagInformation,	// 0n22
	SystemInterruptInformation,	// 0n23
	SystemDpcBehaviorInformation,	// 0n24
	SystemFullMemoryInformation,	// 0n25
	SystemLoadGdiDriverInformation,	// 0n26
	SystemUnloadGdiDriverInformation,	// 0n27
	SystemTimeAdjustmentInformation,	// 0n28
	SystemSummaryMemoryInformation,	// 0n29
	SystemMirrorMemoryInformation,	// 0n30
	SystemPerformanceTraceInformation,	// 0n31
	SystemObsolete0,	// 0n32
	SystemExceptionInformation,	// 0n33
	SystemCrashDumpStateInformation,	// 0n34
	SystemKernelDebuggerInformation,	// 0n35
	SystemContextSwitchInformation,	// 0n36
	SystemRegistryQuotaInformation,	// 0n37
	SystemExtendServiceTableInformation,	// 0n38
	SystemPrioritySeperation,	// 0n39
	SystemVerifierAddDriverInformation,	// 0n40
	SystemVerifierRemoveDriverInformation,	// 0n41
	SystemProcessorIdleInformation,	// 0n42
	SystemLegacyDriverInformation,	// 0n43
	SystemCurrentTimeZoneInformation,	// 0n44
	SystemLookasideInformation,	// 0n45
	SystemTimeSlipNotification,	// 0n46
	SystemSessionCreate,	// 0n47
	SystemSessionDetach,	// 0n48
	SystemSessionInformation,	// 0n49
	SystemRangeStartInformation,	// 0n50
	SystemVerifierInformation,	// 0n51
	SystemVerifierThunkExtend,	// 0n52
	SystemSessionProcessInformation,	// 0n53
	SystemLoadGdiDriverInSystemSpace,	// 0n54
	SystemNumaProcessorMap,	// 0n55
	SystemPrefetcherInformation,	// 0n56
	SystemExtendedProcessInformation,	// 0n57
	SystemRecommendedSharedDataAlignment,	// 0n58
	SystemComPlusPackage,	// 0n59
	SystemNumaAvailableMemory,	// 0n60
	SystemProcessorPowerInformation,	// 0n61
	SystemEmulationBasicInformation,	// 0n62
	SystemEmulationProcessorInformation,	// 0n63
	SystemExtendedHandleInformation,	// 0n64
	SystemLostDelayedWriteInformation,	// 0n65
	SystemBigPoolInformation,	// 0n66
	SystemSessionPoolTagInformation,	// 0n67
	SystemSessionMappedViewInformation,	// 0n68
	SystemHotpatchInformation,	// 0n69
	SystemObjectSecurityMode,	// 0n70
	SystemWatchdogTimerHandler,	// 0n71
	SystemWatchdogTimerInformation,	// 0n72
	SystemLogicalProcessorInformation,	// 0n73
	SystemWow64SharedInformationObsolete,	// 0n74
	SystemRegisterFirmwareTableInformationHandler,	// 0n75
	SystemFirmwareTableInformation,	// 0n76
	SystemModuleInformationEx,	// 0n77
	SystemVerifierTriageInformation,	// 0n78
	SystemSuperfetchInformation,	// 0n79
	SystemMemoryListInformation,	// 0n80
	SystemFileCacheInformationEx,	// 0n81
	SystemThreadPriorityClientIdInformation,	// 0n82
	SystemProcessorIdleCycleTimeInformation,	// 0n83
	SystemVerifierCancellationInformation,	// 0n84
	SystemProcessorPowerInformationEx,	// 0n85
	SystemRefTraceInformation,	// 0n86
	SystemSpecialPoolInformation,	// 0n87
	SystemProcessIdInformation,	// 0n88
	SystemErrorPortInformation,	// 0n89
	SystemBootEnvironmentInformation,	// 0n90
	SystemHypervisorInformation,	// 0n91
	SystemVerifierInformationEx,	// 0n92
	SystemTimeZoneInformation,	// 0n93
	SystemImageFileExecutionOptionsInformation,	// 0n94
	SystemCoverageInformation,	// 0n95
	SystemPrefetchPatchInformation,	// 0n96
	SystemVerifierFaultsInformation,	// 0n97
	SystemSystemPartitionInformation,	// 0n98
	SystemSystemDiskInformation,	// 0n99
	SystemProcessorPerformanceDistribution,	// 0n100
	SystemNumaProximityNodeInformation,	// 0n101
	SystemDynamicTimeZoneInformation,	// 0n102
	SystemCodeIntegrityInformation,	// 0n103
	SystemProcessorMicrocodeUpdateInformation,	// 0n104
	SystemProcessorBrandString,	// 0n105
	SystemVirtualAddressInformation,	// 0n106
	SystemLogicalProcessorAndGroupInformation,	// 0n107
	SystemProcessorCycleTimeInformation,	// 0n108
	SystemStoreInformation,	// 0n109
	SystemRegistryAppendString,	// 0n110
	SystemAitSamplingValue,	// 0n111
	SystemVhdBootInformation,	// 0n112
	SystemCpuQuotaInformation,	// 0n113
	SystemNativeBasicInformation,	// 0n114
	SystemErrorPortTimeouts,	// 0n115
	SystemLowPriorityIoInformation,	// 0n116
	SystemBootEntropyInformation,	// 0n117
	SystemVerifierCountersInformation,	// 0n118
	SystemPagedPoolInformationEx,	// 0n119
	SystemSystemPtesInformationEx,	// 0n120
	SystemNodeDistanceInformation,	// 0n121
	SystemAcpiAuditInformation,	// 0n122
	SystemBasicPerformanceInformation,	// 0n123
	SystemQueryPerformanceCounterInformation,	// 0n124
	SystemSessionBigPoolInformation,	// 0n125
	SystemBootGraphicsInformation,	// 0n126
	SystemScrubPhysicalMemoryInformation,	// 0n127
	SystemBadPageInformation,	// 0n128
	SystemProcessorProfileControlArea,	// 0n129
	SystemCombinePhysicalMemoryInformation,	// 0n130
	SystemEntropyInterruptTimingInformation,	// 0n131
	SystemConsoleInformation,	// 0n132
	SystemPlatformBinaryInformation,	// 0n133
	SystemPolicyInformation,	// 0n134
	SystemHypervisorProcessorCountInformation,	// 0n135
	SystemDeviceDataInformation,	// 0n136
	SystemDeviceDataEnumerationInformation,	// 0n137
	SystemMemoryTopologyInformation,	// 0n138
	SystemMemoryChannelInformation,	// 0n139
	SystemBootLogoInformation,	// 0n140
	SystemProcessorPerformanceInformationEx,	// 0n141
	SystemCriticalProcessErrorLogInformation,	// 0n142
	SystemSecureBootPolicyInformation,	// 0n143
	SystemPageFileInformationEx,	// 0n144
	SystemSecureBootInformation,	// 0n145
	SystemEntropyInterruptTimingRawInformation,	// 0n146
	SystemPortableWorkspaceEfiLauncherInformation,	// 0n147
	SystemFullProcessInformation,	// 0n148
	SystemKernelDebuggerInformationEx,	// 0n149
	SystemBootMetadataInformation,	// 0n150
	SystemSoftRebootInformation,	// 0n151
	SystemElamCertificateInformation,	// 0n152
	SystemOfflineDumpConfigInformation,	// 0n153
	SystemProcessorFeaturesInformation,	// 0n154
	SystemRegistryReconciliationInformation,	// 0n155
	SystemEdidInformation,	// 0n156
	SystemManufacturingInformation,	// 0n157
	SystemEnergyEstimationConfigInformation,	// 0n158
	SystemHypervisorDetailInformation,	// 0n159
	SystemProcessorCycleStatsInformation,	// 0n160
	SystemVmGenerationCountInformation,	// 0n161
	SystemTrustedPlatformModuleInformation,	// 0n162
	SystemKernelDebuggerFlags,	// 0n163
	SystemCodeIntegrityPolicyInformation,	// 0n164
	SystemIsolatedUserModeInformation,	// 0n165
	SystemHardwareSecurityTestInterfaceResultsInformation,	// 0n166
	SystemSingleModuleInformation,	// 0n167
	SystemAllowedCpuSetsInformation,	// 0n168
	SystemVsmProtectionInformation,	// 0n169
	SystemInterruptCpuSetsInformation,	// 0n170
	SystemSecureBootPolicyFullInformation,	// 0n171
	SystemCodeIntegrityPolicyFullInformation,	// 0n172
	SystemAffinitizedInterruptProcessorInformation,	// 0n173
	SystemRootSiloInformation,	// 0n174
	SystemCpuSetInformation,	// 0n175
	SystemCpuSetTagInformation,	// 0n176
	SystemWin32WerStartCallout,	// 0n177
	SystemSecureKernelProfileInformation,	// 0n178
	SystemCodeIntegrityPlatformManifestInformation,	// 0n179
	SystemInterruptSteeringInformation,	// 0n180
	SystemSupportedProcessorArchitectures,	// 0n181
	SystemMemoryUsageInformation,	// 0n182
	SystemCodeIntegrityCertificateInformation,	// 0n183
	SystemPhysicalMemoryInformation,	// 0n184
	SystemControlFlowTransition,	// 0n185
	SystemKernelDebuggingAllowed,	// 0n186
	SystemActivityModerationExeState,	// 0n187
	SystemActivityModerationUserSettings,	// 0n188
	SystemCodeIntegrityPoliciesFullInformation,	// 0n189
	SystemCodeIntegrityUnlockInformation,	// 0n190
	SystemIntegrityQuotaInformation,	// 0n191
	SystemFlushInformation,	// 0n192
	SystemProcessorIdleMaskInformation,	// 0n193
	SystemSecureDumpEncryptionInformation,	// 0n194
	SystemWriteConstraintInformation,	// 0n195
	SystemKernelVaShadowInformation,	// 0n196
	SystemHypervisorSharedPageInformation,	// 0n197
	SystemFirmwareBootPerformanceInformation,	// 0n198
	SystemCodeIntegrityVerificationInformation,	// 0n199
	SystemFirmwarePartitionInformation,	// 0n200
	SystemSpeculationControlInformation,	// 0n201
	SystemDmaGuardPolicyInformation,	// 0n202
	SystemEnclaveLaunchControlInformation,	// 0n203
	SystemWorkloadAllowedCpuSetsInformation,	// 0n204
	SystemCodeIntegrityUnlockModeInformation,	// 0n205
	SystemLeapSecondInformation,	// 0n206
	SystemFlags2Information,	// 0n207
	SystemSecurityModelInformation,	// 0n208
	SystemCodeIntegritySyntheticCacheInformation,	// 0n209
	SystemFeatureConfigurationInformation,	// 0n210
	SystemFeatureConfigurationSectionInformation,	// 0n211
	SystemFeatureUsageSubscriptionInformation,	// 0n212
	SystemSecureSpeculationControlInformation,	// 0n213
	SystemSpacesBootInformation,	// 0n214
	SystemFwRamdiskInformation,	// 0n215
	SystemWheaIpmiHardwareInformation,	// 0n216
	SystemDifSetRuleClassInformation,	// 0n217
	SystemDifClearRuleClassInformation,	// 0n218
	SystemDifApplyPluginVerificationOnDriver,	// 0n219
	SystemDifRemovePluginVerificationOnDriver,	// 0n220
	SystemShadowStackInformation,	// 0n221
	SystemBuildVersionInformation,	// 0n222
	SystemPoolLimitInformation,	// 0n223
	SystemCodeIntegrityAddDynamicStore,	// 0n224
	SystemCodeIntegrityClearDynamicStores,	// 0n225
	SystemPoolZeroingInformation,	// 0n227
	MaxSystemInfoClass	// 0n228
} SYSTEM_INFORMATION_CLASS, * PSYSTEM_INFORMATION_CLASS;

// LOAD DRIVER INFORMATION
typedef struct _SYSTEM_LOAD_GDI_DRIVER_INFORMATION {
	UNICODE_STRING		SysName;
	ULONG               DriverStart;
	PVOID               DriverInfo;
	PVOID               DriverEntry;
	PVOID               ExportDirectory;
	ULONG               SizeOfImage;
} SYSTEM_LOAD_GDI_DRIVER_INFORMATION, * PSYSTEM_LOAD_GDI_DRIVER_INFORMATION;

// SYSTEM MODULE
typedef struct _SYSTEM_MODULE {
	ULONG                Reserved1;
	ULONG                Reserved2;
	ULONG                Reserved3;
	ULONG                Reserved4;
	PVOID                ImageBaseAddress;
	ULONG                ImageSize;
	ULONG                Flags;
	USHORT               Id;
	USHORT               Rank;
	USHORT               w018;
	USHORT               NameOffset;
	UCHAR                Name[MAXIMUM_FILENAME_LENGTH];
} SYSTEM_MODULE, * PSYSTEM_MODULE;

// SYSTEM MODULE INFORMATION
typedef struct _SYSTEM_MODULE_INFORMATION {
	ULONG                ModulesCount;
	SYSTEM_MODULE        Modules[0];
} SYSTEM_MODULE_INFORMATION, * PSYSTEM_MODULE_INFORMATION;

// ldr entry
typedef struct _KLDR_DATA_TABLE_ENTRY {
	LIST_ENTRY InLoadOrderLinks;
	PVOID64 ExceptionTable;
	USHORT ExceptionTableSize;
	PVOID64 GpValue;
	USHORT NonPagedDebugInfo;
	PVOID64 DllBase;
	PVOID64 EntryPoint;
	INT64 SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	UCHAR u1;
	PVOID64 SectionPointer;
	ULONG CheckSum;
	ULONG CoverageSectionSize;
	PVOID64 CoverageSection;
	PVOID64 LoadedImports;
	PVOID64 Spare;
	ULONG SizeOfImageNotRounded;
	ULONG TimeDateStamp;
} KLDR_DATA_TABLE_ENTRY, * PKLDR_DATA_TABLE_ENTRY;

// PATCHCILOAD ENTRY
typedef struct _PATCHCILOAD_ENTRY
{
	PCWSTR sysName;
	INT loadMode;
}PATCHCILOAD_ENTRY, * PPATCHCILOAD_ENTRY;


NTSYSAPI
NTSTATUS
NTAPI ZwQuerySystemInformation(
	IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
	IN OUT PVOID SystemInformation,
	IN ULONG SystemInformationLength,
	OUT OPTIONAL PULONG ReturnLength
);

NTSYSAPI
NTSTATUS
NTAPI ZwSetSystemInformation(
	IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
	IN PVOID					SystemInformation,
	IN ULONG					SystemInformationLength
);


NTSTATUS MajorHandle(PDEVICE_OBJECT pDriverObject, PIRP pIrp);
NTSTATUS DriverUnload(PDRIVER_OBJECT pDriverObject);
NTSTATUS DriverControl(PDEVICE_OBJECT pDriverObject, PIRP pIrp);
NTSTATUS Loading(UNICODE_STRING pSys);
NTSTATUS LoadingEx(UNICODE_STRING pSys);
NTSTATUS PatchCiLoad(UNICODE_STRING pSys, INT loadMode);
NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject, UNICODE_STRING pSysName);
VOID HasStarted(PDRIVER_OBJECT pDriverObject);