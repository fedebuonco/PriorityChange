#include <ntddk.h>

// Prototypes
void PChangeUnload(PDRIVER_OBJECT DriverObject);
NTSTATUS PChangeCreateClose(PDRIVER_OBJECT DriverObject);
NTSTATUS PChangeWrite(PDRIVER_OBJECT DriverObject, PIRP Irp);

// DriverEntry
extern "C" NTSTATUS
DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {

	// Register the dispatch routines
	// Create and close handled by the same routine
	DriverObject->MajorFunction[IRP_MJ_CREATE] = PChangeCreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = PChangeCreateClose;
	DriverObject->MajorFunction[IRP_MJ_WRITE] = PChangeWrite;

	UNICODE_STRING devName = RTL_CONSTANT_STRING(L"\\Device\\PChange");
	PDEVICE_OBJECT DeviceObject;
	
	NTSTATUS status = IoCreateDevice(
		DriverObject,
		0,
		&devName,
		FILE_DEVICE_UNKNOWN,
		0,
		FALSE,
		&DeviceObject);
	if (!NT_SUCCESS(status)) 
	{
		KdPrint(("Failed"));
		return status;
	}

	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\PChange");
	status = IoCreateSymbolicLink(&symLink, &devName);
	if (!NT_SUCCESS(status)) {
		KdPrint("Failed");
		IoDeleteDevice(DeviceObject);
		return status;
	}


	return STATUS_SUCCESS; // Ready to accept requests
}

void PChangeUnload(PDRIVER_OBJECT DriverObject) {
	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\PChange");
	// Delete the link
	IoDeleteSymbolicLink(&symLink);
	// Delete the dev obj
	IoDeleteDevice(DriverObject->DeviceObject);
}


