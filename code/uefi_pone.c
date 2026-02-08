
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

    usize DefaultTextAttribute    = EFITextAttribute(EFI_WHITE,    EFI_BLACK);
    usize EmphasizedTextAttribute = EFITextAttribute(EFI_LIGHTRED, EFI_BLACK);

    // NOTE(vak): ClearScreen and "Hello, world!"
    {
        SetPrintColor(ConOut, DefaultTextAttribute);
        ClearScreen  (ConOut);
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
    }

    efi_input_key InputKey = {0};

    for (;;)
    {
        while (ConIn->ReadKeyStroke(ConIn, &InputKey) == EFI_NOT_READY) {}

        if (InputKey.ScanCode == 0x17)
            break;
    }

    RuntimeServices->ResetSystem(EFI_ResetType_Shutdown, EFI_SUCCESS, 0, 0);

    return (EFI_SUCCESS);
}
