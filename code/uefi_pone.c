
#include "pone_shared.h"
#include "pone_shared.c"

#include "pone_string.h"
#include "pone_string.c"

#include "pone_acpi.h"
#include "pone_acpi.c"

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

        usize BestArea = 0;
        u32 BestMode = 0;

        u32 Mode = 0;

        for (; Mode < 128; Mode++)
        {
            if (ConOut->QueryMode(ConOut, Mode, &Columns, &Rows) == EFI_SUCCESS)
            {
                usize Area = Columns * Rows;
                if (Area > BestArea)
                {
                    BestMode = Mode;
                    BestArea = Area;
                }
            }
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
        EFIInfof(ConOut, Str("Locating Graphics Output Protocol...  "));

        efi_guid GUID = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

        efi_status LocateStatus = BootServices->LocateProtocol(
            &GUID,
            0,
            &GOP
        );

        LocateStatus = EFIStatusUnsetHighBit(LocateStatus);

        if (LocateStatus == EFI_INVALID_PARAMETER)
        {
            EFIErrorf(ConOut, Str("Invalid parameter passed to LocateProtocol().\r\n"));
        }
        else if (LocateStatus == EFI_NOT_FOUND)
        {
            EFIErrorf(ConOut, Str("Graphics Output Protocol is not available.\r\n"));
        }

        if (LocateStatus == EFI_SUCCESS)
        {
            EFISetPrintColor(ConOut, EFITextAttribute(EFI_LIGHTGREEN, EFI_BLACK));
            EFIPrintf(ConOut, Str("SUCCESS"));
        }
        else
        {
            EFISetPrintColor(ConOut, EFITextAttribute(EFI_LIGHTRED, EFI_BLACK));
            EFIPrintf(ConOut, Str("FAILED"));
        }

        EFISetPrintColor(ConOut, EFITextAttribute(EFI_LIGHTGRAY, EFI_BLACK));
        EFIPrintf(ConOut, Str("\r\n"));
    }

    // NOTE(vak): Find a suitable video mode and set it as the
    // current video mode

    if (GOP)
    {
        u32 DesiredSizeX = 1920;
        u32 DesiredSizeY = 1200;
        efi_graphics_pixel_format DesiredPixelFormat = PixelBlueGreenRedReserved8BitPerColor;

        EFIInfof(
            ConOut,
            Str("Switching to video mode %u32x%u32, BGRA8... "),
            DesiredSizeX,
            DesiredSizeY
        );

        u32 MaxMode = GOP->Mode->MaxMode;

        usize InfoSize = sizeof(efi_graphics_output_mode_information);

        efi_graphics_output_mode_information* Info = EFIAllocatePool(
            ConOut,
            BootServices,
            InfoSize
        );

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

            if (Info->PixelFormat != DesiredPixelFormat)
            {
                continue;
            }

            EFIDebugf(ConOut, Str("Found desired video mode: (%u32)\r\n"), Mode);
            ChosenMode = Mode;
            break;
        }

        if (ChosenMode <= MaxMode)
        {
            EFIDebugf(ConOut, Str("Switching to video mode (%u32):\r\n"), ChosenMode);
            EFIDebugf(ConOut, Str("    + SizeX:        %u32\r\n"), Info->HorzResolution);
            EFIDebugf(ConOut, Str("    + SizeY:        %u32\r\n"), Info->VertResolution);
            EFIDebugf(ConOut, Str("    + Pixel format: BGRA8\r\n"));

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

        if (ChosenMode <= MaxMode)
        {
            EFISetPrintColor(ConOut, EFITextAttribute(EFI_LIGHTGREEN, EFI_BLACK));
            EFIPrintf(ConOut, Str("SUCCESS"));
        }
        else
        {
            EFISetPrintColor(ConOut, EFITextAttribute(EFI_LIGHTRED, EFI_BLACK));
            EFIPrintf(ConOut, Str("FAILED"));
        }

        EFISetPrintColor(ConOut, EFITextAttribute(EFI_LIGHTGRAY, EFI_BLACK));
        EFIPrintf(ConOut, Str("\r\n"));
    }

    // NOTE(vak): Obtain memory map

    EFIInfof(ConOut, Str("Obtaining memory map...  "));

    uefi_memory_map MemoryMap = EFIObtainMemoryMap(ConOut, BootServices);
    usize DescriptorCount = MemoryMap.Size / MemoryMap.DescriptorSize;

    b32 ValidMemoryMap = (MemoryMap.Size > 0);

    if (ValidMemoryMap)
    {
        EFISetPrintColor(ConOut, EFITextAttribute(EFI_LIGHTGREEN, EFI_BLACK));
        EFIPrintf(ConOut, Str("SUCCESS"));
    }
    else
    {
        EFISetPrintColor(ConOut, EFITextAttribute(EFI_LIGHTRED, EFI_BLACK));
        EFIPrintf(ConOut, Str("FAILED"));
    }

    EFISetPrintColor(ConOut, EFITextAttribute(EFI_LIGHTGRAY, EFI_BLACK));
    EFIPrintf(ConOut, Str("\r\n"));

    // NOTE(vak): Memory map info
    if (ValidMemoryMap)
    {
        EFIDebugf(ConOut, Str("Memory map info:\r\n"));
        EFIDebugf(ConOut, Str("    + Total size (bytes): %u64\r\n"), MemoryMap.Size);
        EFIDebugf(ConOut, Str("    + Descriptor count:   %u64\r\n"), DescriptorCount);
        EFIDebugf(ConOut, Str("    + Descriptor size:    %u64\r\n"), MemoryMap.DescriptorSize);
        EFIDebugf(ConOut, Str("    + Key:                %u64\r\n"), MemoryMap.Key);
    }

    // NOTE(vak): Detected conventional memory size
    if (ValidMemoryMap)
    {
        usize ConventionalMemorySize = 0;
        usize ConventionalMemoryCount = 0;

        for (usize Index = 0; Index < DescriptorCount; Index++)
        {
            efi_memory_descriptor* Descriptor = (efi_memory_descriptor*)
                (MemoryMap.Descriptors + Index * MemoryMap.DescriptorSize);

            if (Descriptor->Type == EfiConventionalMemory)
            {
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

        EFIInfof(ConOut, Str("Total memory size: "));

        EFISetPrintColor(ConOut, EFITextAttribute(EFI_YELLOW, EFI_BLACK));
        EFIPrintf(ConOut, Str("%u64.%u64%str"), Integer, Decimal, Postfix);

        EFISetPrintColor(ConOut, EFITextAttribute(EFI_LIGHTGRAY, EFI_BLACK));
        EFIPrintf(ConOut, Str("\r\n"));
    }

    // NOTE(vak): Locate ACPI RDSP

    acpi_rsdp* RSDP = 0;

    {
        EFIInfof(ConOut, Str("Locating ACPI RSDP...  "));

        efi_guid TargetGUID = EFI_ACPI_TABLE_GUID;
        usize TableCount = SystemTable->NumberOfTableEntries;

        for (usize Index = 0; Index < TableCount; Index++)
        {
            efi_configuration_table* Table = SystemTable->ConfigurationTables + Index;

            if (EFISameGUID(Table->VendorGUID, TargetGUID))
            {
                RSDP = Table->VendorTable;
                break;
            }
        }

        if (RSDP)
        {
            EFISetPrintColor(ConOut, EFITextAttribute(EFI_LIGHTGREEN, EFI_BLACK));
            EFIPrintf(ConOut, Str("SUCCESS"));
        }
        else
        {
            EFISetPrintColor(ConOut, EFITextAttribute(EFI_LIGHTRED, EFI_BLACK));
            EFIPrintf(ConOut, Str("FAILED"));
        }

        EFISetPrintColor(ConOut, EFITextAttribute(EFI_LIGHTGRAY, EFI_BLACK));
        EFIPrintf(ConOut, Str("\r\n"));
    }

    b32 ValidRSDP = true;

    if (RSDP)
    {
        EFIInfof(ConOut, Str("Validating ACPI RSDP...  "));

         // NOTE(vak): 0x2052545020445352 is "RSD PTR " in little endian

        if (RSDP->Signature != 0x2052545020445352)
        {
            EFIErrorf(ConOut, Str("Invalid ACPI RSDP signature (0x%x64)\r\n"), RSDP->Signature);
            ValidRSDP = false;
        }

        // NOTE(vak): Checksum check

        if (ValidRSDP)
        {
            u8 Checksum = 0;
            u8* Bytes = (u8*)RSDP;

            for (usize Index = 0; Index < 20; Index++)
                Checksum += Bytes[Index];

            if (Checksum != 0)
            {
                EFIErrorf(
                    ConOut,
                    Str("Computed Checksum for ACPI RSDP is not 0 (computed: %x8)\r\n"),
                    Checksum
                );

                ValidRSDP = false;
            }
        }

        // NOTE(vak): ExtendedChecksum check

        if (ValidRSDP)
        {
            u8 Checksum = 0;
            u8* Bytes = (u8*)RSDP;

            for (usize Index = 0; Index < sizeof(acpi_rsdp); Index++)
                Checksum += Bytes[Index];

            if (Checksum != 0)
            {
                EFIErrorf(
                    ConOut,
                    Str("Computed ExtendedChecksum for ACPI RSDP is not 0 (computed: %x8)\r\n"),
                    Checksum
                );

                ValidRSDP = false;
            }
        }

        if (ValidRSDP)
        {
            EFISetPrintColor(ConOut, EFITextAttribute(EFI_LIGHTGREEN, EFI_BLACK));
            EFIPrintf(ConOut, Str("SUCCESS"));
        }
        else
        {
            EFISetPrintColor(ConOut, EFITextAttribute(EFI_LIGHTRED, EFI_BLACK));
            EFIPrintf(ConOut, Str("FAILED"));
        }

        EFISetPrintColor(ConOut, EFITextAttribute(EFI_LIGHTGRAY, EFI_BLACK));
        EFIPrintf(ConOut, Str("\r\n"));
    }

    b32 ValidXSDT = false;
    acpi_xsdt* XSDT = 0;

    if (ValidRSDP)
    {
        EFIInfof(ConOut, Str("Validating ACPI XSDT...  "));

        ValidXSDT = true;

        XSDT = (acpi_xsdt*)RSDP->XSDTAddress;

        EFIDebugf(ConOut, Str("ACPI XSDT Address: %x64\r\n"), RSDP->XSDTAddress);

        // NOTE(vak): 0x54445358 is "XSDT" in little endian

        if (XSDT->Signature != 0x54445358)
        {
            EFIErrorf(ConOut, Str("Invalid signature for ACPI XSDT table.\r\n"));
            ValidXSDT = false;
        }

        u8  Checksum = 0;
        u32 Length   = XSDT->Length;
        u8* Bytes    = (u8*)XSDT;

        for (usize Index = 0; Index < Length; Index++)
            Checksum += Bytes[Index];

        if (Checksum != 0)
        {
            EFIErrorf(
                ConOut,
                Str("Computed Checksum for ACPI XSDT is not 0 (computed: %x8)\r\n"),
                Checksum
            );

            ValidXSDT = false;
        }

        if (ValidRSDP)
        {
            EFISetPrintColor(ConOut, EFITextAttribute(EFI_LIGHTGREEN, EFI_BLACK));
            EFIPrintf(ConOut, Str("SUCCESS"));
        }
        else
        {
            EFISetPrintColor(ConOut, EFITextAttribute(EFI_LIGHTRED, EFI_BLACK));
            EFIPrintf(ConOut, Str("FAILED"));
        }

        EFISetPrintColor(ConOut, EFITextAttribute(EFI_LIGHTGRAY, EFI_BLACK));
        EFIPrintf(ConOut, Str("\r\n"));
    }

    acpi_description_header* MCFG = 0;

    if (ValidXSDT)
    {
        EFIDebugf(ConOut, Str("ACPI tables:\r\n"));

        u32 TableCount = ACPIGetTableCount(XSDT);

        for (u32 Index = 0; Index < TableCount; Index++)
        {
            acpi_description_header* Table = ACPIGetTable(XSDT, Index);

            EFIDebugf(
                ConOut,
                Str("    + [%u32]: %str\r\n"),
                Index,
                StrData((char*)&Table->Signature, 4)
            );

            switch (Table->Signature)
            {
                // NOTE(vak): 0x4746434d is "MCFG" in little endian
                case 0x4746434d:
                {
                    MCFG = Table;
                    EFIInfof(ConOut, Str("Found ACPI MCFG.\r\n"));
                } break;
            }
        }
    }

    if (MCFG)
    {
        EFIDebugf(ConOut, Str("MCFG address: %x64"), MCFG);
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
