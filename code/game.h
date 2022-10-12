#pragma once

#include "platform.h"
#include "math.h"
#include "game_assets.h"
#include "game_mesh.h"
global_variable platform_api Api;

struct memory_arena
{
    size_t Used;
    size_t Size;
    u8 *Base;
};

struct camera_settings
{
    v3 Pos;
    v3 Target;
    f32 Fov;
    f32 Pitch;
    f32 Yaw;
};

struct particle
{
    v2 P;
    v2 dP;
    v2 ddP;
    f32 Life;
};

struct entity
{
    v3 P;
    v3 dP; // Velocity
    v3 TargetP;
};

#define NUM_PARTICLES 256

struct game_state
{
    i32 LastMouseX;
    i32 LastMouseY;
    
    camera_settings Camera;
    entity Player;
    
    u32 NextParticle;
    particle Particles[NUM_PARTICLES];
    
    memory_arena Arena;
    model* Model;
    renderer_mesh_pack* Pack;
    font UIFont;
    
};

struct transient_state
{
    memory_arena Arena;
    game_assets* Assets;
};

inline void InitializeArena(memory_arena *Arena, size_t Size, void *Base)
{
    Arena->Size = Size;
    Arena->Base = (u8 *)Base;
    Arena->Used = 0;
}

#define PushStruct(Arena, Type) (Type *)PushSize_(Arena, sizeof(Type))
inline void *PushSize_(memory_arena *Arena, size_t SizeToReserve)
{
    Assert(Arena->Used + SizeToReserve <= Arena->Size);
    u8 *Result = Arena->Base + Arena->Used;
    Arena->Used += SizeToReserve;
    return Result;
}
