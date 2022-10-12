#include <ObjLoader.h>
#include <fstream>
#include <sstream>
#include <ResourceManager.hh>
#include <algorithm>


#define USE_INDEXING 1
#define and &&
#define or ||
//#define DEBUG_OBJ_LOADER

bool ObjLoader::load(const char *path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cout << "cant open file " << path << std::endl;
        return false;
    } else {
        std::cout << "file opened " << path << std::endl;
    }

    bool inface = false;
    std::string line;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texture_coords;
    std::vector<glm::vec3> normals;
    bool first = true;
    while (std::getline(file, line))
    {
        std::stringstream ss;
        ss << line;
        std::string type;

        ss >> type;
        if (inface and type != "f")
        {
            inface = false;
            parseObject(vertices, texture_coords, normals);
        }
        if (type == "v")
        {
            glm::vec3 vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            if (first)
            {
                minPos = vertex;
                maxPos = vertex;
            }
            else
            {
                // max vertex
                if (vertex.x < minPos.x)
                    minPos.x = vertex.x;
                if (vertex.y < minPos.y)
                    minPos.y = vertex.y;
                if (vertex.z < minPos.z)
                    minPos.z = vertex.z;
                // min vertex

                if (vertex.x > maxPos.x)
                    maxPos.x = vertex.x;
                if (vertex.y > maxPos.y)
                    maxPos.y = vertex.y;
                if (vertex.z > maxPos.z)
                    maxPos.z = vertex.z;
            }

            first = false;

            vertices.push_back(vertex);
        }
        else if (type == "vt")
        {
            glm::vec2 texture_coord;
            ss >> texture_coord.x >> texture_coord.y;
            texture_coords.push_back(texture_coord);
        }
        else if (type == "vn")
        {
            glm::vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        }
        else if (type == "usemtl")
        {
            std::string mat_name;
            ss >> mat_name;
            current_mat = mat_name;
        }
        else if (type == "g" /* || type == "o"*/)
        {
            std::string mesh_name;
            ss >> mesh_name;

            current_group = mesh_name;
        }
        else if (type == "f")
        {
            inface = true;
            parseFace(ss);
        }
        else if (type == "mtllib")
        {
            std::string file_mtl_name;
            ss >> file_mtl_name;
            parseMTL(file_mtl_name);
        }
    }

    centerPosition = glm::vec3((minPos.x + maxPos.x) / 2.0f, (minPos.y + maxPos.y) / 2.0f, (minPos.z + maxPos.z) / 2.0f);

    // Parse the last object remaining
    parseObject(vertices, texture_coords, normals);

    return true;
}

void ObjLoader::doIndexed(std::vector<glm::vec3> &in_vertices,
                          std::vector<glm::vec2> &in_uv,
                          std::vector<glm::vec3> &in_normals,
                          std::vector<uint16_t> &indices,
                          std::vector<glm::vec3> &out_vertices,
                          std::vector<glm::vec2> &out_uvs,
                          std::vector<glm::vec3> &out_normals)
{

    std::map<ModelVertex, uint16_t> vertices;

    for (uint32_t i = 0; i < in_vertices.size(); ++i)
    {
        ModelVertex a;
        memset(&a,0,sizeof(struct ModelVertex));
        a.position = in_vertices[i];
        a.uv = in_uv[i];
        auto it = vertices.find(a);
        if (it == vertices.end())
        {
            out_vertices.push_back(in_vertices[i]);
            out_uvs.push_back(in_uv[i]);
            out_normals.push_back(in_normals[i]);
            uint16_t index = out_vertices.size() - 1;
            indices.push_back(index);
            vertices.emplace(std::make_pair(a, index));
        }
        else
        {
            indices.push_back(it->second);
        }
    }
}

void ObjLoader::parseFace(std::stringstream &ss)
{
    std::string f;
    int c = 0;
    Face face;
    uint32_t ids[4];
    uint32_t tids[4];
    uint32_t nids[4];
    while (ss >> f)
    {
        std::size_t index = f.find("//");
        if (index != std::string::npos)
        { // found face with no texture
            sscanf(f.c_str(), "%u//%u", &ids[c], &nids[c]);
            face.hasTexture = false;
        }
        else
        {
            size_t n = std::count(f.begin(), f.end(), '/');
            if (n == 2)
            {
                sscanf(f.c_str(), "%u/%u/%u", &ids[c], &tids[c], &nids[c]);
                face.hasTexture = true;
            }
            else if (n == 1)
            {
                sscanf(f.c_str(), "%u/%u", &ids[c], &tids[c]);
                face.hasTexture = true;
            }
        }

        ++c;
    }
    if (c == 4)
    {
        /**  * 0 1 2 3 => a b c d
             * 0 1 2   => a b c
             * 0 2 3   => a c d
             * **/
        for (int i = 0; i < 3; ++i)
        {
            face.vertices.push_back(ids[i] - 1);
            face.normals.push_back(nids[i] - 1);
            if (face.hasTexture)
                face.textures.push_back(tids[i] - 1);
            else
                face.textures.push_back(-1);
        }

        face.vertices.push_back(ids[0] - 1);
        face.vertices.push_back(ids[2] - 1);
        face.vertices.push_back(ids[3] - 1);
        face.normals.push_back(nids[0] - 1);
        face.normals.push_back(nids[2] - 1);
        face.normals.push_back(nids[3] - 1);

        if (face.hasTexture)
        {
            face.textures.push_back(tids[0] - 1);
            face.textures.push_back(tids[2] - 1);
            face.textures.push_back(tids[3] - 1);
        }
        else
        {
            face.textures.push_back(-1);
            face.textures.push_back(-1);
            face.textures.push_back(-1);
        }
    }
    else
    {
        for (int i = 0; i < 3; ++i)
        {
            face.vertices.push_back(ids[i] - 1);
            face.textures.push_back(tids[i] - 1);
            face.normals.push_back(nids[i] - 1);
        }
    }

    faces.push_back(face);
}

