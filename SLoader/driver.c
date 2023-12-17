#include "driver.h"

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegPath)
{
	DbgPrint("[SLoader] START\n");

	for (size_t i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; ++i)
	{
		pDriverObject->MajorFunction[i] = MajorHandle;
	}
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DriverControl;

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

		NTSTATUS status = Loading((PCWSTR)(pBuff));

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

NTSTATUS LoadingEx(PCWSTR pSys)
{
	NTSTATUS status = STATUS_SUCCESS;
	KdPrint(("[SLoader] LoadEx: %S\n", pSys));

	WCHAR _pSys[260] = { 0 };
	wcscpy_s(_pSys, 260, L"\\??\\");
	wcscat_s(_pSys, 260, pSys);

	SYSTEM_LOAD_GDI_DRIVER_INFORMATION DriverInfo;
	RtlInitUnicodeString(&(DriverInfo.SysName), _pSys);
	status = ZwSetSystemInformation(SystemExtendServiceTableInformation, &DriverInfo, sizeof(SYSTEM_LOAD_GDI_DRIVER_INFORMATION));
	if (!NT_SUCCESS(status))
	{
		KdPrint(("[SLoader] LoadEx Fail, Status: %d\n", status));
		return status;
	}

	KdPrint(("[SLoader] LoadEx Successful\n"));
	return status;
}

NTSTATUS Loading(PCWSTR pSys)
{
	NTSTATUS status = STATUS_SUCCESS;
	KdPrint(("[SLoader] Load: %S\n", pSys));

	WCHAR _pSys[260] = { 0 };
	wcscpy_s(_pSys, 260, L"\\??\\");
	wcscat_s(_pSys, 260, pSys);

	SYSTEM_LOAD_GDI_DRIVER_INFORMATION DriverInfo;
	RtlInitUnicodeString(&(DriverInfo.SysName), _pSys);
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