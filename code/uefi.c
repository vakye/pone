
#include "shared.h"
#include "shared.c"

#include "uefi.h"

#include "arch.h"
#include "arch_x64.c"

efi_status EFI_API UEFIBoot(efi_handle ImageHandle, efi_system_table* SystemTable)
{
    efi_simple_text_output_protocol*    ConOut          = SystemTable->ConOut;
    efi_boot_services*                  BootServices    = SystemTable->BootServices;

    ConOut->ClearScreen(ConOut);

    usize MemoryMapSize     = 0;
    void* MemoryMap         = 0;
    usize MemoryMapKey      = 0;
    usize DescriptorSize    = 0;
    u32   DescriptorVersion = 0;

    for (;;)
    {
        efi_status Status = BootServices->GetMemoryMap(
            &MemoryMapSize,
            MemoryMap,
            &MemoryMapKey,
            &DescriptorSize,
            &DescriptorVersion
        );

        if (Status == EFI_SUCCESS)
        {
            break;
        }
        else if (Status == EFI_BUFFER_TOO_SMALL)
        {
            usize NewSize = MemoryMapSize + 4*DescriptorSize;

            if (MemoryMap)
            {
                BootServices->FreePool(MemoryMap);
            }

            BootServices->AllocatePool(
                EfiLoaderData,
                NewSize,
                &MemoryMap
            );
        }
        else if (Status == EFI_INVALID_PARAMETER)
        {
            ConOut->OutputString(ConOut, L"Invalid parameter passed to BootServices->GetMemoryMap()\r\n");
        }
    }

    BootServices->ExitBootServices(ImageHandle, MemoryMapKey);

    DisableInterrupts();
    SetupSegments();

    for (;;);
}
