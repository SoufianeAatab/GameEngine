#pragma once
#include "math.h"

struct opengl_program
{
    GLuint ID;
    GLuint TextureArrayID;
    GLint TransformLocation;
    GLint TextureSamplerLocation;
    GLint ColorSamplerLocation;
    
    
    GLint VertexLocation;
    GLint ColorLocation;
    GLint NormalLocation;
    GLint TextureLocation;
    GLint TextureIndexLocation;
};

struct opengl
{
    opengl_program Program;    
    opengl_program MeshProgram;
    
    GLuint Vao;
    GLuint Vbo;
    
    game_render_commands Commands;
    
    u8 PushBufferMemory[65536];
    struct vertex *VertexArray;
    
    
};