void ObjLoader::parseMTL(std::string &file_path)
{

    std::ifstream file("assets/" + file_path);
    if (!file.is_open())
    {
        std::cout << "cant open MTL file " << file_path << std::endl;
        return;
    }
    std::string line;
    std::string mat_name;
    while (std::getline(file, line))
    {
        std::stringstream ss;
        ss << line;
        std::string type;
        ss >> type;
        if (type == "#")
            continue;
        else if (type == "newmtl")
        {
            ss >> mat_name;

            std::unique_ptr<Material> m = std::make_unique<Material>();
            m->mat_name = mat_name;

            auto it = materials.find(mat_name);
            if (it == materials.end())
            {
                // add new material
                materials.emplace(std::make_pair(mat_name, std::move(m)));
            }
        }
        else if (type == "Kd")
        {
            auto it = materials.find(mat_name);
            if (it != materials.end())
            {
                ss >> it->second->diffuse_color.x >> it->second->diffuse_color.y >> it->second->diffuse_color.z;
            }
        }
        else if (type == "map_Kd")
        {
            //std::cout << "CURR ----------" << mat_name << std::endl;
            auto it = materials.find(mat_name);
            if (it != materials.end())
            {
                ss >> it->second->map_kd;
                if (it->second->textures_id.size() == 0)
                {
                    uint32_t id = ResourceManager::loadTexture(it->second->map_kd.c_str())->id;
                    if (id > 0)
                    {
                        it->second->textures_id.emplace_back(id);
                        it->second->hasTexture = true;
                    }
                    else
                    {
                        it->second->hasTexture = false;
                    }
#ifdef DEBUG_OBJ_LOADER
                    std::cout << "ASSIGNED TEXTURE " << id << " to " << it->second->mat_name << " " << it->second->map_kd << std::endl;
#endif
                }
            }
            else
            {
                std::cout << "MAT NAME NOT FOUND " << mat_name << std::endl;
            }
        }
    }
}

void ObjLoader::parseObject(std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &texture_coords,
std::vector<glm::vec3> &normals)
{

    std::vector<glm::vec3> vbo_vertices;
    std::vector<glm::vec2> vbo_texture_coords;
    std::vector<glm::vec3> vbo_normals;

    std::vector<glm::vec3> out_vertices;
    std::vector<glm::vec2> out_textureuv;
    std::vector<glm::vec3> out_normals;
    std::vector<uint16_t> indices;

    for (Face &f : faces)
    {
        for (uint32_t i = 0; i < f.vertices.size(); ++i)
        {
            vbo_vertices.push_back(vertices[f.vertices[i]]);
            if (f.textures[i] == -1 || texture_coords.size() == 0)
                vbo_texture_coords.push_back(glm::vec2(0.0f, 0.0f));
            else
                vbo_texture_coords.push_back(texture_coords[f.textures[i]]);
            
            vbo_normals.push_back(normals[f.normals[i]]);
        }
    }
#ifdef DEBUG_OBJ_LOADER

    //std::cout << "before vertices size = " << vbo_vertices.size() << std::endl;
    //std::cout << "before textures size = " << vbo_texture_coords.size() << std::endl;
    std::cout << "before normals size = " << vbo_normals.size() << std::endl;
    std::cout << "before indices size = " << indices.size() << std::endl;
#endif

#if USE_INDEXING

    doIndexed(vbo_vertices, vbo_texture_coords,vbo_normals, indices, out_vertices, out_textureuv, out_normals);
#ifdef DEBUG_OBJ_LOADER
    //std::cout << "after vertices size = " << out_vertices.size() << std::endl;
    //std::cout << "after textures size = " << out_textureuv.size() << std::endl;
    std::cout << "after noramls size = " << out_normals.size() << std::endl;
    std::cout << "after indices size = " << indices.size() << std::endl;
#endif
    auto it = materials.find(current_mat);
    if (it == materials.end())
    {
        std::cout << " NOT FOUND " << current_mat << std::endl;
        mesh.emplace_back(out_vertices, out_textureuv,out_normals, indices, "nomat");
    }
    else
    {
        mesh.emplace_back(out_vertices, out_textureuv, out_normals,indices, it->second->mat_name);
    }
    faces.clear();

#else
    out_vertices = vbo_vertices;
    out_textureuv = vbo_texture_coords;
#endif
}

glm::vec3 ObjLoader::getCenter() {
    return centerPosition;
}