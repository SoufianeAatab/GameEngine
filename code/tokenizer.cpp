#pragma once
void ReplaceSlashByNull(char* Data)
{
    char* PtrFace = Data; 
    while(*PtrFace != '/')
    {
        ++PtrFace;
    }
    
    *PtrFace = 0;
}

char* SkipSpecialChars(char* Data)
{
    // we want to skip all special characters, like tabs, spaces or newlines
    while (*Data && *Data == '\0' || *Data == ' ' || *Data == '\n' || *Data == '\r' || *Data == '\t')
        ++Data;
    
    return Data;
}

char *Consume(char *Data)
{
    // EOF
    if (!*Data) return NULL;
    
    Data = SkipSpecialChars(Data);
    if (!*Data) return NULL;
    while (*Data && *Data != ' ' && *Data != '\t' && *Data != '\r' && *Data != '\n') ++Data;
    *Data = '\0';
    ++Data;
    Data = SkipSpecialChars(Data);
    return Data;
}

char *GetLine(char *Data)
{
    while (*Data != '\r' && *Data != '\n' && *Data != '\t')
    {
        ++Data;
    }
    *Data = 0;
    ++Data;
    Data = SkipSpecialChars(Data);
    return Data;
}

char *ConsumeToNewLine(char *Data)
{
    while (*Data != '\r' && *Data != '\n')
    {
        ++Data;
    }
    while (*Data == '\r' || *Data == '\n' || *Data == ' ' || *Data == '\t')
    {
        *Data = '\0';
        ++Data;
    }
    return Data;
}


char *ReadFloat(char *Data, f32 *X)
{
    char *Next = Consume(Data);
    *X = ParseFloat(Data);
    return Next;
}

char *ReadInt(char *Data, i32 *X)
{
    char *Next = Consume(Data);
    *X = ParseInt(Data);
    return Next;
}

char *ReadV3(char *Data, v3 *V)
{
    char *Next;
    Next = ReadFloat(Data, &V->x);
    Data = Next;
    Next = ReadFloat(Data, &V->y);
    Data = Next;
    Next = ReadFloat(Data, &V->z);
    return Next;
}
