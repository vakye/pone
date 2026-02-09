
#include "pone_shared.h"
#include "pone_shared.c"

#include "pone_string.h"
#include "pone_string.c"

#include "uefi_pone.h"

#include "uefi_print.h"
#include "uefi_print.c"

#include "uefi_memory.h"
#include "uefi_memory.c"

efi_status EFI_API EntryUEFI(efi_handle ImageHandle, efi_system_table* SystemTable)
{
    efi_simple_text_input_protocol*  ConIn  = SystemTable->ConIn;
    efi_simple_text_output_protocol* ConOut = SystemTable->ConOut;

    efi_runtime_services* RuntimeServices   = SystemTable->RuntimeServices;
    efi_boot_services*    BootServices      = SystemTable->BootServices;

    usize DefaultTextAttribute    = EFITextAttribute(EFI_LIGHTGRAY, EFI_BLACK);
    usize EmphasizedTextAttribute = EFITextAttribute(EFI_LIGHTRED,  EFI_BLACK);

    // NOTE(vak): Setup ConOut
    {
        usize Columns = 0;
        usize Rows = 0;

        if (ConOut->QueryMode(ConOut, 1, &Columns, &Rows) == EFI_SUCCESS)
        {
            ConOut->SetMode(ConOut, 1);
        }

        EFISetPrintColor(ConOut, DefaultTextAttribute);
        EFIClearScreen(ConOut);
    }

    // NOTE(vak): "Hello, world!"
    {
        EFIDebugf(ConOut, Str("Hello, world!\r\n"));
    }

    // NOTE(vak): Locate graphics output protocol

    efi_graphics_output_protocol* GOP = 0;

    {
        efi_guid GUID = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

        efi_status LocateStatus = BootServices->LocateProtocol(
            &GUID,
            0,
            &GOP
        );

        LocateStatus = EFIStatusUnsetHighBit(LocateStatus);

        if (LocateStatus == EFI_SUCCESS)
        {
            EFIDebugf(ConOut, Str("Successfully located Graphics Output Protocol.\r\n"));
        }
        else if (LocateStatus == EFI_INVALID_PARAMETER)
        {
            EFIErrorf(ConOut, Str("Invalid parameter passed to LocateProtocol().\r\n"));
        }
        else if (LocateStatus == EFI_NOT_FOUND)
        {
            EFIErrorf(ConOut, Str("Graphics Output Protocol is not available.\r\n"));
        }
    }

    // NOTE(vak): Find a suitable video mode and set it as the
    // current video mode

    if (GOP)
    {
        u32 MaxMode = GOP->Mode->MaxMode;

        usize InfoSize = sizeof(efi_graphics_output_mode_information);

        efi_graphics_output_mode_information* Info = EFIAllocatePool(
            ConOut,
            BootServices,
            InfoSize
        );

        u32 DesiredSizeX = 1920;
        u32 DesiredSizeY = 1200;

        EFIDebugf(ConOut, Str("Video mode count: %u32\r\n"), MaxMode + 1);
        EFIDebugf(ConOut, Str("Desired video mode:\r\n"));
        EFIDebugf(ConOut, Str("    + SizeX:        %u32\r\n"), DesiredSizeX);
        EFIDebugf(ConOut, Str("    + SizeY:        %u32\r\n"), DesiredSizeY);
        EFIDebugf(ConOut, Str("    + Pixel format: RGBA8 or BGRA8\r\n"));

        u32 ChosenMode = MaxMode + 1;

        for (u32 Mode = 0; Mode <= MaxMode; Mode++)
        {
            efi_status QueryStatus = GOP->QueryMode(
                GOP,
                Mode,
                &InfoSize,
                &Info
            );

            if (QueryStatus == EFI_DEVICE_ERROR)
            {
                EFIErrorf(ConOut, Str("Device error ocurred when trying to query video mode (%u32).\r\n"), Mode);
                continue;
            }
            else if (QueryStatus == EFI_INVALID_PARAMETER)
            {
                EFIErrorf(ConOut, Str("Video mode number (%u32) is not valid.\r\n"), Mode);
                continue;
            }

            if ((Info->HorzResolution != DesiredSizeX) || (Info->VertResolution != DesiredSizeY))
            {
                continue;
            }

            if (
                (Info->PixelFormat != PixelRedGreenBlueReserved8BitPerColor) &&
                (Info->PixelFormat != PixelBlueGreenRedReserved8BitPerColor)
            )
            {
                continue;
            }

            EFIDebugf(ConOut, Str("Found desired video mode: (%u32)\r\n"), Mode);
            ChosenMode = Mode;
            break;
        }

        if (ChosenMode <= MaxMode)
        {
            string PixelFormat =
                (Info->PixelFormat == PixelRedGreenBlueReserved8BitPerColor) ?
                Str("RGBA8") :
                Str("BGRA8");

            EFIDebugf(ConOut, Str("Switching to video mode (%u32):\r\n"), ChosenMode);
            EFIDebugf(ConOut, Str("    + SizeX:        %u32\r\n"), Info->HorzResolution);
            EFIDebugf(ConOut, Str("    + SizeY:        %u32\r\n"), Info->VertResolution);
            EFIDebugf(ConOut, Str("    + Pixel format: %str\r\n"), PixelFormat);

            efi_status SetStatus = GOP->SetMode(
                GOP,
                ChosenMode
            );

            SetStatus = EFIStatusUnsetHighBit(SetStatus);

            if (SetStatus == EFI_DEVICE_ERROR)
            {
                EFIErrorf(ConOut, Str("Device error occured when trying to set video mode (%u32)\r\n"), ChosenMode);
            }
            else if (SetStatus == EFI_UNSUPPORTED)
            {
                 EFIErrorf(ConOut, Str("Video mode (%u32) is unsupported by device\r\n"), ChosenMode);
            }

            EFIRestorePrintBuffer(ConOut);
        }
        else
        {
            EFIErrorf(ConOut, Str("Failed to find the desired video mode.\r\n"));
        }
    }

    // NOTE(vak): Obtain memory map

    uefi_memory_map MemoryMap = EFIObtainMemoryMap(ConOut, BootServices);
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
