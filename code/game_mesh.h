/* date = October 8th 2022 4:49 pm */

#ifndef GAME_MESH_H

#include "platform.h"
#include "renderer.h"

#define MAX_MESHES_PER_MODEL 8


struct renderer_mesh_pack
{
    struct renderer_mesh* Meshes;
    u32 Size;
};

struct mesh_info{
    char* Name;
    u32 NumFaces;
};

struct model_info
{
    u32 NumVertices;
    u32 NumNormals;
    u32 NumUV;
    u32 NumMeshes;
    mesh_info MeshInfo[MAX_MESHES_PER_MODEL];
};

struct mesh
{
    u32 NumVert;
    v3* Vertices;
    v3* Normals;
    v2* Textures;
    u32 ID;
    
    material Material;
    bool UseTexture;
};

struct model
{
    mesh* Meshes;
    model_info Info;
    
};

#define GAME_MESH_H

#endif //GAME_MESH_H
