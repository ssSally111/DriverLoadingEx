#include "driver.h"

static SYSTEM_MODULE CiModule = { 0 };
static UINT64 g_CiFlagAddr = 0;



NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegPath)
{
	DbgPrint("[SLoader] START\n");

	for (size_t i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; ++i)
	{
		pDriverObject->MajorFunction[i] = MajorHandle;
	}
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DriverControl;
	pDriverObject->DriverUnload = DriverUnload;

	NTSTATUS status = STATUS_SUCCESS;

	PDEVICE_OBJECT pDevice;
	UNICODE_STRING SymLinkName;
	UNICODE_STRING DeviceName;

	RtlInitUnicodeString(&DeviceName, DEVICE_NAME);
	status = IoCreateDevice(pDriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, 0, TRUE, &pDevice);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("[SLoader] Create Device Failed, Status:%d\n", status);
		return status;
	}

	RtlInitUnicodeString(&SymLinkName, SYM_LINK_NAME);
	status = IoCreateSymbolicLink(&SymLinkName, &DeviceName);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("[SLoader] Create SymbolicLink Failed, Status:%d\n", status);
		IoDeleteDevice(pDevice);
		return status;
	}

	pDevice->Flags |= DO_BUFFERED_IO;

	HasStarted(pDriverObject);

	return status;
}

NTSTATUS MajorHandle(PDEVICE_OBJECT pDriverObject, PIRP pIrp)
{
	NTSTATUS status = STATUS_SUCCESS;

	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return status;
}

NTSTATUS DriverUnload(PDRIVER_OBJECT pDriverObject)
{
	UNICODE_STRING SymLinkName = RTL_CONSTANT_STRING(SYM_LINK_NAME);
	IoDeleteSymbolicLink(&SymLinkName);
	IoDeleteDevice(pDriverObject->DeviceObject);
	return STATUS_SUCCESS;
}

