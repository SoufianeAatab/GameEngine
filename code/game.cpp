#include "game.h"
#include "renderer.cpp"
#include "game_assets.cpp"
#include "game_mesh.cpp"

enum asset_type_id
{
    WhiteBitmap,
    GrassBitmap,
    ArialFont,
    Tile,
    BitmapTypeCount
};

void SetCameraTransform(camera_settings *Camera)
{
    Camera->Pitch = 0.3* 180;
    Camera->Yaw = 0;
    Camera->Pos = V3(0.0f, 24.0f, 12.0f);
    Camera->Fov = 30.0f;
}

void UpdateCamera(game_input *Input, game_state *State)
{
    
    /*if (Input->MouseButtons[0].IsDown)
    {
        if (Input->MouseX > State->LastMouseX)
        {
            State->Camera.Yaw += Input->Dt * 50.0f;
        }
        else if (Input->MouseX < State->LastMouseX)
        {
            State->Camera.Yaw -= Input->Dt * 50.0f;
        }
        
        if (Input->MouseY > State->LastMouseY)
        {
            State->Camera.Pitch += Input->Dt * 10.0f;
        }
        else if (Input->MouseY < State->LastMouseY)
        {
            State->Camera.Pitch -= Input->Dt * 10.0f;
        }
        
        State->LastMouseX = Input->MouseX;
        State->LastMouseY = Input->MouseY;
    }
    */
    if (Input->Keys.O.IsDown)
    {
        State->Camera.Pitch += 10.0f * Input->Dt;
    }
    else if (Input->Keys.P.IsDown)
    {
        State->Camera.Pitch -= 10.0f * Input->Dt;
    }
    
    if (Input->Keys.Y.IsDown)
    {
        State->Camera.Fov += 10.0f * Input->Dt;
    }
    else if (Input->Keys.U.IsDown)
    {
        State->Camera.Fov -= 10.0f * Input->Dt;
    }
    
    if (Input->Keys.W.IsDown)
    {
        State->Camera.Pos.y += 10.0f * Input->Dt;
    }
    
    if (Input->Keys.S.IsDown)
    {
        State->Camera.Pos.y -= 10.0f * Input->Dt;
    }
    
    
    if (Input->Keys.A.IsDown)
    {
        State->Camera.Pos.z += 10.0f * Input->Dt;
    }
    
    if (Input->Keys.D.IsDown)
    {
        State->Camera.Pos.z -= 10.0f * Input->Dt;
    }
}

void UpdatePlayer(game_input *Input, entity *Player)
{
    v3 ddP = V3(0.0f);
    /*if (Input->Keys.A.IsDown)
    {
        ddP.x = -1.0f;
    }
    
    if (Input->Keys.D.IsDown)
    {
        ddP.x = 1.0f;
    }
    
    if (Input->Keys.W.IsDown)
    {
        ddP.y = 1.0f;
    }
    
    if (Input->Keys.S.IsDown)
    {
        ddP.y = -1.0f;
    }*/
    
    ddP = ddP * 10.0f;
    
    ddP += -Player->dP;
    
    Player->P = 0.5f * ddP * Square(Input->Dt) + Player->dP * Input->Dt + Player->P;
    Player->dP = ddP * Input->Dt + Player->dP;
}

void PushTerrain(render_group* RenderGroup)
{
    for(f32 X=-30.0f;X<30.0f; X += 4.0)
    {
        for(f32 Y=-15.0f;Y<15.0f; Y += 4.0)
        {
            PushCube(RenderGroup, V3(X,0.0,Y), V3(2.0f,2.0f,2.0f), V3(1.0,1.0,0.5), 
                     GetBitmap(RenderGroup->Assets, Tile));
        }
    }
}

