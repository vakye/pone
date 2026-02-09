
#pragma once

// NOTE(vak): EFI types

typedef usize efi_status;
typedef void* efi_handle;
typedef void* efi_event;
typedef u8 efi_boolean;

typedef usize efi_physical_address;
typedef usize efi_virtual_address;

typedef struct
{
    u32 Data1;
    u16 Data2;
    u16 Data3;
    u8  Data4[8];
} efi_guid;

// NOTE(vak): GUID for protcols

#define EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID \
    {0x9042a9de,0x23dc,0x4a38,\
    {0x96,0xfb,0x7a,0xde,0xd0,0x80,0x51,0x6a}}

// NOTE(vak): Calling convention

#define EFI_API __cdecl

// NOTE(vak): EFI status codes

enum
{
    EFI_SUCCESS,
    EFI_LOAD_ERROR,
    EFI_INVALID_PARAMETER,
    EFI_UNSUPPORTED,
    EFI_BAD_BUFFER_SIZE,
    EFI_BUFFER_TOO_SMALL,
    EFI_NOT_READY,
    EFI_DEVICE_ERROR,
    EFI_WRITE_PROTECTED,
    EFI_OUT_OF_RESOURCES,
    EFI_VOLUME_CORRUPTED,
    EFI_VOLUME_FULL,
    EFI_NO_MEDIA,
    EFI_MEDIA_CHANGED,
    EFI_NOT_FOUND,
    EFI_ACCESS_DENIED,
    EFI_NO_RESPONSE,
    EFI_NO_MAPPING,
    EFI_TIMEOUT,
    EFI_NOT_STARTED,
    EFI_ALREADY_STARTED,
    EFI_ABORTED,
    EFI_ICMP_ERROR,
    EFI_TFTP_ERROR,
    EFI_PROTOCOL_ERROR,
    EFI_INCOMPATIBLE_VERSION,
    EFI_SECURITY_VIOLATION,
    EFI_CRC_ERROR,
    EFI_END_OF_MEDIA,
    EFI_END_OF_FILE,
    EFI_INVALID_LANGUAGE,
    EFI_COMPROMISED_DATA,
    EFI_IP_ADDRESS_CONFLICT,
    EFI_HTTP_ERROR,
};

#define EFIStatusUnsetHighBit(Status) ((Status) & ~(1ull << 63))

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

typedef efi_status EFI_API efi_text_reset(
    efi_simple_text_output_protocol*    This,
    efi_boolean                         ExtendedVerification
);

typedef efi_status EFI_API efi_text_string(
    efi_simple_text_output_protocol*    This,
    u16*                                String
);

typedef efi_status EFI_API efi_text_query_mode(
    efi_simple_text_output_protocol*    This,
    usize                               ModeNumber,
    usize*                              Columns,
    usize*                              Rows
);

