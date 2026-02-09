
#pragma once

typedef struct
{
    usize Size;
    u8*   Descriptors;
    usize Key;
    usize DescriptorSize;
    u32   DescriptorVersion;
} uefi_memory_map;

local uefi_memory_map EFIObtainMemoryMap(
    efi_simple_text_output_protocol*    Debug,
    efi_boot_services*                  BootServices
);

local void* EFIAllocatePool(
    efi_simple_text_output_protocol*    Debug,
    efi_boot_services*                  BootServices,
    usize                               Size
);

local void EFIFreePool(
    efi_simple_text_output_protocol*    Debug,
    efi_boot_services*                  BootServices,
    void*                               Buffer
);
