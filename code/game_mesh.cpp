#include "game_mesh.h"
#include "tokenizer.cpp"

renderer_mesh_pack* SetupModel(memory_arena* Arena, model* Model, game_render_commands* Commands)
{
    renderer_mesh_pack* Pack = (renderer_mesh_pack*) PushStruct(Arena, renderer_mesh_pack);
    Pack->Meshes = (renderer_mesh*) PushSize_(Arena, sizeof(renderer_mesh) * Model->Info.NumMeshes);
    Pack->Size = Model->Info.NumMeshes;
    if(!Commands->MeshQueue.Meshes)
    {
        Commands->MeshQueue.Meshes = (loaded_mesh*)PushSize_(Arena, sizeof(loaded_mesh) * 32);
        Commands->MeshQueue.Count = 0;
        Commands->MeshQueue.LoadedCount = 0;
    }
    
    for(u32 Index=0;Index<Model->Info.NumMeshes;++Index)
    {
        mesh* Mesh = Model->Meshes + Index;
        loaded_mesh* MeshQueue = Commands->MeshQueue.Meshes + Commands->MeshQueue.Count;
        MeshQueue->Size = Mesh->NumVert;
        MeshQueue->Vertices = Mesh->Vertices;
        MeshQueue->UV = Mesh->Textures;
        MeshQueue->Normals = Mesh->Normals;
        
        MeshQueue->ID = &Pack->Meshes[Index].Vao;
        Pack->Meshes[Index].NumVert = Mesh->NumVert;
        Pack->Meshes[Index].Material = Mesh->Material;
        Commands->MeshQueue.Count += 1;
        //CreateVertexArray(Mesh->NumVert, Mesh->Vertices, Mesh->Textures, Mesh->Normals); 
    }
    
    return Pack;
}

u32 GetNumVerticesPerFace(char* Data)
{
    u32 VerticesPerFace = 0;
    //DebugPrint(Data);
    return 1;
}

void CalcIndices(/*v3* Vertices, v2* UV, v3* Normals*/)
{
    
}

model_info GetInfo(char* Data)
{
    model_info Info = {};
    u32 CurrentMesh = -1;
    char* Next = Data;
    while(*Data)
    {
        Next = Consume(Data);
        if(StringEquals("v", Data))
        {
            ++Info.NumVertices;
        }
        
        if(StringEquals("vn", Data))
        {
            ++Info.NumNormals;
        }
        
        if(StringEquals("vt", Data))
        {
            ++Info.NumUV;
        }
        if(StringEquals("usemtl", Data))
        {
            ++CurrentMesh;
            Data = Consume(Data); 
            Info.MeshInfo[CurrentMesh].Name = Data;
            Next = Data;
        }
        if(StringEquals("f", Data))
        {
            ++Info.MeshInfo[CurrentMesh].NumFaces;
        }
        Data = Next;
    }
    
    Info.NumMeshes = CurrentMesh + 1;
    return Info;
}

material* ParseMTL(memory_arena* Arena, char* Path, u32 NumMeshes)
{
    material* Materials = (material*)PushSize_(Arena, sizeof(material) * NumMeshes);
    u32 MaterialIndex = -1;
    char* Next;
    read_file_result File = Api.ReadFile(Path);
    if(File.Size == 0) return Materials;
    char* Data = (char*)File.Data;
    
    while(*Data)
    {
        material* Mat = Materials + MaterialIndex;
        Next = Consume(Data);
        if(StringEquals("newmtl", Data))
        {
            MaterialIndex += 1;
        }
        
        if(StringEquals("Kd", Data))
        {
            Data = Next;
            Next = ReadV3(Data,&Mat->DiffuseColor);
        }
        Data = Next;
    }
    return Materials;
}

