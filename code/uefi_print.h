
#pragma once

// NOTE(vak): Attributes

local void SetPrintColor(
    efi_simple_text_output_protocol*    ConOut,
    usize                               Attribute
);

// NOTE(vak): Clear

local void ClearScreen(efi_simple_text_output_protocol* ConOut);

// NOTE(vak): Print

local usize Print(
    efi_simple_text_output_protocol*    ConOut,
    string                              String
);

local usize PrintNewLine(
    efi_simple_text_output_protocol*    ConOut
);

local usize PrintRepeat(
    efi_simple_text_output_protocol*    ConOut,
    string                              String,
    usize                               Count
);

local usize PrintSpaces(
    efi_simple_text_output_protocol*    ConOut,
    usize                               SpaceCount
);

// NOTE(vak): Integer printing

typedef enum
{
    PrintBase_Bin = 2,
    PrintBase_Oct = 8,
    PrintBase_Dec = 10,
    PrintBase_Hex = 16,
} print_base;

local usize PrintUSize(
    efi_simple_text_output_protocol*    ConOut,
    usize                               Number,
    print_base                          Base
);

local usize PrintSSize(
    efi_simple_text_output_protocol*    ConOut,
    ssize                               Number,
    print_base                          Base
);
