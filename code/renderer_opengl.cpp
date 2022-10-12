#include "renderer.h"
#include "math.h"
#include "opengl.h"

#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82

#define GL_MULTISAMPLE                    0x809D
#define GL_TEXTURE_3D                     0x806F

#define GL_TEXTURE0                       0x84C0
#define GL_TEXTURE1                       0x84C1
#define GL_TEXTURE2                       0x84C2
#define GL_TEXTURE3                       0x84C3
#define GL_TEXTURE4                       0x84C4
#define GL_TEXTURE5                       0x84C5
#define GL_TEXTURE6                       0x84C6
#define GL_TEXTURE7                       0x84C7

#define GL_MAX_ARRAY_TEXTURE_LAYERS       0x88FF
#define GL_TEXTURE_2D_ARRAY               0x8C1A
#define GL_CLAMP_TO_EDGE                  0x812F

#define GL_ARRAY_BUFFER                   0x8892
#define GL_STREAM_DRAW                    0x88E0
#define GL_STREAM_READ                    0x88E1
#define GL_STREAM_COPY                    0x88E2
#define GL_STATIC_DRAW                    0x88E4
#define GL_STATIC_READ                    0x88E5
#define GL_STATIC_COPY                    0x88E6

internal u32 OpenGLCreateProgram(char *VSource, char *FSource);

internal i32 LoadShader(char *Source, GLenum Type)
{
    i32 Shader = glCreateShader(Type);
    glShaderSource(Shader, 1, &Source, NULL);
    glCompileShader(Shader);
    //OutputDebugStringA("COMPILING SHADERS");
    i32 IsCompiled;
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &IsCompiled);
    if (IsCompiled != GL_TRUE)
    {
        GLsizei LogLength = 0;
        GLchar Message[1024];
        glGetShaderInfoLog(Shader, 1024, &LogLength, Message);
        // Write the error to a log
        OutputDebugString(Message);
        return -1;
    }
    return Shader;
}

internal i32 ReloadShaders()
{
    read_file_result VS = Api.ReadFile("../data/shaders/shader.vs");
    read_file_result FS = Api.ReadFile("../data/shaders/shader.fs");
    i32 Program = OpenGLCreateProgram((char *)VS.Data, (char *)FS.Data);
    
    Api.FreeFile(VS);
    Api.FreeFile(FS);
    return Program;
}

internal u32 CreateProgram(i32 VS, i32 FS)
{
    i32 Program = glCreateProgram();
    glAttachShader(Program, VS);
    glAttachShader(Program, FS);
    GLint Success;
    char Message[512];
    
    glLinkProgram(Program);
    
    glGetProgramiv(Program, GL_LINK_STATUS, &Success);
    
    if (!Success)
    {
        glGetProgramInfoLog(Program, 512, NULL, Message);
        OutputDebugString(Message);
        return -1;
    }
    
    return Program;
}

internal u32 OpenGLCreateProgram(char *VSource, char *FSource)
{
    i32 VS = LoadShader(VSource, GL_VERTEX_SHADER);
    i32 FS = LoadShader(FSource, GL_FRAGMENT_SHADER);
    return CreateProgram(VS, FS);
}

