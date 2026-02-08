
#pragma once

// NOTE(vak): Keywords

#define local static
#define persist static

// NOTE(vak): Macros

#define CTAssert(Expression) _Static_assert(Expression, "Compile-time assertion failed")

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#define Minimum(A, B) ((A) < (B) ? (A) : (B))
#define Maximum(A, B) ((A) < (B) ? (A) : (B))

// NOTE(vak): Integer

typedef signed char        s8;
typedef signed short       s16;
typedef signed int         s32;
typedef signed long long   s64;

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

typedef s64 ssize;
typedef u64 usize;

CTAssert(sizeof(s8 ) == 1);
CTAssert(sizeof(s16) == 2);
CTAssert(sizeof(s32) == 4);
CTAssert(sizeof(s64) == 8);

CTAssert(sizeof(u8 ) == 1);
CTAssert(sizeof(u16) == 2);
CTAssert(sizeof(u32) == 4);
CTAssert(sizeof(u64) == 8);

CTAssert(sizeof(ssize) == sizeof(void*));
CTAssert(sizeof(usize) == sizeof(void*));

// NOTE(vak): Floating point

typedef float  f32;
typedef double f64;

CTAssert(sizeof(f32) == 4);
CTAssert(sizeof(f64) == 8);

// NOTE(vak): Booleans

typedef u8  b8;
typedef u16 b16;
typedef u32 b32;
typedef u64 b64;

// NOTE(vak): Constants

#define true  (1)
#define false (0)

// NOTE(vak): String

typedef struct
{
    char* Data;
    usize Size;
} string;

#define StrData(Data, Size) (string){Data, Size}
#define Str(Literal)        (string){Literal, sizeof(Literal) - 1}
