#include "renderer.h"

render_group BeginRenderGroup(game_render_commands *Commands, game_assets *Assets)
{
    render_group Result = {};
    Result.CameraTransform = {};
    Result.Commands = Commands;
    Result.Quads = 0;
    Result.Assets = Assets;
    return Result;
}

internal void *PushRenderElement(render_group *Group, size_t Size, render_group_entry_type Type)
{
    game_render_commands *Commands = Group->Commands;
    Assert(Commands->MaxPushBufferSize >= Size + sizeof(render_group_entry_header));
    
    void *Result = 0;
    render_group_entry_header *Header = (render_group_entry_header *)(Commands->PushBufferBase + Commands->PushBufferSize);
    Header->Type = Type;
    Result = ((u8 *)Header + sizeof(render_group_entry_header));
    Size = Size + sizeof(render_group_entry_header);
    Commands->PushBufferSize += Size;
    return Result;
}

renderer_texture GetWhiteTexture()
{
    renderer_texture Texture = {};
    Texture.Height = 4;
    Texture.Width = 4;
    Texture.Index = 0;
    return Texture;
}

internal void PushQuad(render_group *Group, v3 P1, v3 P2, v3 P3, v3 P4, renderer_texture Texture, v3 Color = V3(1.0f, 1.0f, 1.0f))
{
#define MAX_QUADS_PER_BATCH 64
#define VERTICES_PER_QUAD 6
    
    if (!Group->Quads)
    {
        Group->Quads = (render_group_entry_quads *)PushRenderElement(Group, sizeof(render_group_entry_quads), render_group_entry_type::Quads);
        Group->Quads->Count = 0;
        // Set camera to the current render group
        Group->Quads->CameraTransform = Group->CameraTransform;
        // VertexOffset in gpu vertex array
        Group->Quads->VertexOffset = Group->Commands->VertexArrayOffset;
    }
    
    render_group_entry_quads *Quads = Group->Quads;
    
    vertex *Vertex = Group->Commands->VertexArray + Group->Commands->VertexArrayOffset;
    
    u16 Layer = Texture.Index;
    
    f32 W = Texture.Width / TEXTURE_ARRAY_WIDTH;
    f32 H = Texture.Height / TEXTURE_ARRAY_HEIGHT;
    
    v2 MaxUv = V2(W,H);
    v3 N0, N1, N2, N3;
    v3 E21 = (P2 - P1);
    
    v3 E31 = (P3 - P1);
    
    v3 NormalDirection = Cross(E21, E31);
    v3 Normal = Normalize(NormalDirection);
    N0 = N1 = N2 = N3 = Normal;
    
    Vertex[0].Pos = P3; // MinX, MaxY
    Vertex[0].Color = Color;
    Vertex[0].UV = V2(0,MaxUv.y);
    Vertex[0].TextureIndex = Layer;
    Vertex[0].Normal = N3;
    
    Vertex[1].Pos = P4; // MaxX, MaxY
    Vertex[1].Color = Color;
    Vertex[1].UV = V2(MaxUv.x,MaxUv.y);
    Vertex[1].TextureIndex = Layer;
    Vertex[1].Normal = N0;
    
    Vertex[2].Pos = P2; // MaxX, MinY
    Vertex[2].Color = Color;
    Vertex[2].UV = V2(MaxUv.x,0);
    Vertex[2].TextureIndex = Layer;
    Vertex[2].Normal = N2;
    
    Vertex[3].Pos = P2; // MaxX, MinY
    Vertex[3].Color = Color;
    Vertex[3].UV = V2(MaxUv.x,0);
    Vertex[3].TextureIndex = Layer;
    Vertex[3].Normal = N2;
    
    Vertex[4].Pos = P1; // MinX, MinY
    Vertex[4].Color = Color;
    Vertex[4].UV = V2(0,0);
    Vertex[4].TextureIndex = Layer;
    Vertex[4].Normal = N1;
    
    Vertex[5].Pos = P3; // MinX, MaxX
    Vertex[5].Color = Color;
    Vertex[5].UV = V2(0,MaxUv.y);
    Vertex[5].TextureIndex = Layer;
    Vertex[5].Normal = N3;
    
    ++Quads->Count;
    Group->Commands->VertexArrayOffset += 6;
}

void PushClear(render_group *Group, v3 Color)
{
    render_group_entry_clear *Clear = (render_group_entry_clear *)PushRenderElement(Group, sizeof(render_group_entry_clear), render_group_entry_type::Clear);
    Clear->Color = Color;
}

void PushRect(render_group* Group, v3 Pos, v2 Dim, renderer_texture Texture, v3 Color)
{
    v2 Min = Pos.xy;
    v2 Max = Pos.xy + Dim;
    v3 P1 = V3(Min.x, Min.y, Pos.z);
    v3 P2 = V3(Max.x, Min.y, Pos.z);
    v3 P3 = V3(Min.x, Max.y, Pos.z);
    v3 P4 = V3(Max.x, Max.y, Pos.z);
    PushQuad(Group, P1, P2, P3, P4, Texture, Color);
}