internal opengl OpenglInit()
{
    read_file_result VS = Api.ReadFile("../data/shaders/shader.vs");
    read_file_result FS = Api.ReadFile("../data/shaders/shader.fs");
    
    opengl Opengl = {};
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_3D);
    glEnable(GL_BLEND);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    u32 ProgramID = OpenGLCreateProgram((char *)VS.Data, (char *)FS.Data);
    Assert(ProgramID != 0);
    
    opengl_program Program = {};
    Program.ID = ProgramID;
    
    glGenVertexArrays(1, &Opengl.Vao);
    glBindVertexArray(Opengl.Vao);
    glGenBuffers(1, &Opengl.Vbo);
    
    Program.VertexLocation = glGetAttribLocation(Program.ID, "position");
    Program.ColorLocation = glGetAttribLocation(Program.ID, "color");
    Program.NormalLocation = glGetAttribLocation(Program.ID, "normal");
    Program.TextureLocation = glGetAttribLocation(Program.ID, "uv");
    Program.TextureIndexLocation = glGetAttribLocation(Program.ID, "TextureIndex");
    
    glEnableVertexAttribArray(Program.VertexLocation);
    glEnableVertexAttribArray(Program.ColorLocation);
    glEnableVertexAttribArray(Program.NormalLocation);
    glEnableVertexAttribArray(Program.TextureLocation);
    glEnableVertexAttribArray(Program.TextureIndexLocation);
    
    glVertexAttribPointer(Program.VertexLocation, 3, GL_FLOAT, GL_FALSE, 
                          sizeof(vertex), (void *)offsetof(vertex, Pos));
    glVertexAttribPointer(Program.ColorLocation, 3, GL_FLOAT, GL_FALSE, 
                          sizeof(vertex), (void *)offsetof(vertex, Color));
    glVertexAttribPointer(Program.TextureLocation, 2, GL_FLOAT, GL_FALSE, 
                          sizeof(vertex), (void *)offsetof(vertex, UV));
    glVertexAttribIPointer(Program.TextureIndexLocation, 1, GL_UNSIGNED_SHORT, 
                           sizeof(vertex), (void *)offsetof(vertex, TextureIndex));
    glVertexAttribPointer(Program.NormalLocation, 3, GL_FLOAT, 
                          GL_FALSE, sizeof(vertex), (void *)offsetof(vertex, Normal));
    
    
    glUseProgram(ProgramID);
    
    i32 TransformLocation       = glGetUniformLocation(Program.ID, "Transform");
    i32 TextureSamplerLocation  = glGetUniformLocation(Program.ID, "textureSampler");
    
    Assert(TransformLocation >= 0 && TextureSamplerLocation >= 0);
    Program.TransformLocation = TransformLocation;
    Program.TextureSamplerLocation = TextureSamplerLocation;
    
    // Setting TextureArray
    glGenTextures(1,&Program.TextureArrayID);
    u32 Width = TEXTURE_ARRAY_WIDTH;
    u32 Height = TEXTURE_ARRAY_HEIGHT;
#define MAX_TEXTURES_ARRAY_COUNT 128
    
    GLint max_layers;
    glGetIntegerv (GL_MAX_ARRAY_TEXTURE_LAYERS, &max_layers);
    Api.DebugPrint("MAX ARRAY TEXTURES %d", max_layers);
    
    glBindTexture(GL_TEXTURE_2D_ARRAY, Program.TextureArrayID);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, Width, Height, MAX_TEXTURES_ARRAY_COUNT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // White bitmap for non textured quads
    i32 WhiteBitmap[4][4];
    for(u32 Y = 0; Y < 4; ++Y)
    {
        for(u32 X = 0; X < 4; ++X)
        {
            WhiteBitmap[Y][X] = 0xFFFFFFFF;
        }
    }
    
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 
                    0,  // Level
                    0,  // xoffset
                    0,  // yoffset 
                    0, // zoffset
                    4, 
                    4, 
                    1, 
                    GL_RGBA, 
                    GL_UNSIGNED_BYTE, 
                    (u8*)WhiteBitmap);
    
    Opengl.Program = Program;
    
    ///--------------------------------- mesh program
    
    read_file_result MeshVS = Api.ReadFile("../data/shaders/mesh.vs");
    read_file_result MeshFS = Api.ReadFile("../data/shaders/mesh.fs");
    
    u32 MeshProgramID = OpenGLCreateProgram((char *)MeshVS.Data, (char *)MeshFS.Data);
    Assert(MeshProgramID != 0);
    
    opengl_program MeshProgram = {};
    MeshProgram.ID = MeshProgramID;
    
    MeshProgram.VertexLocation = glGetAttribLocation(MeshProgram.ID, "position");
    MeshProgram.NormalLocation = glGetAttribLocation(MeshProgram.ID, "normal");
    MeshProgram.TextureLocation = glGetAttribLocation(MeshProgram.ID, "uv");
    
    glUseProgram(MeshProgramID);
    TransformLocation       = glGetUniformLocation(MeshProgram.ID, "Transform");
    TextureSamplerLocation  = glGetUniformLocation(MeshProgram.ID, "textureSampler");
    i32 ColorSamplerLocation    = glGetUniformLocation(MeshProgram.ID, "Color");
    
    Assert(TransformLocation >= 0);
    MeshProgram.TransformLocation = TransformLocation;
    MeshProgram.TextureSamplerLocation = TextureSamplerLocation;
    MeshProgram.ColorSamplerLocation = ColorSamplerLocation;
    
    Opengl.MeshProgram = MeshProgram;
    return Opengl;
}

internal void OpenGLInitTexturesQueue(opengl* OpenGL)
{
    OpenGL->Commands.TextureQueue.NextTextureSlotEmpty = 1;
    OpenGL->Commands.TextureQueue.Count = 0;
    OpenGL->Commands.TextureQueue.LoadedCount = 0;
}

internal void OpenGLInitMeshesQueue(opengl* OpenGL)
{
    OpenGL->Commands.MeshQueue.Count = 0;
    OpenGL->Commands.MeshQueue.LoadedCount = 0;
}

