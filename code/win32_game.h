#pragma once

#define WIN32_MAX_PATH MAX_PATH

struct win32_state
{
    char EXEPath[WIN32_MAX_PATH];
    int EXEDirLength;
};

struct win32_game_code {
    HMODULE GameDLL;
    FILETIME  GameDLLLastWriteTime;
    FILETIME  GameVShaderLastWriteTime;
    FILETIME  GameFShaderLastWriteTime;
    bool IsValid;
    game_update_video *GameUpdateVideo;
};

struct win32_window_dimension
{
    int Width;
    int Height;
};

/*

u64 Hash(char *String)
{
    u64 Hash = 5381;
    i32 C = 0;
    while (C = *String++)
        Hash = ((Hash << 5) + Hash) + C;
    return Hash;
}

struct joint
{
    i32 ParentID;
    v3 Position;
    v3 Orientation;
    char *Name;
};

char *ParseJoints(char *Data)
{
    joint Joints[33];
    i32 i = 0;
    char *Next = Consume(Data);
    Data = Next;
    do
    {
        Next = Consume(Data);
        Joints[i].Name = Data;
        Data = Next;
        Data = ReadInt(Data, &Joints[i].ParentID);
        // (
        Data = Consume(Data);
        Data = ReadV3(Data, &Joints[i].Position);
        // )
        Data = Consume(Data);
        // (
        Data = Consume(Data);
        Data = ReadV3(Data, &Joints[i].Orientation);
        // )
        Data = Consume(Data);
        
        Data = ConsumeToNewLine(Data);
        ++i;
    } while (*Data != '}');
    
    return Data;
}

char *ParseMesh(char *Data)
{
    joint Joints[33];
    i32 i = 0;
    char *Next = Consume(Data);
    Data = Next;
    do
    {
        Next = Consume(Data);
        Joints[i].Name = Data;
        Data = Next;
        Data = ReadInt(Data, &Joints[i].ParentID);
        // (
        Data = Consume(Data);
        Data = ReadV3(Data, &Joints[i].Position);
        // )
        Data = Consume(Data);
        // (
        Data = Consume(Data);
        Data = ReadV3(Data, &Joints[i].Orientation);
        // )
        Data = Consume(Data);
        
        Data = ConsumeToNewLine(Data);
        ++i;
    } while (*Data != '}');
    
    return Data;
}
*/