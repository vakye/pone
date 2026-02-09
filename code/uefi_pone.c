
#include "pone_shared.h"
#include "pone_shared.c"

#include "pone_string.h"
#include "pone_string.c"

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

local uefi_memory_map UEFIObtainMemoryMap(
    efi_simple_text_output_protocol* ConOut,
    efi_boot_services* BootServices
)
{
    uefi_memory_map Result = {0};

    for (;;)
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
            EFIDebugf(ConOut, Str("Successfully obtained memory map.\r\n"));
            break;
        }
        else if (Status == EFI_BUFFER_TOO_SMALL)
        {
            EFIDebugf(ConOut, Str("Memory map buffer is too small. Reallocating to a larger size...\r\n"));

            usize NewSize = Result.Size + 4 * Result.DescriptorSize;

            if (Result.Descriptors)
            {
                BootServices->FreePool(Result.Descriptors);
            }

            efi_status PoolStatus = BootServices->AllocatePool(
                EfiLoaderData,
                NewSize,
                &Result.Descriptors
            );

            PoolStatus = EFIStatusUnsetHighBit(PoolStatus);

            if (PoolStatus != EFI_SUCCESS)
            {
                EFIErrorf(ConOut, Str("Failed to allocate memory map buffer due to insufficient pool memory.\r\n"));
                break;
            }
        }
        else if (Status == EFI_INVALID_PARAMETER)
        {
            EFIErrorf(ConOut, Str("Invalid parameter passed to GetMemoryMap().\r\n"));
            break;
        }
        else
        {
            EFIErrorf(ConOut, Str("Unknown error reported from GetMemoryMap().\r\n"));
            break;
        }
    }

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

    // NOTE(vak): Setup ConOut
    {
        EFISetPrintColor(ConOut, DefaultTextAttribute);
        EFIClearScreen(ConOut);
    }

    // NOTE(vak): "Hello, world!"
    {
        EFIDebugf(ConOut, Str("Hello, world!\r\n"));
    }

    // NOTE(vak): Obtain the memory map from boot services

    uefi_memory_map MemoryMap = UEFIObtainMemoryMap(ConOut, BootServices);
    usize DescriptorCount = MemoryMap.Size / MemoryMap.DescriptorSize;

    // NOTE(vak): Memory map info
    {
        EFIDebugf(ConOut, Str("Memory map info:\r\n"));
        EFIDebugf(ConOut, Str("    + Total size (bytes): %u64\r\n"), MemoryMap.Size);
        EFIDebugf(ConOut, Str("    + Descriptor count:   %u64\r\n"), DescriptorCount);
        EFIDebugf(ConOut, Str("    + Descriptor size:    %u64\r\n"), MemoryMap.DescriptorSize);
        EFIDebugf(ConOut, Str("    + Key:                %u64\r\n"), MemoryMap.Key);
    }

    // NOTE(vak): Detected conventional memory size
    {
        usize ConventionalMemorySize = 0;
        usize ConventionalMemoryCount = 0;

        for (usize Index = 0; Index < DescriptorCount; Index++)
        {
            efi_memory_descriptor* Descriptor = (efi_memory_descriptor*)
                (MemoryMap.Descriptors + Index * MemoryMap.DescriptorSize);

            if (Descriptor->Type == EfiConventionalMemory)
            {
                usize RegionSize = Descriptor->NumberOfPages * 4096;

                usize  SizeInteger = 0;
                usize  SizeDecimal = 0;
                string SizePostfix = Str("");
                usize  Shift = 0;

                if (0) {}
                else if (RegionSize >= TB(1)) { Shift = 40; SizePostfix = Str("TB"); }
                else if (RegionSize >= GB(1)) { Shift = 30; SizePostfix = Str("GB"); }
                else if (RegionSize >= MB(1)) { Shift = 20; SizePostfix = Str("MB"); }
                else if (RegionSize >= KB(1)) { Shift = 10; SizePostfix = Str("KB"); }

                SizeInteger = RegionSize >> Shift;
                SizeDecimal = (10*RegionSize >> Shift) - 10*SizeInteger;

                // TODO(vak): Support padding so we don't have to do it manually.

                usize Printed = 0;

                EFIDebugf(ConOut, Str("Conventional region ["));

                Printed = EFIPrintf(ConOut, Str("%u32"), ConventionalMemoryCount);
                while (Printed < 2)
                {
                    EFIPrintf(ConOut, Str(" "));
                    Printed++;
                }

                EFIPrintf(ConOut, Str("]: Size = "));

                Printed = EFIPrintf(ConOut, Str("%u64.%u64%"),
                    SizeInteger,
                    SizeDecimal
                );

                while (Printed < 6)
                {
                    EFIPrintf(ConOut, Str(" "));
                    Printed++;
                }

                EFIPrintf(ConOut, SizePostfix);

                EFIPrintf(ConOut, Str(", Attributes = "));

                if (Descriptor->Attribute & EFI_MEMORY_UC ) EFIPrintf(ConOut, Str("UC|"));
                if (Descriptor->Attribute & EFI_MEMORY_WC ) EFIPrintf(ConOut, Str("WC|"));
                if (Descriptor->Attribute & EFI_MEMORY_WT ) EFIPrintf(ConOut, Str("WT|"));
                if (Descriptor->Attribute & EFI_MEMORY_WB ) EFIPrintf(ConOut, Str("WB|"));
                if (Descriptor->Attribute & EFI_MEMORY_UCE) EFIPrintf(ConOut, Str("UCE|"));
                if (Descriptor->Attribute & EFI_MEMORY_WP ) EFIPrintf(ConOut, Str("WP|"));
                if (Descriptor->Attribute & EFI_MEMORY_RP ) EFIPrintf(ConOut, Str("RP|"));
                if (Descriptor->Attribute & EFI_MEMORY_XP ) EFIPrintf(ConOut, Str("XP|"));
                if (Descriptor->Attribute & EFI_MEMORY_NV ) EFIPrintf(ConOut, Str("NV|"));

                if (Descriptor->Attribute & EFI_MEMORY_MORE_RELIABLE)
                    EFIPrintf(ConOut, Str("MR|"));

                if (Descriptor->Attribute & EFI_MEMORY_RO) EFIPrintf(ConOut, Str("RO|"));
                if (Descriptor->Attribute & EFI_MEMORY_SP) EFIPrintf(ConOut, Str("SP|"));

                if (Descriptor->Attribute & EFI_MEMORY_CPU_CRYPTO)
                    EFIPrintf(ConOut, Str("CC|"));

                if (Descriptor->Attribute & EFI_MEMORY_RUNTIME)
                    EFIPrintf(ConOut, Str("RT|"));

                EFIPrintf(ConOut, Str("\r\n"));

                ConventionalMemorySize += Descriptor->NumberOfPages * 4096;
                ConventionalMemoryCount++;
            }
        }

        usize  Integer   = 0;
        usize  Decimal   = 0;
        usize  Shift     = 0;
        string Postfix   = NilString;

        if (0) {}
        else if (ConventionalMemorySize >= TB(1)) { Shift = 40; Postfix = Str("TB"); }
        else if (ConventionalMemorySize >= GB(1)) { Shift = 30; Postfix = Str("GB"); }
        else if (ConventionalMemorySize >= MB(1)) { Shift = 20; Postfix = Str("MB"); }
        else if (ConventionalMemorySize >= KB(1)) { Shift = 10; Postfix = Str("KB"); }

        Integer = ConventionalMemorySize >> Shift;
        Decimal = (10*ConventionalMemorySize >> Shift) - 10*Integer;

        EFIDebugf(
            ConOut,
            Str("Total conventional memory size: %u64.%u64%str\r\n"),
            Integer,
            Decimal,
            Postfix
        );
    }

    EFIPrintf(ConOut, Str("\r\n"));

    // NOTE(vak): Press [ESC] to shutdown...
    {
        EFIPrintf(ConOut, Str("Press [ESC] to "));

        EFISetPrintColor(ConOut, EmphasizedTextAttribute);
        EFIPrintf(ConOut, Str("shutdown"));

        EFISetPrintColor(ConOut, DefaultTextAttribute);
        EFIPrintf(ConOut, Str("\r\n"));

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
