
#include "pone_shared.h"
#include "pone_shared.c"

#include "uefi_pone.h"

efi_status EFI_API EntryUEFI(efi_handle ImageHandle, efi_system_table* SystemTable)
{
    (void) ImageHandle;

    efi_simple_text_input_protocol*  ConIn  = SystemTable->ConIn;
    efi_simple_text_output_protocol* ConOut = SystemTable->ConOut;

    efi_runtime_services* RuntimeServices   = SystemTable->RuntimeServices;

    usize DefaultTextAttribute    = EFITextAttribute(EFI_WHITE,    EFI_BLACK);
    usize EmphasizedTextAttribute = EFITextAttribute(EFI_LIGHTRED, EFI_BLACK);

    ConOut->SetAttribute(ConOut, DefaultTextAttribute);
    ConOut->ClearScreen (ConOut);
    ConOut->OutputString(ConOut, L"Hello, world\r\n");

    ConOut->OutputString(ConOut, L"Press [ESC] to ");
    ConOut->SetAttribute(ConOut, EmphasizedTextAttribute);
    ConOut->OutputString(ConOut, L"shutdown");
    ConOut->SetAttribute(ConOut, DefaultTextAttribute);
    ConOut->OutputString(ConOut, L"...");

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
