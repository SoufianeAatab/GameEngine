#pragma once
#include <stdint.h>
#include <math.h>
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef i32 b32;
typedef float f32;
typedef double f64;

#define global_variable static
#define local_persist static
#define internal static

#define ArrayCount(arr) (sizeof((arr)) / (sizeof((arr)[0])))
#define Kilobytes(number) ((number) * 1024ull)
#define Megabytes(number) (Kilobytes(number) * 1024ull)
#define Gigabytes(number) (Megabytes(number) * 1024ull)
#define Terabytes(number) (Gigabytes(number) * 1024ull)
#define Assert(expression) if(!(expression)) { *(int*) 0 = 0; }

struct read_file_result
{
    i32 Size;
    void* Data;
};

struct platform_api
{
    read_file_result (*ReadFile)(char* FileName);
    void (*FreeFile)(read_file_result);
    void* (*AllocMem)(u32);
    void (*FreeMem)(void*, u32);
    void (*CopyMem)(void*, void*,  u32);
    void (*DebugPrint)(char* fmt, ...);
    
};


struct game_memory {
    b32 IsInitialized;
    
    size_t PermanentStorageSize;
    void *PermanentStorage; // required to be cleared to zero
    
    size_t TransientStorageSize;
    void *TransientStorage;
    
    platform_api* Platform;
};

struct clear_color{
    f32 red;
    f32 green;
    f32 blue;
};

struct game_mouse_button
{
    b32 IsDown;
};

struct game_button_state{
    bool IsDown;
};

struct game_keyboard
{
    union{
        game_button_state Buttons[8];
        struct {
            game_button_state W;
            game_button_state S;
            game_button_state A;
            game_button_state D;
            game_button_state Y;
            game_button_state U;
            game_button_state O;
            game_button_state P;
        };
    };
};

struct game_input
{
    f32 Dt;
    i32 MouseX;
    i32 MouseY;
    i32 MouseZ;
    game_mouse_button MouseButtons[5]; 
    game_keyboard Keys;
};

i32 ParseInt(char* Data)
{
    i32 Result = 0;
    b32 IsNegative = 1;
    // remove initial spaces
    while(*(Data) == ' ')
    {
        ++Data;
    }
    if(*Data == '-') 
    {
        ++Data;
        IsNegative = -1;
    }
    if(*Data == '+') ++Data;
    while(*Data)
    {
        Result = (Result<<3) + (Result<<1) + (*Data) - '0';
        ++Data;
    }
    return Result * IsNegative;
}

f64 ParseFloat(char* Data)
{
    return atof(Data);
    i32 K = 0;
    b32 IsNegative = 1;
    // remove initial spaces
    while(*(Data) == ' ') ++Data;
    
    if(*Data == '-') {
        IsNegative = -1;
        ++Data;
    }
    if(*Data == '+') ++Data;
    
    while(*Data && *Data != '.')
    {
        K = (K<<3) + (K<<1) + (*Data) - '0';
        ++Data;
    }
    if(*Data && *Data == '.'){
        ++Data;
        
        i32 AfterComma = 0;
        i32 Divider = 1;
        while(*Data)
        {
            if(*Data == 'E' || *Data == 'e') break;
            AfterComma = (AfterComma<<3) + (AfterComma<<1) + (*Data) - '0';
            Divider = (Divider << 3) + (Divider << 1);
            ++Data;
        }
        if(*Data == 'E' || *Data == 'e') 
        {
            u32 Exp = ParseInt(Data);
            //Divider += Pow(10, Abs(Exp));
        }
        f64 Result = K + ((f64)AfterComma / (f64)Divider);
        return Result * IsNegative;
    }
    
    return K;
}

u32 Len(char* String)
{
    u32 Len = 0;
    while(*String){ ++Len; ++String;};
    
    return Len;
}

i32 Find(char* String, char* ToFind)
{
    u32 LenA = Len(String);
    u32 LenB = Len(ToFind);
    
    if(LenB > LenA) return -1;
    
    u32 J = 0;
    
    for(u32 I = 0; I < LenA; ++I)
    {
        if(String[I] == ToFind[J]){
            J++;
        }
        else if(J == LenB)
        {
            return I - J;
        }
        else 
        {
            J = 0;
        }
    }
    
    return -1;
}


b32 StringEquals(char* A, char* B)
{
    while(*A && *B)
    {
        if(*A != *B) return false;
        ++A;
        ++B;
    }
    
    return *A == *B;
}

struct game_render_commands;
extern platform_api Api;

#define GAME_UPDATE_VIDEO(name) void name(game_memory* Memory, game_render_commands* Video, game_input* Input)
typedef void game_update_video(game_memory* Memory, game_render_commands* Video, game_input* Input);
