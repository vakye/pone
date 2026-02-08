
#pragma once

typedef usize efi_status;
typedef void* efi_handle;

typedef struct
{
    u64 Signature;
    u32 Revision;
    u32 HeaderSize;
    u32 CRC32;
    u32 Reserved;
} efi_table_header;

typedef struct efi_simple_text_output_protocol efi_simple_text_output_protocol;

typedef efi_status efi_text_clear_screen(
    efi_simple_text_output_protocol* This
);

typedef efi_status efi_text_string(
    efi_simple_text_output_protocol* This,
    u16* String
);

typedef struct efi_simple_text_output_protocol
{
     void*                  Reset;
     efi_text_string*       OutputString;
     void*                  TestString;
     void*                  QueryMode;
     void*                  SetMode;
     void*                  SetAttribute;
     efi_text_clear_screen* ClearScreen;
     void*                  SetCursorPosition;
     void*                  EnableCursor;
     void*                  Mode;
} efi_simple_text_output_protocol;

typedef struct
{
    efi_table_header                 Header;
    u16*                             FirmwareVendor;
    u32                              FirmwareRevision;
    efi_handle                       ConsoleInHandle;
    void*                            ConIn;
    efi_handle                       ConsoleOutHandle;
    efi_simple_text_output_protocol* ConOut;
    efi_handle                       StandardErrorHandle;
    efi_simple_text_output_protocol* StdErr;
    void*                            RuntimeServices;
    void*                            BootServices;
    usize                            NumberOfTableEntries;
    void*                            ConfigurationTable;
} efi_system_table;
