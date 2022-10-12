
#define MAX_CHARS 128
void DrawFPS(render_group* Group, f32 Dt, camera_settings* Camera, font* Font)
{
    char* fmt = R"###(
FPS:%f
Pitch: %f
Fov: %f
Y: %f 
Z: %f
)###";
    
    char String[MAX_CHARS]; // get rid of this hard-coded buffer
    int nBuf = sprintf_s(String, MAX_CHARS, fmt, 1.0f/Dt, Camera->Pitch,
                         Camera->Fov, Camera->Pos.y, Camera->Pos.z);
    String[nBuf] = 0;
    
    PushText(Group, Font, String,V3(0,670,-100));
}


internal void UpdateAndRenderUI(render_group* RenderGroup, game_render_commands* Video, game_input* Input, game_state* State)
{
    mat4 OrthoProj = Ortho(Video->Width,Video->Height);
    mat4 I = Identity();
    mat4 View = Translate(I, V3(10.0f,0.0f,0.0f));
    mat4 Transform2 = OrthoProj * View;
    
    //PushClear(&RenderGroupUI, V3(1.0f));
    
    PushCameraTransform(RenderGroup, Transform2);
    DrawFPS(RenderGroup, Input->Dt, &State->Camera, &State->UIFont);
    
    
}