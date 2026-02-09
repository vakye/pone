
// NOTE(vak): Character

local b32 IsLowercase(char Character)
{
    b32 Result = ((Character >= 'a') && (Character <= 'z'));
    return (Result);
}

local b32 IsUppercase(char Character)
{
    b32 Result = ((Character >= 'A') && (Character <= 'Z'));
    return (Result);
}

local char ToLowercase(char Character)
{
    char Result = (char)(Character + 32*IsUppercase(Character));
    return (Result);
}

local char ToUppercase(char Character)
{
    char Result = (char)(Character - 32*IsLowercase(Character));
    return (Result);
}

// NOTE(vak): Conversion

local string CString(char* Data)
{
    string Result = StrData(Data, 0);

    if (Data)
    {
        while (Data[Result.Size])
            Result.Size++;
    }

    return (Result);
}

// NOTE(vak): Comparison

local b32 StringStartsWith(string String, string Compare)
{
    b32 Result = (String.Size >= Compare.Size);

    if (Result)
    {
        for (usize Index = 0; Index < Compare.Size; Index++)
        {
            if (String.Data[Index] != Compare.Data[Index])
            {
                Result = false;
                break;
            }
        }
    }

    return (Result);
}

local b32 StringEquals(string A, string B)
{
    b32 Result = (A.Size == B.Size);

    if (Result)
    {
        for (usize Index = 0; Index < A.Size; Index++)
        {
            if (A.Data[Index] != B.Data[Index])
            {
                Result = false;
                break;
            }
        }
    }

    return (Result);
}

// NOTE(vak): Case-insensitive comparison

local b32 StringStartsWithCI(string String, string Compare)
{
    b32 Result = (String.Size >= Compare.Size);

    if (Result)
    {
        for (usize Index = 0; Index < Compare.Size; Index++)
        {
            if (ToLowercase(String.Data[Index]) != ToLowercase(Compare.Data[Index]))
            {
                Result = false;
                break;
            }
        }
    }

    return (Result);
}

local b32 StringEqualsCI(string A, string B)
{
    b32 Result = (A.Size == B.Size);

    if (Result)
    {
        for (usize Index = 0; Index < A.Size; Index++)
        {
            if (ToLowercase(A.Data[Index]) != ToLowercase(B.Data[Index]))
            {
                Result = false;
                break;
            }
        }
    }

    return (Result);
}

// NOTE(vak): Stream

local char PeekCharacter(string* String)
{
    char Character = 0;

    if (String->Size)
        Character = String->Data[0];

    return (Character);
}

local char ConsumeCharacter(string* String)
{
    char Character = 0;

    if (String->Size)
    {
        Character = String->Data[0];

        String->Data++;
        String->Size--;
    }

    return (Character);
}

local string ConsumeStringView(string* String, usize Size)
{
    usize LimitedSize = Minimum(String->Size, Size);

    string Result = StrData(String->Data, LimitedSize);

    String->Data += LimitedSize;
    String->Size -= LimitedSize;

    return (Result);
}

// NOTE(vak): Formatting

local string FormatString(void* Buffer, usize BufferSize, string Format, ...)
{
    va_list ArgList;
    va_start(ArgList, Format);

    string Result = FormatStringV(Buffer, BufferSize, Format, ArgList);

    va_end(ArgList);

    return (Result);
}

typedef struct
{
    char* Current;
    usize Remaining;
} format_string_dest;

typedef enum
{
    FormatStringType_Unknown = 0,

    FormatStringType_S8,
    FormatStringType_S16,
    FormatStringType_S32,
    FormatStringType_S64,

    FormatStringType_U8,
    FormatStringType_U16,
    FormatStringType_U32,
    FormatStringType_U64,

    FormatStringType_B8,
    FormatStringType_B16,
    FormatStringType_B32,
    FormatStringType_B64,

    FormatStringType_O8,
    FormatStringType_O16,
    FormatStringType_O32,
    FormatStringType_O64,

    FormatStringType_X8,
    FormatStringType_X16,
    FormatStringType_X32,
    FormatStringType_X64,

    FormatStringType_C,
    FormatStringType_Str,
    FormatStringType_CStr,
} format_string_type;