void PushText(game_state* State, render_group* Group, char* String, v3 Pos)
{
    font Font = GetFont(State, Group->Assets, ArialFont);
    u32 I = 0;
    f32 X = Pos.x;
    f32 Y = Pos.y;
    
    u32 LineHeight = Font.Scale * ( Font.Ascent - Font.Descent);
    while(*String)
    {
        if(*String == '\n') 
        {
            Y -= LineHeight + Font.LineGap*Font.Scale;
            X = Pos.x;
            ++String;
            continue;
        }
        //OutputDebugStringA("ENTRO?\n");
        renderer_texture Texture = {};
        glyph Glyph = Font.Glyphs[*String - ' '];
        Texture.Width = Glyph.Width;
        Texture.Height = Glyph.Height;
        Texture.Index = Glyph.Index;
        
        i32 Advance = 0;
        if(*(String+1)) Advance = GetAdvanceKernings(Font, *String, *(String+1));
        //v3 CharOffset = V3(0, BaseLine, 0.0f);
        v3 Position = V3(X + Glyph.X0, Y - Glyph.Y1, -5.0f);
        
        PushBitmap(Group, Texture, Position, V2(Glyph.Width, Glyph.Height));
        X += (Glyph.Advance + Advance) * Font.Scale;
        ++String;
    }
    
}

void DrawFPS(game_state* State, render_group* Group, f32 Dt, camera_settings* Camera)
{
    font Font = GetFont(State, Group->Assets, ArialFont);
    char* fmt = R"###(
FPS:%f
Pitch: %f
Fov: %f
Y: %f 
Z: %f
)###";
#define MAX_CHARS 128
    
    char String[MAX_CHARS]; // get rid of this hard-coded buffer
    int nBuf = sprintf_s(String, MAX_CHARS, fmt, 1.0f/Dt, Camera->Pitch,
                         Camera->Fov, Camera->Pos.y, Camera->Pos.z);
    String[nBuf] = 0;
    
    PushText(State, Group, String,V3(0,670,-100));
}
#include <time.h>

void UpdateParticles(game_state* State, render_group* Group, f32 Dt)
{
    f32 a = 3.0f;
    for(u32 Index=0;Index<4;++Index)
    {
        particle* Particle = State->Particles + State->NextParticle++;
        if(State->NextParticle >= NUM_PARTICLES)
        {
            State->NextParticle = 0;
        }
        
        f32 x = (((float)rand()/(float)(RAND_MAX)) * a) - a/2.0f;
        f32 y = (((float)rand()/(float)(RAND_MAX)) * a);
        
        Particle->P = V2(0,4);
        Particle->dP = V2(0,0);
        Particle->Life = 1.0f;
        
        Particle->ddP = V2(x, y*10.0f);
        
    }
    
    for(u32 Index=0;Index<NUM_PARTICLES;++Index)
    {
        particle* Particle = State->Particles + Index;
        
        Particle->dP = Particle->ddP * Dt + Particle->dP;
        Particle->P = Particle->P + Particle->dP * Dt + Particle->ddP * Square(Dt);
        
        PushCube(Group, V3(Particle->P.x, Particle->P.y, 0.0f), V3(0.5,0.5,0.5), V3(0.9,0.12,0.6));
    }
    
}

u32 Used = 0;
f32 T = 0;


