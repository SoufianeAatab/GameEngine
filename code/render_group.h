#pragma once
#include "math.h"
#include "game_assets.h"

#define TEXTURE_ARRAY_WIDTH 512.0f
#define TEXTURE_ARRAY_HEIGHT 512.0f

enum render_group_entry_type
{
    Quads,
    Clear,
    Rect,
    Cube,
    Mesh,
};

struct render_group_entry_header
{
    render_group_entry_type Type;
};

struct render_group_entry_setup
{
    mat4 Proj;
};

struct render_group_entry_clear
{
    v3 Color;
};

struct render_group_entry_rect
{
    v3 Color;
    v2 Dim;
    v2 Pos;
};

struct render_group_entry_cube
{
    v3 Color;
    v3 Dim;
    v3 Pos;
};

struct camera_transform
{
    mat4 Transform;
};


struct material{
    v3 DiffuseColor;
};

struct renderer_mesh
{
    u32 Vao;
    u32 NumVert;
    material Material;
};

struct render_group_entry_mesh
{
    camera_transform CameraTransform;
    
    renderer_mesh* Meshes;
    u32 NumMeshes;
    
    v3 Scale;
    v3 Pos;
    v3 Rot;
};

struct render_group_entry_quads
{
    camera_transform CameraTransform;
    i32 Count;
    i32 VertexOffset;
};

struct vertex
{
    v3 Pos;
    v3 Color;
    v3 Normal;
    v2 UV;
    u16 TextureIndex;
};

struct loaded_bitmap
{
    u16 Width;
    u16 Height;
    u8 *Pixels;
    u16 Level;
    u32 Format;
};

struct loaded_mesh
{
    u32 Size;
    v3* Vertices;
    v3* Normals;
    v2* UV;
    
    u32* ID;
};

struct renderer_mesh_queue
{
    i32 Count;
    i32 LoadedCount;
    
    loaded_mesh* Meshes;
};

struct renderer_texture_queue
{
    i32 Count;
    i32 LoadedCount;
    u32 NextTextureSlotEmpty;
    
    loaded_bitmap* Bitmaps;
};

struct game_render_commands
{
    int Width;
    int Height;
    // Inmediate Mode data
    u32 MaxPushBufferSize;
    u32 PushBufferSize;
    u8 *PushBufferBase;
    u32 VertexArrayOffset;
    vertex *VertexArray;
    
    // Texture Queue
    renderer_texture_queue TextureQueue;
    renderer_mesh_queue MeshQueue;
};

struct render_group
{
    camera_transform CameraTransform;
    game_render_commands *Commands;
    
    render_group_entry_quads *Quads;
    game_assets *Assets;
};