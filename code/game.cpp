#include "game.h"
#include "renderer.cpp"
#include "game_mesh.cpp"
#include "game_particle.cpp"
#include "game_ui.cpp"
enum asset_type_id
{
    WhiteBitmap,
    GrassBitmap,
    ArialFont,
    Tile,
    YellowTile,
    Stone,
    BitmapTypeCount
};

mat4 GetTransform(u32 Width, u32 Height, camera_settings Camera)
{
    f32 AspectRatio = (f32)Width / (f32)Height;
    mat4 PerspProj = Perspective(AspectRatio, Camera.Fov);
    
    mat4 Rot = RotateX(Radians(Camera.Pitch));
    Rot = Rot * RotateY(Radians(Camera.Yaw));
    mat4 Cam = CameraTransform(GetCol(Rot, 0), GetCol(Rot, 1),GetCol(Rot, 2), Camera.Pos);
    
    mat4 Lookat = LookAt(Camera.Pos, V3(0.0,0.0,0.0), V3(0.0,1.0,0.0));
    
    return PerspProj * Cam;
}

void SetCameraTransform(camera_settings *Camera)
{
    Camera->Pitch = 0.3* 180;
    Camera->Yaw = 0;
    Camera->Pos = V3(0.0f, 33.0f, 24.0f);
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
        State->Camera.Pos.z += 10.0f * Input->Dt;
    }
    else if (Input->Keys.P.IsDown)
    {
        State->Camera.Pos.z -= 10.0f * Input->Dt;
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
    
    
    if (Input->Keys.D.IsDown)
    {
        State->Camera.Pos.x += 10.0f * Input->Dt;
    }
    
    if (Input->Keys.A.IsDown)
    {
        State->Camera.Pos.x -= 10.0f * Input->Dt;
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

void PushTerrain(render_group* RenderGroup, u8 TileMap[12][16])
{
    for(u32 X=0;X<16;++X)
    {
        for(u32 Y=0;Y<12;++Y)
        {
            if(TileMap[Y][X] == 1)
            {
                PushCube(RenderGroup, V3(-26 + X * 4.0f ,0.0,-14 + Y*4.0f), V3(2.0f,2.0f,2.0f), V3(1.0,1.0,0.5), 
                         GetBitmap(RenderGroup->Assets, YellowTile));
            }
            else if(TileMap[Y][X] == 0)
            {
                PushCube(RenderGroup, V3(-26 + X * 4.0f ,0.0,-14 + Y*4.0f), V3(2.0f,2.0f,2.0f), V3(1.0,1.0,0.5), 
                         GetBitmap(RenderGroup->Assets, Stone));
            } 
            else 
            {
                PushCube(RenderGroup, V3(-26 + X * 4.0f ,0.0,-14 + Y*4.0f), V3(2.0f,2.0f,2.0f), V3(1.0,1.0,0.5), 
                         GetBitmap(RenderGroup->Assets, Tile));
            }
        }
    }
    
}

u8 TileMap[12][16] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1},
    {1,1,1,1,1,1,1,2,2,2,1,1,0,1,1,1},
    {1,1,1,1,1,1,1,2,2,2,1,1,1,0,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1},
};
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
        AddAssetType(TransientState->Assets, YellowTile, "../data/yellow_tile.jpg");
        AddAssetType(TransientState->Assets, Stone, "../data/stone.png");
        
        State->Model = LoadModel(&TransientState->Arena, "../data/obj/weapon_blaster.obj");
        State->Pack = SetupModel(&TransientState->Arena, State->Model, Video);
        State->UIFont = GetFont(TransientState->Assets, ArialFont);
        
        
        
        //State->TileMap = PushSize_(&TransientState->Arena, 32*32 * sizeof(u8));
        //InitTileMap(State->TileMap);
    }
    
    
    //State->Used = TransientState->Arena.Used;
    entity *Player = &State->Player;
    render_group RenderGroup = BeginRenderGroup(Video, TransientState->Assets);
    
    mat4 Transform = GetTransform(Video->Width, Video->Height, State->Camera);
    PushCameraTransform(&RenderGroup, Transform);
    
    UpdateCamera(Input, State);
    UpdatePlayer(Input, Player);
    
    PushClear(&RenderGroup, V3(0.5f));
    PushTerrain(&RenderGroup, TileMap);
    PushMesh(&RenderGroup, State->Pack, 
             V3(5.0f,2.0f,2.0f), 
             V3(5.0f,5.0f,5.0f),
             V3(0.0f,M_PI/2.0f,0.0f));
    
    // UI
    UpdateAndRenderUI(&RenderGroup, Video, Input, State);
}