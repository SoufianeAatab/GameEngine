#include "game_assets.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#define MAX_ASSETS 256

internal game_assets* AllocateGameAssets(memory_arena* Arena, game_render_commands* Commands, transient_state* TranState)
{
    game_assets* Assets = PushStruct(Arena, game_assets);
    Assets->Assets = (asset *)PushSize_(Arena, MAX_ASSETS * sizeof(asset));
    Assets->AssetCount = 0;
    
    Assets->TextureQueue = &Commands->TextureQueue;
    Assets->TextureQueue->Bitmaps = (loaded_bitmap *)PushSize_(Arena, MAX_ASSETS * sizeof(loaded_bitmap));
    Assets->TextureQueue->Count = 0;
    Assets->TextureQueue->LoadedCount = 0;
    Assets->TextureQueue->NextTextureSlotEmpty = 1;
    Assets->TranState = TranState;
    return Assets;
} 

void AddAssetType(game_assets* Assets, u32 Index, char* Path)
{
    Assets->Types[Index] = {};
    Assets->Types[Index].Info.FileName = Path;
};

void PushBitmapToGPUQueue(renderer_texture_queue* Queue, i32 Width, i32 Height, u8* Pixels, u32 Format)
{
    Assert(Queue->Count < MAX_ASSETS);
    loaded_bitmap *LoadedBitmap = Queue->Bitmaps + Queue->Count;
    LoadedBitmap->Height = Height;
    LoadedBitmap->Width = Width;
    LoadedBitmap->Pixels = Pixels;
    LoadedBitmap->Level = Queue->NextTextureSlotEmpty;
    LoadedBitmap->Format = Format;
    Queue->Count += 1;
    
}

void LoadBitmap(game_assets* Assets, u32 Index)
{
    read_file_result Data = Api.ReadFile(Assets->Types[Index].Info.FileName);
    i32 Width, Height, Comp;
    stbi_set_flip_vertically_on_load(1);
    u8 *Pixels = stbi_load_from_memory((u8*)Data.Data, Data.Size, &Width, &Height, &Comp, 0);
    
    u32 Format = 0x1908;
    if(Comp == 3)
    {
        Format = 0x1907;
    }
    else if(Comp == 1)
    {
        Format = 0x1908;
    }
    
    PushBitmapToGPUQueue(Assets->TextureQueue, Width, Height, Pixels, Format);
    asset* Asset = Assets->Assets + Index;
    Asset->Bitmap.Height = Height;
    Asset->Bitmap.Width = Width;
    Asset->Bitmap.Index = Assets->TextureQueue->NextTextureSlotEmpty;
    ++Assets->AssetCount;
    ++(Assets->TextureQueue->NextTextureSlotEmpty);
    Api.FreeFile(Data);
}

loaded_bitmap* MakeEmptyBitmap(game_assets* Assets, u32 Width, u32 Height)
{
    loaded_bitmap* Bitmap = PushStruct(&Assets->TranState->Arena, loaded_bitmap);
    Bitmap->Height = Height;
    Bitmap->Width = Width;
    Bitmap->Pixels = (u8*)PushSize_(&Assets->TranState->Arena, sizeof(u8)*Width*Height);
    Bitmap->Format = 0x1903;
    return Bitmap;
}

i32 GetAdvanceKernings(font Font, char Prev, char Next)
{
    return stbtt_GetCodepointKernAdvance(&Font.FontInfo, Prev, Next);
}

void LoadFont(game_assets* Assets, u32 Index)
{
    read_file_result Data = Api.ReadFile(Assets->Types[Index].Info.FileName);
    
    stbtt_fontinfo Font;
    stbtt_InitFont(&Font, (u8 *)Data.Data, stbtt_GetFontOffsetForIndex((u8 *)Data.Data, 0));
    asset* Asset = Assets->Assets + Index;
    Asset->Font.Index = Index;
    f32 Scale = stbtt_ScaleForPixelHeight(&Font, 36.0f);
    Asset->Font.Scale = Scale;
    stbtt_GetFontVMetrics(&Font, &Asset->Font.Ascent, &Asset->Font.Descent, &Asset->Font.LineGap);
    
    for(u32 CodePoint = 32; CodePoint < 125;++CodePoint)
    {
        glyph CodePointGlyph = {};
        
        u8 *MonoBitmap = stbtt_GetCodepointBitmap(&Font, 0, Scale,
                                                  CodePoint, &CodePointGlyph.Width, &CodePointGlyph.Height, 
                                                  &CodePointGlyph.XOffset, 
                                                  &CodePointGlyph.YOffset);
        
        stbtt_GetCodepointHMetrics(&Font, CodePoint, &CodePointGlyph.Advance, &CodePointGlyph.LeftSideBearing);
        
        
        i32 X0, X1, Y1;
        stbtt_GetCodepointBitmapBox(&Font, CodePoint, Scale, Scale, &CodePointGlyph.X0, &CodePointGlyph.Y0, &CodePointGlyph.X1, &CodePointGlyph.Y1);
        //loaded_bitmap* TempBitmap = MakeEmptyBitmap(Assets, CodePointGlyph.Width, CodePointGlyph.Height);
        //TempBitmap->Pixels = MonoBitmap;
        u32 Format = 0x1908;
        u8* MonoBitmapFlippedY = (u8*)PushSize_(&Assets->TranState->Arena, CodePointGlyph.Width*CodePointGlyph.Height*sizeof(u8)*4);
        
        // Convert form GL_RED to GL_RGBA texture
        u32 EndY = CodePointGlyph.Height - 1;
        for(u32 Y=0;Y<CodePointGlyph.Height;++Y)
        {
            for(u32 X=0;X<CodePointGlyph.Width;++X)
            {
                u8 PixelData = MonoBitmap[Y*CodePointGlyph.Width + X];
                
                MonoBitmapFlippedY[(EndY - Y) * CodePointGlyph.Width*4 + X*4 + 0] = PixelData;
                MonoBitmapFlippedY[(EndY - Y) * CodePointGlyph.Width*4 + X*4 + 1] = PixelData;
                MonoBitmapFlippedY[(EndY - Y) * CodePointGlyph.Width*4 + X*4 + 2] = PixelData;
                MonoBitmapFlippedY[(EndY - Y) * CodePointGlyph.Width*4 + X*4 + 3] = PixelData;
                
            }
        }
        PushBitmapToGPUQueue(Assets->TextureQueue, CodePointGlyph.Width, CodePointGlyph.Height, MonoBitmapFlippedY, Format);
        
        CodePointGlyph.Index = Assets->TextureQueue->NextTextureSlotEmpty;
        CodePointGlyph.Char = CodePoint;
        
        Asset->Font.Glyphs[CodePoint - 32] = CodePointGlyph;
        
        Assets->TextureQueue->NextTextureSlotEmpty += 1;
    }
    Assets->AssetCount += 1;
    
    Api.FreeFile(Data);
}

internal renderer_texture GetBitmap(game_assets *Assets, u32 Index)
{
    asset* Asset = Assets->Assets + Index;
    if(Asset->Bitmap.Width != 0 && Asset->Bitmap.Height != 0 && Asset->Bitmap.Index != 0)
    {
        return Asset->Bitmap;
    }
    // Load
    LoadBitmap(Assets, Index);
    return Asset->Bitmap;
}

internal font GetFont(game_assets *Assets, u32 Index)
{
    asset* Asset = Assets->Assets + Index;
    if(Asset->Font.Index != 0)
    {
        return Asset->Font;
    }
    // Load
    LoadFont(Assets, Index);
    return Asset->Font;
}
