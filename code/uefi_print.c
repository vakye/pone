
local char  OutputBuffer[KB(32)] = {0};
local u8    AttributeBuffer[KB(32)] = {0};
local usize OutputBufferSize  = 0;
local usize OutputBufferWrite = 0;

local u8    CurrentAttribute = 0;

local void EFIRestorePrintBuffer(
    efi_simple_text_output_protocol*    ConOut
)
{
    ConOut->SetAttribute(ConOut, EFITextAttribute(EFI_LIGHTGRAY, EFI_BLACK));
    ConOut->ClearScreen (ConOut);

    if (OutputBufferWrite == OutputBufferSize)
    {
        for (usize Index = 0; Index < OutputBufferWrite; Index++)
        {
            u16 String[] = {OutputBuffer[Index], 0};

            ConOut->SetAttribute(ConOut, AttributeBuffer[Index]);
            ConOut->OutputString(ConOut, String);
        }
    }
    else if (OutputBufferSize < OutputBufferSize)
    {
        usize Before = OutputBufferSize - OutputBufferWrite;
        usize After  = OutputBufferWrite;

        for (usize Index = 0; Index < Before; Index++)
        {
            u16 String[] = {OutputBuffer[After + Index], 0};

            ConOut->SetAttribute(ConOut, AttributeBuffer[After + Index]);
            ConOut->OutputString(ConOut, String);
        }

        for (usize Index = 0; Index < After; Index++)
        {
            u16 String[] = {OutputBuffer[Index], 0};

            ConOut->SetAttribute(ConOut, AttributeBuffer[Index]);
            ConOut->OutputString(ConOut, String);
        }
    }
}

local void EFISetPrintColor(
    efi_simple_text_output_protocol*    ConOut,
    usize                               Attribute
)
{
    ConOut->SetAttribute(ConOut, Attribute);
    CurrentAttribute = (u8)Attribute;
}

local void EFIClearScreen(efi_simple_text_output_protocol* ConOut)
{
    ConOut->ClearScreen(ConOut);
}

local usize EFIPrintfV(
    efi_simple_text_output_protocol*    ConOut,
    string                              Format,
    va_list                             ArgList
)
{
    // TODO(vak): Convert UTF8 to UTF16

    usize Result = 0;

    persist char Buffer8[4096] = {0};
    persist u16 Buffer16[2048]  = {0};

    string String = FormatStringV(Buffer8, sizeof(Buffer8), Format, ArgList);

    while (String.Size)
    {
        usize Count = Minimum(String.Size, ArrayCount(Buffer16) - 1);

        for (usize Index = 0; Index < Count; Index++)
        {
            char Character = (u16)String.Data[Index];

            Buffer16[Index] = (u16)Character;

            OutputBuffer[OutputBufferWrite] = Character;
            AttributeBuffer[OutputBufferWrite] = CurrentAttribute;

            OutputBufferWrite = (OutputBufferWrite + 1) % ArrayCount(OutputBuffer);
        }

        Buffer16[Count] = '\0';

        ConOut->OutputString(ConOut, Buffer16);

        String.Data += Count;
        String.Size -= Count;

        Result += Count;

        OutputBufferSize = Minimum(OutputBufferSize  + Count, ArrayCount(OutputBuffer));
    }

    return (Result);
}

local usize EFIPrintf(
    efi_simple_text_output_protocol*    ConOut,
    string                              Format,
    ...
)
{
    va_list ArgList;
    va_start(ArgList, Format);
    usize Result = EFIPrintfV(ConOut, Format, ArgList);
    va_end(ArgList);

    return (Result);
}

local usize EFIInfof(
    efi_simple_text_output_protocol*    ConOut,
    string                              Format,
    ...
)
{
    usize Result = 0;

    Result += EFIPrintf(ConOut, Str("["));

    EFISetPrintColor(ConOut, EFITextAttribute(EFI_LIGHTCYAN, EFI_BLACK));
    Result += EFIPrintf(ConOut, Str("INFO "));
    EFISetPrintColor(ConOut, EFITextAttribute(EFI_LIGHTGRAY, EFI_BLACK));

    Result += EFIPrintf(ConOut, Str("]: "));

    va_list ArgList;
    va_start(ArgList, Format);
    Result += EFIPrintfV(ConOut, Format, ArgList);
    va_end(ArgList);

    return (Result);
}

local usize EFIDebugf(
    efi_simple_text_output_protocol*    ConOut,
    string                              Format,
    ...
)
{
    usize Result = 0;

#if 0
    Result += EFIPrintf(ConOut, Str("["));

    EFISetPrintColor(ConOut, EFITextAttribute(EFI_LIGHTGREEN, EFI_BLACK));
    Result += EFIPrintf(ConOut, Str("DEBUG"));
    EFISetPrintColor(ConOut, EFITextAttribute(EFI_LIGHTGRAY, EFI_BLACK));

    Result += EFIPrintf(ConOut, Str("]: "));

    va_list ArgList;
    va_start(ArgList, Format);
    Result += EFIPrintfV(ConOut, Format, ArgList);
    va_end(ArgList);
#endif

    return (Result);
}

local usize EFIErrorf(
    efi_simple_text_output_protocol*    ConOut,
    string                              Format,
    ...
)
{
    usize Result = 0;

    Result += EFIPrintf(ConOut, Str("["));

    EFISetPrintColor(ConOut, EFITextAttribute(EFI_LIGHTRED, EFI_BLACK));
    Result += EFIPrintf(ConOut, Str("ERROR"));
    EFISetPrintColor(ConOut, EFITextAttribute(EFI_LIGHTGRAY, EFI_BLACK));

    Result += EFIPrintf(ConOut, Str("]: "));

    va_list ArgList;
    va_start(ArgList, Format);
    Result += EFIPrintfV(ConOut, Format, ArgList);
    va_end(ArgList);

    return (Result);
}