typedef efi_status EFI_API efi_text_set_mode(
    efi_simple_text_output_protocol*    This,
    usize                               ModeNumber
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
     efi_text_reset*         Reset;
     efi_text_string*        OutputString;
     void*                   TestString;
     efi_text_query_mode*    QueryMode;
     efi_text_set_mode*      SetMode;
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
    EfiResetCold,
    EfiResetWarm,
    EfiResetShutdown,
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

typedef enum
{
    EfiReservedMemoryType,
    EfiLoaderCode,
    EfiLoaderData,
    EfiBootServicesCode,
    EfiBootServicesData,
    EfiRuntimeServicesCode,
    EfiRuntimeServicesData,
    EfiConventionalMemory,
    EfiUnusableMemory,
    EfiACPIReclaimMemory,
    EfiACPIMemoryNVS,
    EfiMemoryMappedIO,
    EfiMemoryMappedIOPortSpace,
    EfiPalCode,
    EfiPersistentMemory,
    EfiUnacceptedMemoryType,
    EfiMaxMemoryType,
} efi_memory_type;

typedef efi_status EFI_API efi_allocate_pool(
    efi_memory_type MemoryType,
    usize           Size,
    void**          Result
);

typedef efi_status EFI_API efi_free_pool(
    void*           Buffer
);

typedef struct
{
    u32                     Type;
    efi_physical_address    PhysicalStart;
    efi_virtual_address     VirtualStart;
    u64                     NumberOfPages;
    u64                     Attribute;
} efi_memory_descriptor;

#define EFI_MEMORY_UC              0x0000000000000001
#define EFI_MEMORY_WC              0x0000000000000002
#define EFI_MEMORY_WT              0x0000000000000004
#define EFI_MEMORY_WB              0x0000000000000008
#define EFI_MEMORY_UCE             0x0000000000000010
#define EFI_MEMORY_WP              0x0000000000001000
#define EFI_MEMORY_RP              0x0000000000002000
#define EFI_MEMORY_XP              0x0000000000004000
#define EFI_MEMORY_NV              0x0000000000008000
#define EFI_MEMORY_MORE_RELIABLE   0x0000000000010000
#define EFI_MEMORY_RO              0x0000000000020000
#define EFI_MEMORY_SP              0x0000000000040000
#define EFI_MEMORY_CPU_CRYPTO      0x0000000000080000
#define EFI_MEMORY_RUNTIME         0x8000000000000000
#define EFI_MEMORY_ISA_VALID       0x4000000000000000
#define EFI_MEMORY_ISA_MASK        0x0FFFF00000000000

typedef efi_status EFI_API efi_get_memory_map(
    usize*                  MemoryMapSize,
    efi_memory_descriptor*  MemoryDescriptors,
    usize*                  MapKey,
    usize*                  DescriptorSize,
    u32*                    DescriptorVersion
);

typedef efi_status EFI_API efi_locate_protocol(
    efi_guid*               Protocol,
    void*                   Registration,
    void**                  Interface
);

typedef struct
{
    efi_table_header                    Header;

    // NOTE(vak): Task priority

    void*                               RaiseTPL;
    void*                               RestoreTPL;

    // NOTE(vak): Memory services

    void*                               AllocatePages;
    void*                               FreePages;
    efi_get_memory_map*                 GetMemoryMap;
    efi_allocate_pool*                  AllocatePool;
    efi_free_pool*                      FreePool;

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
    efi_locate_protocol*                LocateProtocol;
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

// NOTE(vak): Graphics output protocol

typedef struct efi_graphics_output_protocol efi_graphics_output_protocol;

typedef enum
{
    PixelRedGreenBlueReserved8BitPerColor,
    PixelBlueGreenRedReserved8BitPerColor,
    PixelBitMask,
    PixelBltOnly,
    PixelFormatMax
} efi_graphics_pixel_format;

typedef struct
{
    u32 RedMask;
    u32 GreenMask;
    u32 BlueMask;
    u32 ReservedMask;
} efi_pixel_bitmask;

typedef struct
{
    u32                         Version;
    u32                         HorzResolution;
    u32                         VertResolution;
    efi_graphics_pixel_format   PixelFormat;
    efi_pixel_bitmask           PixelInformation;
    u32                         PixelsPerScanline;
} efi_graphics_output_mode_information;

typedef struct
{
    u32                                     MaxMode;
    u32                                     Mode;
    efi_graphics_output_mode_information*   Info;
    usize                                   SizeOfInfo;
    efi_physical_address                    FramebufferBase;
    usize                                   FramebufferSize;
} efi_graphics_output_protocol_mode;

typedef efi_status EFI_API efi_graphics_output_protocol_query_mode(
    efi_graphics_output_protocol*           This,
    u32                                     ModeNumber,
    usize*                                  SizeOfInfo,
    efi_graphics_output_mode_information**  Info
);

typedef efi_status EFI_API efi_graphics_output_protocol_set_mode(
    efi_graphics_output_protocol*           This,
    u32                                     ModeNumber
);

typedef struct
{
    u8 Blue;
    u8 Green;
    u8 Red;
    u8 Reserved;
} efi_graphics_output_blt_pixel;

typedef enum
{
    EfiBltVideoFill,
    EfiBltVideoToBltBuffer,
    EfiBltBufferToVideo,
    EfiBltVideoToVideo,
    EfiGraphicsOutputBltOperationMax
} efi_graphics_output_blt_operation;

typedef efi_status EFI_API efi_graphics_output_protocol_blt(
    efi_graphics_output_protocol*           This,
    efi_graphics_output_blt_pixel*          BltBuffer,
    efi_graphics_output_blt_operation       BltOperation,
    usize                                   SourceX,
    usize                                   SourceY,
    usize                                   DestinationX,
    usize                                   DestinationY,
    usize                                   Width,
    usize                                   Height,
    usize                                   Delta
);

typedef struct efi_graphics_output_protocol
{
    efi_graphics_output_protocol_query_mode*    QueryMode;
    efi_graphics_output_protocol_set_mode*      SetMode;
    efi_graphics_output_protocol_blt*           Blt;
    efi_graphics_output_protocol_mode*          Mode;
} efi_graphics_output_protocol;
