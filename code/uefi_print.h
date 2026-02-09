
#pragma once

local void EFISetPrintColor(
    efi_simple_text_output_protocol*    ConOut,
    usize                               Attribute
);

local void EFIClearScreen(
    efi_simple_text_output_protocol*    ConOut
);

local usize EFIPrintfV(
    efi_simple_text_output_protocol*    ConOut,
    string                              Format,
    va_list                             ArgList
);

local usize EFIPrintf(
    efi_simple_text_output_protocol*    ConOut,
    string                              Format,
    ...
);

local usize EFIDebugf(
    efi_simple_text_output_protocol*    ConOut,
    string                              Format,
    ...
);

local usize EFIErrorf(
    efi_simple_text_output_protocol*    ConOut,
    string                              Format,
    ...
);
