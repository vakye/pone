
#pragma once

#pragma pack(push, 1)

typedef struct
{
    // NOTE(vak): ACPI 1.0 and above

    u64 Signature;
    u8  Checksum;
    u8  OEMID[6];
    u8  Revision;
    u32 RSDTAddress;

    // NOTE(vak): ACPI 2.0 and above

    u32 Length;
    u64 XSDTAddress;
    u8  ExtendedChecksum;
    u8  Reserved[3];
} acpi_rsdp;

typedef struct
{
    u32 Signature;
    u32 Length;
    u8  Revision;
    u8  Checksum;
    u8  OEMID[6];
    u64 OEMTableID;
    u32 OEMRevision;
    u32 CreatorID;
    u32 CreatorRevision;

    // Following the structure above is an array of
    // pointers with each leading to an acpi_description_header.
    //
    // acpi_description_header* Entries[Count];
    //
    // Count = (XSDT.Length - sizeof(acpi_xsdt)) / 8
} acpi_xsdt;

// The same as acpi_xsdt but no "Entries" array.
typedef acpi_xsdt acpi_description_header;

#pragma pack(pop)

local u32 ACPIGetTableCount(acpi_xsdt* XSDT);

local acpi_description_header* ACPIGetTable(acpi_xsdt* XSDT, u32 TableIndex);
local acpi_description_header* ACPIFindTable(acpi_xsdt* XSDT, u32 Tag);
