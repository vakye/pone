
local u32 ACPIGetTableCount(acpi_xsdt* XSDT)
{
    u32 Result = (XSDT->Length - sizeof(acpi_xsdt)) / 8;
    return (Result);
}

local acpi_description_header* ACPIGetTable(acpi_xsdt* XSDT, u32 TableIndex)
{
    // NOTE(vak): The "Entries" array is located right after the
    // XSDT description header.

    acpi_description_header** Entries = (acpi_description_header**)
        ((u8*)XSDT + sizeof(acpi_xsdt));

    acpi_description_header* Table = 0;

    if (TableIndex < ACPIGetTableCount(XSDT))
        Table = Entries[TableIndex];

    return (Table);
}
