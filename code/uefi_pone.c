
#include "pone_shared.h"
#include "pone_shared.c"

#include "uefi_pone.h"

#include "uefi_print.h"
#include "uefi_print.c"

efi_status EFI_API EntryUEFI(efi_handle ImageHandle, efi_system_table* SystemTable)
{
    (void) ImageHandle;

    efi_simple_text_input_protocol*  ConIn  = SystemTable->ConIn;
    efi_simple_text_output_protocol* ConOut = SystemTable->ConOut;

    efi_runtime_services* RuntimeServices   = SystemTable->RuntimeServices;
    efi_boot_services*    BootServices      = SystemTable->BootServices;

    usize DefaultTextAttribute    = EFITextAttribute(EFI_LIGHTGRAY, EFI_BLACK);
    usize EmphasizedTextAttribute = EFITextAttribute(EFI_LIGHTRED,  EFI_BLACK);

    // NOTE(vak): Clear screen
    {
        SetPrintColor(ConOut, DefaultTextAttribute);
        ClearScreen  (ConOut);
    }

    // NOTE(vak): Obtain memory map
    {
        usize MemoryMapSize     = 0;
        usize DescriptorSize    = sizeof(efi_memory_descriptor);
        u32   DescriptorVersion = 0;

        BootServices->GetMemoryMap(
            &MemoryMapSize,
            0,
            0,
            &DescriptorSize,
            &DescriptorVersion
        );

        u8* MemoryDescriptors = 0;

        BootServices->AllocatePool(
            EfiLoaderData,
            MemoryMapSize,
            &MemoryDescriptors
        );

        usize MemoryMapKey = 0;

        BootServices->GetMemoryMap(
            &MemoryMapSize,
            (efi_memory_descriptor*)MemoryDescriptors,
            &MemoryMapKey,
            &DescriptorSize,
            &DescriptorVersion
        );

        usize DescriptorCount = MemoryMapSize / DescriptorSize;

        persist string MemoryTypeNames[EfiMaxMemoryType] =
        {
            [EfiReservedMemoryType]      = StaticStr("ReservedMemory"),
            [EfiLoaderCode]              = StaticStr("LoaderCode"),
            [EfiLoaderData]              = StaticStr("LoaderData"),
            [EfiBootServicesCode]        = StaticStr("BootServicesCode"),
            [EfiBootServicesData]        = StaticStr("BootServicesData"),
            [EfiRuntimeServicesCode]     = StaticStr("RuntimeServicesCode"),
            [EfiRuntimeServicesData]     = StaticStr("RuntimeServicesData"),
            [EfiConventionalMemory]      = StaticStr("ConventionalMemory"),
            [EfiUnusableMemory]          = StaticStr("UnusableMemory"),
            [EfiACPIReclaimMemory]       = StaticStr("ACPIReclaimMemory"),
            [EfiACPIMemoryNVS]           = StaticStr("ACPIMemoryNVS"),
            [EfiMemoryMappedIO]          = StaticStr("MemoryMappedIO"),
            [EfiMemoryMappedIOPortSpace] = StaticStr("MemoryMappedIOPortSpace"),
            [EfiPalCode]                 = StaticStr("PalCode"),
            [EfiPersistentMemory]        = StaticStr("PersistentMemory"),
            [EfiUnacceptedMemoryType]    = StaticStr("UnacceptedMemory"),
        };

        Print       (ConOut, Str("Memory map: "));
        PrintNewLine(ConOut);

        usize IndexPadding =
            (DescriptorCount >=     0) +
            (DescriptorCount >=    10) +
            (DescriptorCount >=   100) +
            (DescriptorCount >=  1000) +
            (DescriptorCount >= 10000)
        ;

        for (usize Index = 0; Index < DescriptorCount; Index++)
        {
            efi_memory_descriptor* Descriptor = (efi_memory_descriptor*)
                (MemoryDescriptors + Index*DescriptorSize);

            if (Descriptor->Type == EfiConventionalMemory)
            {
                PrintSpaces (ConOut, 4);
                Print       (ConOut, Str("["));
                PrintUSize  (ConOut, Index, PrintBase_Dec);

                Print       (ConOut, Str("]: Type="));
                Print       (ConOut, MemoryTypeNames[Descriptor->Type]);

                Print       (ConOut, Str(", Physical="));
                PrintUSize  (ConOut, Descriptor->PhysicalStart, PrintBase_Hex);

                Print       (ConOut, Str(", Virtual="));
                PrintUSize  (ConOut, Descriptor->VirtualStart, PrintBase_Hex);

                Print       (ConOut, Str(", PageCount="));
                PrintUSize  (ConOut, Descriptor->NumberOfPages, PrintBase_Dec);

                PrintNewLine(ConOut);
            }
        }
    }

    PrintNewLine (ConOut);

    // NOTE(vak): "Hello, world!"
    {
        Print        (ConOut, Str("Hello, world!"));
        PrintNewLine (ConOut);
    }

    PrintNewLine (ConOut);

    // NOTE(vak): PrintUSize test
    {
        Print(ConOut, Str("PrintUSize test:\r\n"));

        PrintSpaces  (ConOut, 4);
        Print        (ConOut, Str("Binary: "));
        PrintUSize   (ConOut, 1249812048, PrintBase_Bin);
        PrintNewLine (ConOut);

        PrintSpaces  (ConOut, 4);
        Print        (ConOut, Str("Decimal: "));
        PrintUSize   (ConOut, 1249812048, PrintBase_Dec);
        PrintNewLine (ConOut);

        PrintSpaces  (ConOut, 4);
        Print        (ConOut, Str("Octal: "));
        PrintUSize   (ConOut, 1249812048, PrintBase_Oct);
        PrintNewLine (ConOut);

        PrintSpaces  (ConOut, 4);
        Print        (ConOut, Str("Hexadecimal: "));
        PrintUSize   (ConOut, 1249812048, PrintBase_Hex);
        PrintNewLine (ConOut);
    }

    PrintNewLine (ConOut);

    // NOTE(vak): PrintSSize test
    {
        Print(ConOut, Str("PrintSSize test:\r\n"));

        PrintSpaces  (ConOut, 4);
        Print        (ConOut, Str("Binary: "));
        PrintSSize   (ConOut, -1249812048, PrintBase_Bin);
        PrintNewLine (ConOut);

        PrintSpaces  (ConOut, 4);
        Print        (ConOut, Str("Decimal: "));
        PrintSSize   (ConOut, -1249812048, PrintBase_Dec);
        PrintNewLine (ConOut);

        PrintSpaces  (ConOut, 4);
        Print        (ConOut, Str("Octal: "));
        PrintSSize   (ConOut, -1249812048, PrintBase_Oct);
        PrintNewLine (ConOut);

        PrintSpaces  (ConOut, 4);
        Print        (ConOut, Str("Hexadecimal: "));
        PrintSSize   (ConOut, -1249812048, PrintBase_Hex);
        PrintNewLine (ConOut);
    }

    PrintNewLine (ConOut);

    // NOTE(vak): Press [ESC] to shutdown...
    {
        Print        (ConOut, Str("Press [ESC] to "));
        SetPrintColor(ConOut, EmphasizedTextAttribute);
        Print        (ConOut, Str("shutdown"));
        SetPrintColor(ConOut, DefaultTextAttribute);
        PrintNewLine (ConOut);

        efi_input_key InputKey = {0};

        for (;;)
        {
            while (ConIn->ReadKeyStroke(ConIn, &InputKey) == EFI_NOT_READY) {}

            if (InputKey.ScanCode == 0x17)
                break;
        }

        RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, 0);
    }

    return (EFI_SUCCESS);
}