void PushLine(render_group *Group, v3 From, v3 To, f32 Thickness, v3 Color)
{
    v3 Dir = To - From;
    v3 Perp = Normalize(Cross(Dir,V3(0.0,1.0,1.0))); 
    
    v3 Min = From;
    v3 Max = To;
    
    v3 P1 = Min - Perp * Thickness; // 0,0,0 - 0,1,0
    v3 P2 = Max - Perp * Thickness; // 3,0,0 - 0,1,0
    v3 P3 = Min + Perp * Thickness; // 0,0,0 + 0,1,0
    v3 P4 = Max + Perp * Thickness; // 3,0,0 + 0,1,0
    
    PushQuad(Group, P1, P2, P3 ,P4, GetWhiteTexture(), Color);
}

void PushRect(render_group *Group, v3 Pos, v2 Dim, v3 Color)
{
    renderer_texture Texture;
    Texture.Index = 0;
    Texture.Height = 4;
    Texture.Width = 4;
    
    PushRect(Group, Pos, Dim, Texture, Color);
}

void PushCube(render_group* Group, v3 Pos, v3 Dim, v3 Color, renderer_texture Texture)
{
    v3 Min = Pos - Dim;
    v3 Max = Pos + Dim;
    v3 Color2 = Color;
    {
        // Up Face
        v3 P1 = V3(Min.x, Max.y, Min.z);
        v3 P2 = V3(Max.x, Max.y, Min.z);
        v3 P3 = V3(Min.x, Max.y, Max.z);
        v3 P4 = V3(Max.x, Max.y, Max.z);
        PushQuad(Group, P1, P2, P3, P4, Texture, Color);
    }
    
    {
        // Bottom Face
        v3 P1 = V3(Min.x, Min.y, Min.z);
        v3 P2 = V3(Max.x, Min.y, Min.z);
        v3 P3 = V3(Min.x, Min.y, Max.z);
        v3 P4 = V3(Max.x, Min.y, Max.z);
        PushQuad(Group, P1, P2, P3, P4, Texture, Color2);
    }
    
    {
        // Front face
        v3 P1 = V3(Min.x, Min.y, Max.z);
        v3 P2 = V3(Max.x, Min.y, Max.z);
        v3 P3 = V3(Min.x, Max.y, Max.z);
        v3 P4 = V3(Max.x, Max.y, Max.z);
        PushQuad(Group, P1, P2, P3, P4, Texture, Color2);
    }
    
    {
        // Back face
        v3 P1 = V3(Min.x, Min.y, Min.z);
        v3 P2 = V3(Max.x, Min.y, Min.z);
        v3 P3 = V3(Min.x, Max.y, Min.z);
        v3 P4 = V3(Max.x, Max.y, Min.z);
        PushQuad(Group, P1, P2, P3, P4, Texture, Color2);
    }
    {
        // Left face
        v3 P1 = V3(Min.x, Min.y, Min.z);
        v3 P2 = V3(Min.x, Max.y, Min.z);
        v3 P3 = V3(Min.x, Min.y, Max.z);
        v3 P4 = V3(Min.x, Max.y, Max.z);
        PushQuad(Group, P1, P2, P3, P4, Texture, Color2);
    }
    
    {
        // Right face
        v3 P1 = V3(Max.x, Min.y, Min.z);
        v3 P2 = V3(Max.x, Max.y, Min.z);
        v3 P3 = V3(Max.x, Min.y, Max.z);
        v3 P4 = V3(Max.x, Max.y, Max.z);
        PushQuad(Group, P1, P2, P3, P4, Texture, Color2);
    }
}

void PushCube(render_group *Group, v3 Pos, v3 Dim, v3 Color)
{
    PushCube(Group, Pos, Dim, Color, GetWhiteTexture());
}

void PushCameraTransform(render_group *Group, mat4 Transform)
{
    Group->CameraTransform.Transform = Transform;
    Group->Quads = 0;
}

void PushBitmap(render_group *RenderGroup, renderer_texture Texture, v3 Pos, v2 Dim)
{
    v2 Min = Pos.xy;
    v2 Max = Pos.xy + Dim;
    v3 P1 = V3(Min.x, Min.y, Pos.z);
    v3 P2 = V3(Max.x, Min.y, Pos.z);
    v3 P3 = V3(Min.x, Max.y, Pos.z);
    v3 P4 = V3(Max.x, Max.y, Pos.z);
    PushQuad(RenderGroup, P1, P2, P3, P4, Texture, V3(1.0,1.0,1.0));
}

void PushMesh(render_group* Group, renderer_mesh_pack* Pack, v3 Pos, v3 Scale, v3 Rot = V3(0.0,0.0,0.0))
{
    
    render_group_entry_mesh *Mesh = (render_group_entry_mesh *)PushRenderElement(Group, sizeof(render_group_entry_mesh), render_group_entry_type::Mesh);
    
    Mesh->CameraTransform = Group->CameraTransform;
    Mesh->Meshes = Pack->Meshes;
    Mesh->NumMeshes = Pack->Size;
    Mesh->Scale = Scale;
    Mesh->Pos = Pos;
    Mesh->Rot = Rot;
}