internal void OpenGLInitRenderCommands(opengl* OpenGL)
{
    OpenGL->Commands.PushBufferSize = 0;
    OpenGL->Commands.MaxPushBufferSize = Megabytes(4);
    OpenGL->Commands.VertexArray = OpenGL->VertexArray;
    OpenGL->Commands.VertexArrayOffset = 0;
}

u32 CreateVertexArray(u32 Size, v3* Vertices, v2* UV, v3* Normals, opengl_program* Program)
{
    u32 Vao;
    glGenVertexArrays(1, &Vao);
    glBindVertexArray(Vao);
    
    u32 VboVertices, VboTextures, VboNormals;
    glGenBuffers(1, &VboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, VboVertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v3) * Size, Vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(Program->VertexLocation);
    glVertexAttribPointer(Program->VertexLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glGenBuffers(1, &VboTextures);
    glBindBuffer(GL_ARRAY_BUFFER, VboTextures);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v2) * Size, UV, GL_STATIC_DRAW);
    glEnableVertexAttribArray(Program->TextureLocation);
    glVertexAttribPointer(Program->TextureLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
    
    glGenBuffers(1, &VboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, VboNormals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v3) * Size, Normals, GL_STATIC_DRAW);
    glEnableVertexAttribArray(Program->NormalLocation);
    glVertexAttribPointer(Program->NormalLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    return Vao;
}

internal void ManageTextureQueue(game_render_commands *Commands, opengl_program Program)
{
    renderer_texture_queue *Queue = &Commands->TextureQueue;
    //Assert(*Queue);
    while (Queue->LoadedCount < Queue->Count)
    {
        loaded_bitmap *Bitmap = Queue->Bitmaps + Queue->LoadedCount;
        
        glPixelStorei(GL_UNPACK_ALIGNMENT,1);
        
        
        glBindTexture(GL_TEXTURE_2D_ARRAY, Program.TextureArrayID);
        
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 
                        0,  // Level
                        0,  // xoffset
                        0,  // yoffset 
                        Bitmap->Level, // zoffset
                        Bitmap->Width, 
                        Bitmap->Height, 
                        1, 
                        Bitmap->Format, 
                        GL_UNSIGNED_BYTE, 
                        Bitmap->Pixels);
        
        
        // FREE PIXELS HERE
        Queue->LoadedCount += 1;
    }
    Queue->Count = 0;
}

internal void ManageMeshQueue(game_render_commands* Commands, opengl_program Program)
{
    renderer_mesh_queue* Queue = &Commands->MeshQueue;
    //Assert(*Queue);
    while (Queue->LoadedCount < Queue->Count)
    {
        loaded_mesh *Mesh = Queue->Meshes + Queue->LoadedCount;
        
        *Mesh->ID = CreateVertexArray(Mesh->Size, 
                                      Mesh->Vertices, 
                                      Mesh->UV, 
                                      Mesh->Normals,
                                      &Program);
        
        // FREE PIXELS HERE
        Queue->LoadedCount += 1;
    }
}

internal game_render_commands* OpenGLBeginFrame(opengl* OpenGL, u32 Width, u32 Height)
{
    
    opengl_program Program = OpenGL->Program;
    opengl_program MeshProgram = OpenGL->MeshProgram;
    
    ManageTextureQueue(&OpenGL->Commands, Program);
    ManageMeshQueue(&OpenGL->Commands, MeshProgram);
    
    OpenGL->Commands.Width = Width;
    OpenGL->Commands.Height = Height;
    
    glViewport(0, 0, OpenGL->Commands.Width, OpenGL->Commands.Height);
    
    // Buffer from where GPU will read
    OpenGL->Commands.PushBufferBase = OpenGL->PushBufferMemory;
    OpenGL->Commands.VertexArrayOffset = 0;
    OpenGL->Commands.PushBufferSize = 0;
    
    return &OpenGL->Commands;
}

