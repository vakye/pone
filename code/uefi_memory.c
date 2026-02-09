
local uefi_memory_map EFIObtainMemoryMap(
    efi_simple_text_output_protocol*    Debug,
    efi_boot_services*                  BootServices
)
{
    uefi_memory_map Result = {0};

    b32 Good = false;

    while (!Good)
    {
        efi_status Status = BootServices->GetMemoryMap(
            &Result.Size,
            (efi_memory_descriptor*)Result.Descriptors,
            &Result.Key,
            &Result.DescriptorSize,
            &Result.DescriptorVersion
        );

        Status = EFIStatusUnsetHighBit(Status);

        if (Status == EFI_SUCCESS)
        {
            Good = true;
            break;
        }
        else if (Status == EFI_BUFFER_TOO_SMALL)
        {
            EFIDebugf(Debug, Str("Memory map buffer is too small. Reallocating to a larger size...\r\n"));

            usize NewSize = Result.Size + 4 * Result.DescriptorSize;

            if (Result.Descriptors)
            {
                EFIFreePool(Debug, BootServices, Result.Descriptors);
            }

            Result.Descriptors = EFIAllocatePool(Debug, BootServices, NewSize);
        }
        else if (Status == EFI_INVALID_PARAMETER)
        {
            EFIErrorf(Debug, Str("Invalid parameter passed to GetMemoryMap().\r\n"));
            break;
        }
        else
        {
            EFIErrorf(Debug, Str("Unknown error reported from GetMemoryMap().\r\n"));
            break;
        }
    }

    if (!Good)
    {
        Result = (uefi_memory_map){0};
    }

    return (Result);
}

local void* EFIAllocatePool(
    efi_simple_text_output_protocol*    Debug,
    efi_boot_services*                  BootServices,
    usize                               Size
)
{
    void* Result = 0;

    efi_status PoolStatus = BootServices->AllocatePool(
        EfiLoaderData,
        Size,
        &Result
    );

    PoolStatus = EFIStatusUnsetHighBit(PoolStatus);

    if (PoolStatus == EFI_OUT_OF_RESOURCES)
    {
        EFIErrorf(Debug, Str("AllocatePool() ran out of memory.\r\n"));
    }
    else if (PoolStatus == EFI_INVALID_PARAMETER)
    {
        EFIErrorf(Debug, Str("Invalid parameter passed to AllocatePool().\r\n"));
    }

    return (Result);
}

local void EFIFreePool(
    efi_simple_text_output_protocol*    Debug,
    efi_boot_services*                  BootServices,
    void*                               Buffer
)
{
    efi_status FreeStatus = BootServices->FreePool(Buffer);

    FreeStatus = EFIStatusUnsetHighBit(FreeStatus);

    if (FreeStatus == EFI_INVALID_PARAMETER)
    {
        EFIErrorf(Debug, Str("Invalid parameter passed to FreePool().\r\n"));
    }
}
