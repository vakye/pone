
#include "pone_shared.h"
#include "pone_shared.c"

#include "uefi_pone.h"

efi_status EntryUEFI(void* ImageHandle, efi_system_table* SystemTable)
{
    (void) ImageHandle;

    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Hello, world!\n");

    for (;;) {}
}