typedef struct
{
    string             Match;
    format_string_type Type;
} format_string_entry;

typedef struct
{
    ssize Signed;
    usize Unsigned;
} format_string_arg_value;

local void FormatStringPush(format_string_dest* Dest, char Character)
{
    if (Dest->Remaining)
    {
        Dest->Current[0] = Character;

        Dest->Current++;
        Dest->Remaining--;
    }
}

local void FormatStringPushString(format_string_dest* Dest, string String)
{
    usize Size = Minimum(Dest->Remaining, String.Size);

    for (usize Index = 0; Index < Size; Index++)
    {
        Dest->Current[Index] = String.Data[Index];
    }

    Dest->Current   += Size;
    Dest->Remaining -= Size;
}

local void FormatIntegerToString(format_string_dest* Dest, usize Value, usize Base)
{
    if (Base <= 1) return;
    if (Base > 16) return;

    persist char Buffer[64] = {0};
    persist char DigitMap[] = "0123456789abcdef";

    usize DigitCount = 0;
    usize DigitIndex = ArrayCount(Buffer);

    do
    {
        char Digit = (char)(Value % Base);
        Value /= Base;

        DigitCount++;
        DigitIndex--;

        Buffer[DigitIndex] = DigitMap[Digit];
    } while (Value);

    FormatStringPushString(Dest, StrData(Buffer + DigitIndex, DigitCount));
}

