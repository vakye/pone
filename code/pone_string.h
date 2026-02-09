
#pragma once

#include <stdarg.h>

typedef struct
{
    char* Data;
    usize Size;
} string;

// NOTE(vak): Initialization macros

#define StrData(Data, Size)         (string){Data, Size}
#define Str(Literal)                (string){Literal, sizeof(Literal) - 1}

#define StaticStrData(Data, Size)   {Data, Size}
#define StaticStr(Literal)          {Literal, sizeof(Literal) - 1}

// NOTE(vak): Common types of strings

local string NilString = StaticStr("");

// NOTE(vak): Character

local b32  IsLowercase(char Character);
local b32  IsUppercase(char Character);

local char ToLowercase(char Character);
local char ToUppercase(char Character);

// NOTE(vak): Conversion

local string CString(char* Data);

// NOTE(vak): Comparison

local b32 StringStartsWith(string String, string Compare);
local b32 StringEquals    (string A, string B);

// NOTE(vak): Case-insensitive comparison

local b32 StringStartsWithCI(string String, string Compare);
local b32 StringEqualsCI    (string A, string B);

// NOTE(vak): Stream

local char PeekCharacter(string* String);
local char ConsumeCharacter(string* String);
local string ConsumeStringView(string* String, usize Size);

// NOTE(vak): Formatting
//

local string FormatString (void* Buffer, usize BufferSize, string Format, ...);
local string FormatStringV(void* Buffer, usize BufferSize, string Format, va_list ArgList);

//
// TODO(vak): Support UTF8
// TODO(vak): Support printf features such as left/right pad, zero-padding or force sign printing.
//
// Available formats:
//      %s8   - 8-bit  signed integer (decimal)
//      %s16  - 16-bit signed integer (decimal)
//      %s32  - 32-bit signed integer (decimal)
//      %s64  - 64-bit signed integer (decimal)
//
//      %u8   - 8-bit  unsigned integer (decimal)
//      %u16  - 16-bit unsigned integer (decimal)
//      %u32  - 32-bit unsigned integer (decimal)
//      %u64  - 64-bit unsigned integer (decimal)
//
//      %b8   - 8-bit  unsigned integer (binary)
//      %b16  - 16-bit unsigned integer (binary)
//      %b32  - 32-bit unsigned integer (binary)
//      %b64  - 64-bit unsigned integer (binary)
//
//      %o8   - 8-bit  unsigned integer (octal)
//      %o16  - 16-bit unsigned integer (octal)
//      %o32  - 32-bit unsigned integer (octal)
//      %o64  - 64-bit unsigned integer (octal)
//
//      %x8   - 8-bit  unsigned integer (hexadecimal)
//      %x16  - 16-bit unsigned integer (hexadecimal)
//      %x32  - 32-bit unsigned integer (hexadecimal)
//      %x64  - 64-bit unsigned integer (hexadecimal)
//
//      %c    - ASCII character
//      %str  - `string` struct
//      %cstr - Null-terminated string
//
// Note that format detection is case-insensitive, so something
// like "%S8" or "%CsTr" would be still be detected as "%s8" and
// "%cstr" respectively.
//
// If the format isn't any of the specified format, then everything
// after the "%" character will be printed out. For example, if "%%"
// were to be passed then the formatter will just print out "%".
// Another example is "%asdjh", where the formatter will print out
// "asdjh".