NTSTATUS DriverControl(PDEVICE_OBJECT pDriverObject, PIRP pIrp)
{
	NTSTATUS status = STATUS_SUCCESS;

	PIO_STACK_LOCATION pIrps = IoGetCurrentIrpStackLocation(pIrp);
	ULONG inLength = pIrps->Parameters.DeviceIoControl.InputBufferLength;
	ULONG optLength = pIrps->Parameters.DeviceIoControl.OutputBufferLength;
	ULONG CODE = pIrps->Parameters.DeviceIoControl.IoControlCode;
	ULONG info = 0;

	switch (CODE)
	{
	case LOADING:
	{
		PVOID pBuff = pIrp->AssociatedIrp.SystemBuffer;
		UNICODE_STRING uSys;
		__try {
			RtlInitUnicodeString(&uSys, (PCWSTR)pBuff);
			status = Loading(uSys);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			KdPrint(("[SLoader] FAIL ON LOADING\n"));
		}
		pIrps->Parameters.DeviceIoControl.OutputBufferLength = sizeof(status);
		memset(pBuff, status, sizeof(status));
		info = sizeof(status);
		break;
	}
	case LOADINGEX:
	{
		PVOID pBuff = pIrp->AssociatedIrp.SystemBuffer;
		UNICODE_STRING uSys;
		__try {
			RtlInitUnicodeString(&uSys, (PCWSTR)pBuff);
			status = LoadingEx(uSys);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			KdPrint(("[SLoader] FAIL ON LOADINGEX\n"));
		}
		pIrps->Parameters.DeviceIoControl.OutputBufferLength = sizeof(status);
		memset(pBuff, status, sizeof(status));
		info = sizeof(status);
		break;
	}
	case PATCHCILOAD:
	{
		PVOID pBuff = pIrp->AssociatedIrp.SystemBuffer;
		PPATCHCILOAD_ENTRY loadEntry = (PPATCHCILOAD_ENTRY)pBuff;
		UNICODE_STRING uSys;
		__try {
			RtlInitUnicodeString(&uSys, loadEntry->sysName);
			status = PatchCiLoad(uSys, loadEntry->loadMode);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			KdPrint(("[SLoader] FAIL ON PATCHCILOAD\n"));
		}
		pIrps->Parameters.DeviceIoControl.OutputBufferLength = sizeof(status);
		memset(pBuff, status, sizeof(status));
		info = sizeof(status);
		break;
	}
	case UNLOAD:
	{
		PVOID pBuff = pIrp->AssociatedIrp.SystemBuffer;
		UNICODE_STRING uSys;
		__try {
			RtlInitUnicodeString(&uSys, (PCWSTR)pBuff);
			status = UnloadDriver(pDriverObject->DriverObject, uSys);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			KdPrint(("[SLoader] FAIL ON UNLOAD\n"));
		}
		pIrps->Parameters.DeviceIoControl.OutputBufferLength = sizeof(status);
		memset(pBuff, status, sizeof(status));
		info = sizeof(status);
		break;
	}
	default:
		break;
	}

	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = info;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS GetModuleInfo(ANSI_STRING name, PSYSTEM_MODULE pModule)
{
	NTSTATUS status = STATUS_SUCCESS;
	ULONG length = 0;
	status = ZwQuerySystemInformation(SystemModuleInformation, NULL, 0, &length);
	if (status == STATUS_INFO_LENGTH_MISMATCH)
	{

		PVOID pSysModuleInfo = ExAllocatePool2(POOL_FLAG_NON_PAGED, length, ZW_QUERY_SYS_INFO);
		if (!pSysModuleInfo) {
			KdPrint(("[SLoader] GetModuleInfo ExAllocatePool Fail [size:%d], \
				There is not enough memory in the pool to satisfy the request...\n", length));
			return status;
		}

		status = ZwQuerySystemInformation(SystemModuleInformation, pSysModuleInfo, length, &length);
		if (!NT_SUCCESS(status))
		{
			ExFreePoolWithTag(pSysModuleInfo, ZW_QUERY_SYS_INFO);
			KdPrint(("[SLoader] GetModuleInfo QuerySystemInformation PSYSTEM_MODULE_INFORMATION Fail %d ...\n", status));
			return status;
		}

		ANSI_STRING ptName;
		SYSTEM_MODULE module;
		PSYSTEM_MODULE_INFORMATION pSystemModelInformation = (PSYSTEM_MODULE_INFORMATION)pSysModuleInfo;
		for (size_t i = 0; i < pSystemModelInformation->ModulesCount; i++)
		{
			module = pSystemModelInformation->Modules[i];
			RtlInitAnsiString(&ptName, module.Name + module.NameOffset);
			if (RtlEqualString(&name, &ptName, TRUE))
			{
				*pModule = module;
				break;
			}
		}

		ExFreePoolWithTag(pSysModuleInfo, ZW_QUERY_SYS_INFO);

		if (!pModule->ImageBaseAddress)
		{
			status = STATUS_OBJECT_NAME_NOT_FOUND;
		}
	}
	else {
		KdPrint(("[SLoader] GetModuleInfo QuerySystemInformation Init SystemModuleInformation Size Fail %d ...\n", status));
	}
	return status;
}

NTSTATUS PatchCiInitialize()
{
	NTSTATUS status = STATUS_SUCCESS;

	if (!CiModule.ImageBaseAddress)
	{
		ANSI_STRING name = RTL_CONSTANT_STRING("CI.dll");
		status = GetModuleInfo(name, &CiModule);
		if (!NT_SUCCESS(status))
		{
			KdPrint(("[SLoader] Get Module NOT_FOUND: %d\n", status));
			return status;
		}
	}

	if (!g_CiFlagAddr)
	{
		UCHAR i, r;
		UINT64 ImageEndAddress = (UINT64)CiModule.ImageBaseAddress + CiModule.ImageSize;
		for (UINT64 p = (UINT64)CiModule.ImageBaseAddress; p < ImageEndAddress - 7; p++)
		{
			r = TRUE;
			for (i = 0; i < CHECK_BLOCK_SIZE; i++)
			{
				if (!MmIsAddressValid((PVOID)(p + i))) {
					p += i;
					r = FALSE;
					break;
				}
			}

			if (r && _64R8U(p) == _64R8U(WIN10LTSC_21H2_19044_MARK)) {
				g_CiFlagAddr = p + (0xFFFFFFFF00000000 | (UINT64)_64R4U(p + 2)) + 6;
				if (!MmIsAddressValid((PVOID)g_CiFlagAddr))
				{
					status = STATUS_ACPI_INVALID_DATA;
					KdPrint(("[SLoader] INVALID ADDR 0x%llX: %d\n", g_CiFlagAddr, status));
					g_CiFlagAddr = 0;
				}
				return status;
			}
		}

		/*
		 * This doesn't support your machine.
		 *
		 * Only Support Version(Tested):
		 * WIN10LTSC_21H2_19044_MARK,
		 */
		status = STATUS_IMAGE_MACHINE_TYPE_MISMATCH_EXE;
		KdPrint(("[SLoader] MACHINE TYPE MISMATCH: %d\n", status));
	}

	return status;
}

NTSTATUS PatchCiLoad(UNICODE_STRING pSys, INT loadMode)
{
	NTSTATUS status = STATUS_SUCCESS;
	status = PatchCiInitialize();

	if (NT_SUCCESS(status))
	{
		// must IRQL < DISPATCH_LEVEL, need access paging.
		LONG cuMode = _64R4U(g_CiFlagAddr);
		_64R4U(g_CiFlagAddr) = 0;

		switch (loadMode)
		{
		case 1:
		{
			status = Loading(pSys);
			break;
		}
		case 2:
		{
			status = LoadingEx(pSys);
			break;
		}
		default:
			break;
		}

		_64R4U(g_CiFlagAddr) = cuMode;
	}

	return status;
}

NTSTATUS LoadingEx(UNICODE_STRING uSys)
{
	NTSTATUS status = STATUS_SUCCESS;
	KdPrint(("[SLoader] LoadEx: %wZ\n", uSys));

	UNICODE_STRING sys = RTL_CONSTANT_STRING(L"\\??\\");
	RtlAppendUnicodeStringToString(&sys, &uSys);

	SYSTEM_LOAD_GDI_DRIVER_INFORMATION DriverInfo;
	DriverInfo.SysName = sys;
	status = ZwSetSystemInformation(SystemExtendServiceTableInformation, &DriverInfo, sizeof(SYSTEM_LOAD_GDI_DRIVER_INFORMATION));
	if (!NT_SUCCESS(status))
	{
		KdPrint(("[SLoader] LoadEx Fail, Status: %d\n", status));
		return status;
	}

	KdPrint(("[SLoader] LoadEx Successful\n"));
	return status;
}

NTSTATUS Loading(UNICODE_STRING uSys)
{
	NTSTATUS status = STATUS_SUCCESS;
	KdPrint(("[SLoader] Load: %wZ\n", uSys));

	UNICODE_STRING sys = RTL_CONSTANT_STRING(L"\\??\\");
	RtlAppendUnicodeStringToString(&sys, &uSys);

	SYSTEM_LOAD_GDI_DRIVER_INFORMATION DriverInfo;
	DriverInfo.SysName = sys;
	status = ZwSetSystemInformation(SystemLoadGdiDriverInSystemSpace, &DriverInfo, sizeof(SYSTEM_LOAD_GDI_DRIVER_INFORMATION));
	if (!NT_SUCCESS(status))
	{
		KdPrint(("[SLoader] Load Fail, Status: %d\n", status));
		return status;
	}

	DRIVER_OBJECT      DriverObject;
	UNICODE_STRING     RegistryPath;
	status = ((FnDriverEntry)(DriverInfo.DriverEntry))(&DriverObject, &RegistryPath);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("[SLoader] Load Call FnDriverEntry Fail, Status: %d\n", status));
		NTSTATUS status2 = ZwSetSystemInformation(SystemUnloadGdiDriverInformation, DriverInfo.DriverInfo, 0x4);
		if (!NT_SUCCESS(status2))
		{
			KdPrint(("[SLoader] Load UnloadDriver Fail, Status: %d\n", status2));
		}
		return status;
	}

	KdPrint(("[SLoader] Load Successful\n"));
	return status;
}

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject, UNICODE_STRING sysName)
{
	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING suffix = RTL_CONSTANT_STRING(L".sys");
	RtlAppendUnicodeStringToString(&sysName, &suffix);
	PLIST_ENTRY tempEntry = ((PLIST_ENTRY)pDriverObject->DriverSection)->Flink;
	while (pDriverObject->DriverSection != tempEntry)
	{
		if (RtlEqualUnicodeString(&sysName, &((PKLDR_DATA_TABLE_ENTRY)tempEntry)->BaseDllName, FALSE))
		{
			SYSTEM_LOAD_GDI_DRIVER_INFORMATION DriverInfo;
			DriverInfo.SysName = ((PKLDR_DATA_TABLE_ENTRY)tempEntry)->FullDllName;
			status = ZwSetSystemInformation(SystemLoadGdiDriverInSystemSpace, &DriverInfo, sizeof(SYSTEM_LOAD_GDI_DRIVER_INFORMATION));
			if (status == STATUS_IMAGE_ALREADY_LOADED)
			{
				status = ZwSetSystemInformation(SystemUnloadGdiDriverInformation, DriverInfo.DriverInfo, 0x4);
				if (NT_SUCCESS(status))
				{
					KdPrint(("[SLoader] UnloadDriver Successful\n"));
					return status;
				}
			}

			KdPrint(("[SLoader] UnloadDriver Successful\n"));
			return status;
		}
		tempEntry = tempEntry->Flink;
	}

	return STATUS_SUCCESS;
}

VOID _HasStarted(PVOID pDriver)
{
	LARGE_INTEGER SpTime;
	SpTime.QuadPart = -100 * 1000 * 1000 * 2;
	KeDelayExecutionThread(KernelMode, 0, &SpTime);

	PDRIVER_OBJECT pDriverObject = (PDRIVER_OBJECT)pDriver;
	PLIST_ENTRY pModuleList = pDriverObject->DriverSection;

	pModuleList->Flink->Blink = pModuleList->Blink;
	pModuleList->Blink->Flink = pModuleList->Flink;

	pDriverObject->DriverSize = 0;
	pDriverObject->DriverSection = NULL;
	pDriverObject->DriverExtension = NULL;
	pDriverObject->DriverStart = NULL;
	pDriverObject->DriverInit = NULL;
	pDriverObject->FastIoDispatch = NULL;
	pDriverObject->DriverStartIo = NULL;
}

VOID HasStarted(PDRIVER_OBJECT pDriverObject)
{
	HANDLE hThread;
	PsCreateSystemThread(&hThread, 0, NULL, NULL, NULL, _HasStarted, pDriverObject);
	ZwClose(hThread);
}