model* LoadModel(memory_arena* Arena, char* FileName)
{
    model* Model = (model*) PushStruct(Arena,model);
    read_file_result File = Api.ReadFile(FileName);
    
    Assert(File.Data);
    
    char* Data = (char*)File.Data;
    char* Next = Data;
    char* UntouchedData = (char*) Api.AllocMem(File.Size);
    
    Api.CopyMem(UntouchedData, Data, File.Size);
    
    model_info ModelInfo = GetInfo(Data);
    Model->Info = ModelInfo; 
    
    v3* Vertices = (v3*) PushSize_(Arena, sizeof(v3) * ModelInfo.NumVertices);
    u32 VertexIndex = 0;
    
    v3* Normals = (v3*) PushSize_(Arena, sizeof(v3) * ModelInfo.NumNormals);
    u32 NormalIndex = 0;
    
    v2* UVs = (v2*) PushSize_(Arena, sizeof(v2) * ModelInfo.NumUV);
    u32 UVIndex = 0;
    
    
    u32 MeshIndex = 0;
    Model->Meshes = (mesh*) PushSize_(Arena, sizeof(mesh) * ModelInfo.NumMeshes);
    mesh* Mesh = Model->Meshes + MeshIndex;
    
    // Allocate meshes buffers;
    for(u32 Index = 0; Index < ModelInfo.NumMeshes; ++Index)
    {
        u32 NumElems = ModelInfo.MeshInfo[Index].NumFaces * 3;
        
        Model->Meshes[Index].Vertices = (v3*) PushSize_(Arena, sizeof(v3) * NumElems);
        Model->Meshes[Index].Normals = (v3*) PushSize_(Arena, sizeof(v3) * NumElems);
        Model->Meshes[Index].Textures = (v2*) PushSize_(Arena, sizeof(v2) * NumElems);
        
        Model->Meshes[Index].NumVert = NumElems;
    }
    
    u32 VertexCount = 0;
    u32 TextureCount = 0;
    u32 NormalCount = 0;
    Data = UntouchedData;
    
    material* Materials;
    
    
    while(*Data)
    {
        v3* Vertex = Vertices + VertexIndex;
        v3* Normal = Normals + NormalIndex;
        v2* UV = UVs + UVIndex;
        
        Next = Consume(Data);
        
        if(StringEquals("mtllib", Data))
        {
            Data = Next;
            Next = Consume(Data);
            char Path[128] = "";
            strcat(Path, "../data/obj/");
            strcat(Path, Data);
            Materials = ParseMTL(Arena, Path, ModelInfo.NumMeshes);
            for(u32 Index=0;Index<ModelInfo.NumMeshes;++Index)
            {
                Model->Meshes[Index].Material = Materials[Index];
            }
        }
        
        if(StringEquals("v", Data)) 
        {
            Next = ReadFloat(Next, &Vertex->x);
            Next = ReadFloat(Next, &Vertex->y);
            Next = ReadFloat(Next, &Vertex->z);
            ++VertexIndex;
        }
        
        if(StringEquals("vn", Data)) 
        {
            Next = ReadFloat(Next, &Normal->x);
            Next = ReadFloat(Next, &Normal->y);
            Next = ReadFloat(Next, &Normal->z);
            ++NormalIndex;
        }
        
        if(StringEquals("vt", Data)) 
        {
            Next = ReadFloat(Next, &UV->x);
            Next = ReadFloat(Next, &UV->y);
            ++UVIndex;
        }
        
        if(StringEquals("usemtl", Data))
        {
            VertexCount = 0;
            TextureCount = 0;
            NormalCount = 0;
            
            Mesh = Model->Meshes + MeshIndex;
            MeshIndex ++;
        }
        
        if(StringEquals("f", Data))
        {
            Next = GetLine(Data);
            ++Data;
            ++Data;
            char* Face = Data;
            
            u32 VerticesPerFace = GetNumVerticesPerFace(Face);
            
            i32 I = Find(Data, "//");
            if(I == -1) Mesh->UseTexture = true;
            else Mesh->UseTexture = false;
            
            // Maybe Index - 1
            i32 Index = 0;
            ReplaceSlashByNull(Data);
            Data= ReadInt(Data, &Index);
            Mesh->Vertices[VertexCount++] = Vertices[Index-1];
            //i32 VertexIndex_ = Index;
            
            ReplaceSlashByNull(Data);
            Data = ReadInt(Data, &Index);
            if(Mesh->UseTexture)
            {
                Mesh->Textures[TextureCount++] = UVs[Index-1];
            }
            
            Data = ReadInt(Data, &Index);
            Mesh->Normals[NormalCount++] = Normals[Index-1];
            
            
            ReplaceSlashByNull(Data);
            Data= ReadInt(Data, &Index);
            Mesh->Vertices[VertexCount++] = Vertices[Index-1];
            //i32 VertexIndex_ = Index;
            
            ReplaceSlashByNull(Data);
            Data = ReadInt(Data, &Index);
            if(Mesh->UseTexture)
            {
                Mesh->Textures[TextureCount++] = UVs[Index-1];
            }
            
            Data= ReadInt(Data, &Index);
            Mesh->Normals[NormalCount++] = Normals[Index-1];
            
            ReplaceSlashByNull(Data);
            Data= ReadInt(Data, &Index);
            Mesh->Vertices[VertexCount++] = Vertices[Index-1];
            //i32 VertexIndex_ = Index;
            
            ReplaceSlashByNull(Data);
            Data= ReadInt(Data, &Index);
            if(Mesh->UseTexture)
            {
                Mesh->Textures[TextureCount++] = UVs[Index-1];
            }
            
            Data= ReadInt(Data, &Index);
            Mesh->Normals[NormalCount++] = Normals[Index-1];
            
        }
        Data = Next;
    }
    
    CalcIndices();
    
    Api.FreeMem(UntouchedData, File.Size);
    Api.FreeFile(File);
    return Model;
}