local string FormatStringV(void* Buffer, usize BufferSize, string Format, va_list ArgList)
{
    format_string_dest Dest =
    {
        .Current   = Buffer,
        .Remaining = BufferSize,
    };

    while (Format.Size && Dest.Remaining)
    {
        while (Format.Size && Dest.Remaining)
        {
            char Character = PeekCharacter(&Format);
            if (Character == '%')
                break;

            FormatStringPush(&Dest, Character);

            ConsumeCharacter(&Format);
        }

        if ((Format.Size == 0) || (Dest.Remaining == 0))
            break;

        ConsumeCharacter(&Format); // NOTE(vak): Consume '%'

        // NOTE(vak): Match specifiers

        format_string_type Type = FormatStringType_Unknown;

        persist format_string_entry Entries[] =
        {
            {StaticStr("s8"),   FormatStringType_S8 },
            {StaticStr("s16"),  FormatStringType_S16},
            {StaticStr("s32"),  FormatStringType_S32},
            {StaticStr("s64"),  FormatStringType_S64},

            {StaticStr("u8"),   FormatStringType_U8 },
            {StaticStr("u16"),  FormatStringType_U16},
            {StaticStr("u32"),  FormatStringType_U32},
            {StaticStr("u64"),  FormatStringType_U64},

            {StaticStr("b8"),   FormatStringType_B8 },
            {StaticStr("b16"),  FormatStringType_B16},
            {StaticStr("b32"),  FormatStringType_B32},
            {StaticStr("b64"),  FormatStringType_B64},

            {StaticStr("o8"),   FormatStringType_O8 },
            {StaticStr("o16"),  FormatStringType_O16},
            {StaticStr("o32"),  FormatStringType_O32},
            {StaticStr("o64"),  FormatStringType_O64},

            {StaticStr("x8"),   FormatStringType_X8 },
            {StaticStr("x16"),  FormatStringType_X16},
            {StaticStr("x32"),  FormatStringType_X32},
            {StaticStr("x64"),  FormatStringType_X64},

            {StaticStr("c"),    FormatStringType_C},
            {StaticStr("str"),  FormatStringType_Str},
            {StaticStr("cstr"), FormatStringType_CStr},
        };

        for (usize Index = 0; Index < ArrayCount(Entries); Index++)
        {
            format_string_entry* Entry = Entries + Index;

            if (StringStartsWithCI(Format, Entry->Match))
            {
                ConsumeStringView(&Format, Entry->Match.Size);
                Type = Entry->Type;

                break;
            }
        }

        // NOTE(vak): Consume value from arguement and then format
        // the string to the destination buffer

        format_string_arg_value ArgValue = {0};

        switch (Type)
        {
            case FormatStringType_Unknown: break;

            // NOTE(vak): Unfortunately, C doesn't have a feature where you can specify
            // common code paths for certain groups of cases, so gotos are employed instead :[

            case FormatStringType_S8:  { ArgValue.Signed = va_arg(ArgList, s8);  goto DoSxx; }
            case FormatStringType_S16: { ArgValue.Signed = va_arg(ArgList, s16); goto DoSxx; }
            case FormatStringType_S32: { ArgValue.Signed = va_arg(ArgList, s32); goto DoSxx; }
            case FormatStringType_S64: { ArgValue.Signed = va_arg(ArgList, s64); goto DoSxx; }
            {
                DoSxx:

                if (ArgValue.Signed < 0)
                {
                    ArgValue.Signed = -ArgValue.Signed;
                    FormatStringPush(&Dest, '-');
                }

                FormatIntegerToString(&Dest, ArgValue.Signed, 10);
            } break;

            case FormatStringType_U8:  { ArgValue.Unsigned = va_arg(ArgList, u8);  goto DoUxx; }
            case FormatStringType_U16: { ArgValue.Unsigned = va_arg(ArgList, u16); goto DoUxx; }
            case FormatStringType_U32: { ArgValue.Unsigned = va_arg(ArgList, u32); goto DoUxx; }
            case FormatStringType_U64: { ArgValue.Unsigned = va_arg(ArgList, u64); goto DoUxx; }
            {
                DoUxx:

                FormatIntegerToString(&Dest, ArgValue.Unsigned, 10);
            } break;

            case FormatStringType_B8:  { ArgValue.Unsigned = va_arg(ArgList, u8);  goto DoBxx; }
            case FormatStringType_B16: { ArgValue.Unsigned = va_arg(ArgList, u16); goto DoBxx; }
            case FormatStringType_B32: { ArgValue.Unsigned = va_arg(ArgList, u32); goto DoBxx; }
            case FormatStringType_B64: { ArgValue.Unsigned = va_arg(ArgList, u64); goto DoBxx; }
            {
                DoBxx:

                FormatIntegerToString(&Dest, ArgValue.Unsigned, 2);
            } break;

            case FormatStringType_O8:  { ArgValue.Unsigned = va_arg(ArgList, u8);  goto DoOxx; }
            case FormatStringType_O16: { ArgValue.Unsigned = va_arg(ArgList, u16); goto DoOxx; }
            case FormatStringType_O32: { ArgValue.Unsigned = va_arg(ArgList, u32); goto DoOxx; }
            case FormatStringType_O64: { ArgValue.Unsigned = va_arg(ArgList, u64); goto DoOxx; }
            {
                DoOxx:

                FormatIntegerToString(&Dest, ArgValue.Unsigned, 8);
            } break;

            case FormatStringType_X8:  { ArgValue.Unsigned = va_arg(ArgList, u8);  goto DoXxx; }
            case FormatStringType_X16: { ArgValue.Unsigned = va_arg(ArgList, u16); goto DoXxx; }
            case FormatStringType_X32: { ArgValue.Unsigned = va_arg(ArgList, u32); goto DoXxx; }
            case FormatStringType_X64: { ArgValue.Unsigned = va_arg(ArgList, u64); goto DoXxx; }
            {
                DoXxx:

                FormatIntegerToString(&Dest, ArgValue.Unsigned, 16);
            } break;

            case FormatStringType_C:
            {
                FormatStringPush(&Dest, va_arg(ArgList, char));
            } break;

            case FormatStringType_Str:
            {
                FormatStringPushString(&Dest, va_arg(ArgList, string));
            } break;

            case FormatStringType_CStr:
            {
                FormatStringPushString(&Dest, CString(va_arg(ArgList, char*)));
            } break;
        }
    }

    string Result = StrData(Buffer, BufferSize - Dest.Remaining);

    return (Result);
}
