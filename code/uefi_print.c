
// NOTE(vak): Basic

local void SetPrintColor(
    efi_simple_text_output_protocol*    ConOut,
    usize                               Attribute
)
{
    ConOut->SetAttribute(ConOut, Attribute);
}

local void ClearScreen(efi_simple_text_output_protocol* ConOut)
{
    ConOut->ClearScreen(ConOut);
}

local usize Print(
    efi_simple_text_output_protocol*    ConOut,
    string                              String
)
{
    // TODO(vak): Convert UTF8 to UTF16

    persist u16 Buffer[4096] = {0};

    while (String.Size)
    {
        usize Count = Minimum(String.Size, ArrayCount(Buffer) - 1);

        for (usize Index = 0; Index < Count; Index++)
        {
            Buffer[Index] = String.Data[Index];
        }

        Buffer[Count] = '\0';

        ConOut->OutputString(ConOut, Buffer);

        String.Data += Count;
        String.Size -= Count;
    }

    usize Result = String.Size;
    return (Result);
}

local usize PrintNewLine(
    efi_simple_text_output_protocol*    ConOut
)
{
    usize Result = Print(ConOut, Str("\r\n"));
    return (Result);
}

local usize PrintRepeat(
    efi_simple_text_output_protocol*    ConOut,
    string                              String,
    usize                               Count
)
{
    usize Result = 0;

    for (usize Index = 0; Index < Count; Index++)
        Result += Print(ConOut, String);

    return (Result);
}

local usize PrintSpaces(
    efi_simple_text_output_protocol*    ConOut,
    usize                               SpaceCount
)
{
    usize Result = PrintRepeat(ConOut, Str(" "), SpaceCount);
    return (Result);
}

// NOTE(vak): Integer printing

local usize PrintUSize(
    efi_simple_text_output_protocol*    ConOut,
    usize                               Number,
    print_base                          Base
)
{
    if (Base == 0) return (0);
    if (Base > 16) return (0);

    char Buffer[64] = {0};
    usize DigitCount = 0;
    usize DigitIndex = ArrayCount(Buffer);

    persist char DigitCharacters[] = "0123456789abcdef";

    do
    {
        char Digit = (char)(Number % Base);
        Number /= Base;

        DigitCount++;
        DigitIndex--;

        Buffer[DigitIndex] = DigitCharacters[Digit];
    } while(Number);

    usize Result = Print(ConOut, StrData(Buffer + DigitIndex, DigitCount));
    return (Result);
}

local usize PrintSSize(
    efi_simple_text_output_protocol*    ConOut,
    ssize                               Number,
    print_base                          Base
)
{
    usize Result = 0;

    if (Number < 0)
    {
        Number = -Number;
        Result += Print(ConOut, Str("-"));
    }

    Result += PrintUSize(ConOut, Number, Base);
    return (Result);
}
