
#include "pone_shared.h"
#include "pone_shared.c"

#include "uefi_pone.h"

#include "uefi_print.h"
#include "uefi_print.c"

typedef struct
{
    usize Size;
    u8*   Descriptors;
    usize Key;
    usize DescriptorSize;
    u32   DescriptorVersion;
} uefi_memory_map;

local uefi_memory_map UEFIObtainMemoryMap(efi_boot_services* BootServices)
{
    uefi_memory_map Result = {0};

    BootServices->GetMemoryMap(
        &Result.Size,
        (efi_memory_descriptor*)Result.Descriptors,
        &Result.Key,
        &Result.DescriptorSize,
        &Result.DescriptorVersion
    );

    BootServices->AllocatePool(
        EfiLoaderData,
        Result.Size,
        &Result.Descriptors
    );

    BootServices->GetMemoryMap(
        &Result.Size,
        (efi_memory_descriptor*)Result.Descriptors,
        &Result.Key,
        &Result.DescriptorSize,
        &Result.DescriptorVersion
    );

    return (Result);
}

efi_status EFI_API EntryUEFI(efi_handle ImageHandle, efi_system_table* SystemTable)
{
    (void) ImageHandle;

    efi_simple_text_input_protocol*  ConIn  = SystemTable->ConIn;
    efi_simple_text_output_protocol* ConOut = SystemTable->ConOut;

    efi_runtime_services* RuntimeServices   = SystemTable->RuntimeServices;
    efi_boot_services*    BootServices      = SystemTable->BootServices;

    usize DefaultTextAttribute    = EFITextAttribute(EFI_LIGHTGRAY, EFI_BLACK);
    usize EmphasizedTextAttribute = EFITextAttribute(EFI_LIGHTRED,  EFI_BLACK);

    uefi_memory_map MemoryMap = UEFIObtainMemoryMap(BootServices);

    // NOTE(vak): Setup ConOut
    {
        SetPrintColor(ConOut, DefaultTextAttribute);
        ClearScreen  (ConOut);
    }

    // NOTE(vak): "Hello, world!"
    {
        Print        (ConOut, Str("Hello, world!"));
        PrintNewLine (ConOut);
    }

    PrintNewLine (ConOut);

    // NOTE(vak): Detected conventional memory size
    {
        usize ConventionalMemorySize = 0;

        usize DescriptorCount = MemoryMap.Size / MemoryMap.DescriptorSize;

        for (usize Index = 0; Index < DescriptorCount; Index++)
        {
            efi_memory_descriptor* Descriptor = (efi_memory_descriptor*)
                (MemoryMap.Descriptors + Index * MemoryMap.DescriptorSize);

            if (Descriptor->Type == EfiConventionalMemory)
                ConventionalMemorySize += Descriptor->NumberOfPages * 4096;
        }

        Print(ConOut, Str("Detected conventional memory size: "));

        usize  Integer = 0;
        usize  Decimal = 0;
        usize  Shift   = 0;

        if (0) {}
        else if (ConventionalMemorySize >= TB(1)) Shift = 40;
        else if (ConventionalMemorySize >= GB(1)) Shift = 30;
        else if (ConventionalMemorySize >= MB(1)) Shift = 20;
        else if (ConventionalMemorySize >= KB(1)) Shift = 10;

        Integer = ConventionalMemorySize >> Shift;
        Decimal = (10*ConventionalMemorySize >> Shift) - 10*Integer;

        PrintUSize(ConOut, Integer, PrintBase_Dec);
        Print     (ConOut, Str("."));
        PrintUSize(ConOut, Decimal, PrintBase_Dec);

        switch (Shift)
        {
            case 1:  break;
            case 10: Print(ConOut, Str("KB")); break;
            case 20: Print(ConOut, Str("MB")); break;
            case 30: Print(ConOut, Str("GB")); break;
            case 40: Print(ConOut, Str("TB")); break;
        }

        PrintNewLine(ConOut);
    }

    PrintNewLine(ConOut);

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