internal void OpenGLEndFrame(game_render_commands *Commands, opengl *OpenGL)
{
    opengl_program Program = OpenGL->Program;
    opengl_program MeshProgram = OpenGL->MeshProgram;
    
    for (u32 Base = 0; Base < Commands->PushBufferSize;)
    {
        render_group_entry_header *Header = (render_group_entry_header *)(Commands->PushBufferBase + Base);
        Base += sizeof(*Header);
        void *Data = (u8 *)(Commands->PushBufferBase + Base);
        switch (Header->Type)
        {
            case Quads:
            {
                
                glUseProgram(Program.ID);
                Assert(OpenGL->Vao > 0 && OpenGL->Vbo > 0);
                //Assert(Program.TransformLocation >= 0);
                render_group_entry_quads *Quads = (render_group_entry_quads *)Data;
                
                // Pass directly a mat4 
                camera_transform Camera = Quads->CameraTransform;
                
                glUniformMatrix4fv(Program.TransformLocation, 1, GL_TRUE, &Camera.Transform.M[0][0]);
                
                glBindVertexArray(OpenGL->Vao);
                glUniform1i(Program.TextureSamplerLocation, 0);
                
                glBindTexture(GL_TEXTURE_2D_ARRAY, Program.TextureArrayID);
                glBindBuffer(GL_ARRAY_BUFFER, OpenGL->Vbo);
                
                // For multiple render groups
                glBufferData(GL_ARRAY_BUFFER, 
                             sizeof(vertex) * (Commands->VertexArrayOffset + Quads->Count * 6), 
                             Commands->VertexArray, 
                             GL_STREAM_DRAW);
                
                glEnableVertexAttribArray(Program.VertexLocation);
                glEnableVertexAttribArray(Program.ColorLocation);
                glEnableVertexAttribArray(Program.NormalLocation);
                glEnableVertexAttribArray(Program.TextureLocation);
                glEnableVertexAttribArray(Program.TextureIndexLocation);
                
                glVertexAttribPointer(Program.VertexLocation, 3, GL_FLOAT, GL_FALSE, 
                                      sizeof(vertex), (void *)offsetof(vertex, Pos));
                glVertexAttribPointer(Program.ColorLocation, 3, GL_FLOAT, GL_FALSE, 
                                      sizeof(vertex), (void *)offsetof(vertex, Color));
                glVertexAttribPointer(Program.TextureLocation, 2, GL_FLOAT, GL_FALSE, 
                                      sizeof(vertex), (void *)offsetof(vertex, UV));
                glVertexAttribIPointer(Program.TextureIndexLocation, 1, GL_UNSIGNED_SHORT, 
                                       sizeof(vertex), (void *)offsetof(vertex, TextureIndex));
                glVertexAttribPointer(Program.NormalLocation, 3, GL_FLOAT, 
                                      GL_FALSE, sizeof(vertex), (void *)offsetof(vertex, Normal));
                
                glDrawArrays(GL_TRIANGLES, Quads->VertexOffset, Quads->Count * 6);
                glBindVertexArray(0);
                
                // Disable VertexAttribs?
                Base += sizeof(render_group_entry_quads);
            }
            break;
            
            case Mesh:
            {
                glUseProgram(MeshProgram.ID);
                render_group_entry_mesh *MeshPack = (render_group_entry_mesh *)Data;
                
                // Pass directly a mat4 
                camera_transform Camera = MeshPack->CameraTransform;
                
                mat4 RX = RotateX(MeshPack->Rot.x);
                mat4 RY = RotateY(MeshPack->Rot.y);
                mat4 RZ = RotateZ(MeshPack->Rot.z);
                
                mat4 R = RX * RY * RZ;
                mat4 T = Translate(MeshPack->Pos);
                mat4 S = Scale(MeshPack->Scale);
                mat4 Transform = Camera.Transform * T * R * S;
                
                //Angle += 0.1f;
                
                for(u32 MeshIndex=0;MeshIndex<3;++MeshIndex)
                {
                    glUniformMatrix4fv(MeshProgram.TransformLocation, 1, GL_TRUE, &Transform.M[0][0]);
                    
                    //glBindTexture(GL_TEXTURE_2D_ARRAY, Program.TextureArrayID);
                    renderer_mesh* Mesh = MeshPack->Meshes + MeshIndex;
                    glBindVertexArray(Mesh->Vao);
                    //glUniform1i(MeshProgram.TextureSamplerLocation, 0);
                    glUniform3f(MeshProgram.ColorSamplerLocation, Mesh->Material.DiffuseColor.x,
                                Mesh->Material.DiffuseColor.y, 
                                Mesh->Material.DiffuseColor.z);
                    
                    glBindTexture(GL_TEXTURE_2D_ARRAY, Program.TextureArrayID);
                    glDrawArrays(GL_TRIANGLES, 0, Mesh->NumVert);
                    
                }
                
                Base += sizeof(render_group_entry_mesh);
            }break;
            
            case Clear:
            {
                render_group_entry_clear *EntryClear = (render_group_entry_clear *)Data;
                glClearColor(EntryClear->Color.r, EntryClear->Color.g, EntryClear->Color.b, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                Base += sizeof(render_group_entry_clear);
            }
            break;
        }
    }
    
}
