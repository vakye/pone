
#pragma once

// NOTE(vak): EFI types

typedef usize efi_status;
typedef void* efi_handle;
typedef void* efi_event;

// NOTE(vak): Calling convention

#define EFI_API

// NOTE(vak): EFI status codes

#define EFI_SUCCESS                 (0)
#define EFI_NOT_READY               (6)

// NOTE(vak): Simple text input protocol

typedef struct efi_simple_text_input_protocol efi_simple_text_input_protocol;

typedef struct
{
    u16 ScanCode;
    s16 UnicodeChar;
} efi_input_key;

typedef efi_status EFI_API efi_input_read_key(
    efi_simple_text_input_protocol* This,
    efi_input_key*                  Key
);

typedef struct efi_simple_text_input_protocol
{
    void*                   Reset;
    efi_input_read_key*     ReadKeyStroke;
    efi_event               WaitForKey;
} efi_simple_text_input_protocol;

// NOTE(vak): Simple text output protocol

typedef struct efi_simple_text_output_protocol efi_simple_text_output_protocol;

#define EFI_BLACK                              (0x00)
#define EFI_BLUE                               (0x01)
#define EFI_GREEN                              (0x02)
#define EFI_CYAN                               (0x03)
#define EFI_RED                                (0x04)
#define EFI_MAGENTA                            (0x05)
#define EFI_BROWN                              (0x06)
#define EFI_LIGHTGRAY                          (0x07)
#define EFI_BRIGHT                             (0x08)
#define EFI_DARKGRAY                           (EFI_BLACK | EFI_BRIGHT)
#define EFI_LIGHTBLUE                          (0x09)
#define EFI_LIGHTGREEN                         (0x0A)
#define EFI_LIGHTCYAN                          (0x0B)
#define EFI_LIGHTRED                           (0x0C)
#define EFI_LIGHTMAGENTA                       (0x0D)
#define EFI_YELLOW                             (0x0E)
#define EFI_WHITE                              (0x0F)

#define EFITextAttribute(Foreground,Background) ((Foreground) | ((Background) << 4))

typedef efi_status EFI_API efi_text_string(
    efi_simple_text_output_protocol*    This,
    u16*                                String
);

typedef efi_status EFI_API efi_text_set_attribute(
    efi_simple_text_output_protocol* This,
    usize                            Attribute
);

typedef efi_status EFI_API efi_text_clear_screen(
    efi_simple_text_output_protocol* This
);

typedef struct efi_simple_text_output_protocol
{
     void*                   Reset;
     efi_text_string*        OutputString;
     void*                   TestString;
     void*                   QueryMode;
     void*                   SetMode;
     efi_text_set_attribute* SetAttribute;
     efi_text_clear_screen*  ClearScreen;
     void*                   SetCursorPosition;
     void*                   EnableCursor;
     void*                   Mode;
} efi_simple_text_output_protocol;

// NOTE(vak): Table header

typedef struct
{
    u64 Signature;
    u32 Revision;
    u32 HeaderSize;
    u32 CRC32;
    u32 Reserved;
} efi_table_header;

// NOTE(vak): Runtime services

typedef enum
{
    EFI_ResetType_Cold,
    EFI_ResetType_Warm,
    EFI_ResetType_Shutdown,
    EFI_ResetType_PlatformSpecific,
} efi_reset_type;

typedef void EFI_API efi_reset_system(
    efi_reset_type  ResetType,
    efi_status      ResetStatus,
    usize           DataSize,
    void*           ResetData
);

