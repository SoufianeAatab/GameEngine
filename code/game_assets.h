#pragma once

#include "stb_truetype.h"

struct renderer_texture
{
    u16 Width;
    u16 Height;
    u16 Index;
};

struct glyph{
    i32 Width;
    i32 Height;
    i32 XOffset;
    i32 YOffset;
    i32 Advance;
    i32 LeftSideBearing;
    u32 Index;
    i32 YDiff;
    i32 X0, X1, Y0, Y1;
    char Char;
};

struct font
{
    glyph Glyphs[96];
    u32 Index;
    f32 Scale;
    i32 LineGap;
    
    i32 Ascent;
    i32 Descent;
    stbtt_fontinfo FontInfo;
};

struct asset
{
    renderer_texture Bitmap;
    font Font;
    
};

struct asset_bitmap_info
{
    char* FileName;
};

struct asset_type
{
    asset_bitmap_info Info;
};



#define MAX_ASSET_TYPES 64
struct game_assets
{
    u32 AssetCount;
    asset* Assets;
    
    u32 TypeCount;
    asset_type Types[MAX_ASSET_TYPES];
    struct renderer_texture_queue* TextureQueue;
    struct renderer_mesh_queue* MeshQueue;
    struct transient_state *TranState;
    
};