// void GameUpdateVideo(game_memory* Memory, game_render_commands* Video, platform_api* PlatformApi)
extern "C" GAME_UPDATE_VIDEO(GameUpdateVideo)
{
    game_state *State = (game_state *)Memory->PermanentStorage;
    transient_state *TransientState = (transient_state *)Memory->TransientStorage;
    Api = *Memory->Platform;
    
    if (!Memory->IsInitialized)
    {
        srand((unsigned int)time(NULL));
        
        Memory->IsInitialized = true;
        // State->Api = PlatformApi;
        InitializeArena(&TransientState->Arena,
                        Memory->TransientStorageSize - sizeof(transient_state),
                        (u8 *)Memory->TransientStorage + sizeof(transient_state));
        
        InitializeArena(&State->Arena,
                        Memory->PermanentStorageSize - sizeof(game_state),
                        (u8 *)Memory->PermanentStorage + sizeof(game_state));
        
        // Allocate game assets
        TransientState->Assets = AllocateGameAssets(&TransientState->Arena, Video, TransientState);
        State->Player.P = V3(0, 0, 1.0f);
        State->Player.dP = V3(0.0f);
        
        SetCameraTransform(&State->Camera);
        
        AddAssetType(TransientState->Assets, GrassBitmap, "../data/grass.png");
        AddAssetType(TransientState->Assets, ArialFont, "../data/arial.ttf");
        AddAssetType(TransientState->Assets, Tile, "../data/tile3.png");
        
        State->Model = LoadModel(&TransientState->Arena, "../data/obj/weapon_blaster.obj");
        State->Pack = SetupModel(&TransientState->Arena, State->Model, Video);
    }
    
    
    Used = TransientState->Arena.Used;
    entity *Player = &State->Player;
    render_group RenderGroup = BeginRenderGroup(Video, TransientState->Assets);
    
    f32 AspectRatio = (f32)Video->Width / (f32)Video->Height;
    mat4 PerspProj = Perspective(AspectRatio, State->Camera.Fov);
    //OrthoProj = Ortho(Video->Width,Video->Height);
    
    mat4 Rot = RotateX(Radians(State->Camera.Pitch));
    Rot = Rot * RotateY(Radians(State->Camera.Yaw));
    mat4 Cam = CameraTransform(GetCol(Rot, 0), GetCol(Rot, 1),GetCol(Rot, 2), State->Camera.Pos);
    
    mat4 Lookat = LookAt(State->Camera.Pos, V3(0.0,0.0,0.0), V3(0.0,1.0,0.0));
    
    mat4 Transform = PerspProj * Cam;
    
    PushCameraTransform(&RenderGroup, Transform);
    
    UpdateCamera(Input, State);
    UpdatePlayer(Input, Player);
    
    PushClear(&RenderGroup, V3(0.5f));
    
    PushTerrain(&RenderGroup);
    
    PushMesh(&RenderGroup, State->Pack, 
             V3(5.0f,2.0f,5.0f), 
             V3(5.0f,5.0f,5.0f),
             V3(0.0f,M_PI/2.0f,0.0f));
    PushMesh(&RenderGroup, State->Pack, V3(5.0f,2.0f,0.0f), V3(5.0f,5.0f,5.0f));
    PushMesh(&RenderGroup, State->Pack, V3(0.0f,2.0f,5.0f), V3(5.0f,5.0f,5.0f));
    PushMesh(&RenderGroup, State->Pack, V3(0.0f,2.0f,0.0f), V3(5.0f,5.0f,5.0f));
    
    
    //UpdateParticles(State, &RenderGroup, Input->Dt);
    
    // Player
    //PushCube(&RenderGroup, Player->P, V3(0.2f, 0.2f, 0.2f), V3(1.0f, 1.0f, 0.0f));
    
    //PushCube(&RenderGroup, V3(3.0f,0.0f+0.3f*cosf(T),1.0f), V3(0.1,0.1,0.1), V3(1.0,0.66,0.4));
    
    
    // UI
    mat4 OrthoProj = Ortho(Video->Width,Video->Height);
    mat4 I = Identity();
    mat4 View = Translate(I, V3(10.0f,0.0f,0.0f));
    mat4 Transform2 = OrthoProj * View;
    
    //PushClear(&RenderGroupUI, V3(1.0f));
    
    PushCameraTransform(&RenderGroup, Transform2);
    DrawFPS(State,&RenderGroup, Input->Dt, &State->Camera);
    
    // Reset memory
    
    //TransientState->Arena.Used = Used;
    T += Input->Dt * 4;
    if(T>2*M_PI)
    {
        T -= 2*M_PI;
    }
    //if(T>1) T = 0;
    //TransientState->Arena.Base = (u8 *)Memory->TransientStorage + sizeof(transient_state);
    
    
}