typedef struct
{
    efi_table_header                    Header;

    // NOTE(vak): Time services

    void*                               GetTime;
    void*                               SetTime;
    void*                               GetWakeupTime;
    void*                               SetWakeupTime;

    // NOTE(vak): Virtual memory services

    void*                               SetVirtualAddressMap;
    void*                               ConvertPointer;

    // NOTE(vak): Variable services

    void*                               GetVariable;
    void*                               GetNextVariableName;
    void*                               SetVariable;

    // NOTE(vak): Miscellaneous services

    void*                               GetNextHighMonotonicCount;
    efi_reset_system*                   ResetSystem;

    // NOTE(vak): UEFI 2.0 Capsule Services

    void*                               UpdateCapsule;
    void*                               QueryCapsuleCapabilities;

    // NOTE(vak): Miscellaneous UEFI 2.0 service

    void*                               QueryVariableInfo;
} efi_runtime_services;

// NOTE(vak): Boot services

typedef struct
{
    u32     Type;
    usize   PhysicalAddress;
    usize   VirtualAddress;
    u64     NumberOfPages;
    u64     Attribute;
} efi_memory_descriptor;

typedef efi_status EFI_API efi_get_memory_map(
    usize*                 MemoryMapSize,
    efi_memory_descriptor* MemoryDescriptors,
    usize*                 MapKey,
    usize*                 DescriptorSize,
    u32*                   DescriptorVersion
);

typedef struct
{
    efi_table_header                    Header;

    // NOTE(vak): Task priority

    void*                               RaiseTPL;
    void*                               RestoreTPL;

    // NOTE(vak): Memory services

    void*                               AllocateMemory;
    void*                               FreePages;
    efi_get_memory_map*                 GetMemoryMap;
    void*                               AllocatePool;
    void*                               FreePool;

    // NOTE(vak): Event & Timer services

    void*                               CreateEvent;
    void*                               SetTimer;
    void*                               WaitForEvent;
    void*                               SignalEvent;
    void*                               CloseEvent;
    void*                               CheckEvent;

    // NOTE(vak): Protocol handler services

    void*                               InstallProtocolInterface;
    void*                               ReinstallProtocolInterface;
    void*                               UninstallProtocolInterface;
    void*                               HandleProtocol;
    void*                               Reserved;
    void*                               RegisterProtocolNotify;
    void*                               LocateHandle;
    void*                               LocateDevicePath;
    void*                               InstallConfigurationTable;

    // NOTE(vak): Image services

    void*                               LoadImage;
    void*                               StartImage;
    void*                               Exit;
    void*                               UnloadImage;
    void*                               ExitBootServices;

    // NOTE(vak): Miscellaneous services

    void*                               GetNextMonotonicCount;
    void*                               Stall;
    void*                               SetWatchdogTimer;

    // NOTE(vak): DriverSupport services

    void*                               ConnectController;
    void*                               DisconnectController;

    // NOTE(vak): Open and Close protocol services

    void*                               OpenProtocol;
    void*                               CloseProtocol;
    void*                               OpenProtocolInformation;

    // NOTE(vak): Library services

    void*                               ProtocolsPerHandle;
    void*                               LocateHandleBuffer;
    void*                               LocateProtocol;
    void*                               InstallMultipleProtocolInterfaces;
    void*                               UninstallMultipleProtocolInterfaces;

    // NOTE(vak): 32-bit CRC services

    void*                               CalculateCRC32;

    // NOTE(vak): Miscellaneous services

    void*                               CopyMem;
    void*                               SetMem;
    void*                               CreateEventEx;
} efi_boot_services;

// NOTE(vak): System table

typedef struct
{
    efi_table_header                    Header;
    u16*                                FirmwareVendor;
    u32                                 FirmwareRevision;
    efi_handle                          ConsoleInHandle;
    efi_simple_text_input_protocol*     ConIn;
    efi_handle                          ConsoleOutHandle;
    efi_simple_text_output_protocol*    ConOut;
    efi_handle                          StandardErrorHandle;
    efi_simple_text_output_protocol*    StdErr;
    efi_runtime_services*               RuntimeServices;
    efi_boot_services*                  BootServices;
    usize                               NumberOfTableEntries;
    void*                               ConfigurationTable;
} efi_system_table;
