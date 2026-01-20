#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

struct vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 texCoord;
};

namespace MODEL_LOADING
{
    inline void ProcessMesh(aiMesh* mesh, std::vector<vertex>& vertices, std::vector<unsigned int>& indices)
    {
       /* std::cout << "Mesh name: " << mesh->mName.C_Str()
            << "verts: " << mesh->mNumVertices
            << "Faces: " << mesh->mNumFaces << std::endl;*/
        vertices.reserve(mesh->mNumVertices);

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            aiVector3D pos = mesh->mVertices[i];
            vertex vert;
            vert.position.x = pos.x;
            vert.position.y = pos.y;
            vert.position.z = pos.z;

            vert.color = glm::vec3(1.2, 1.1, 0.1);

            aiVector3D normal = mesh->mNormals[i];
            vert.normal.x = normal.x;
            vert.normal.y = normal.y;
            vert.normal.z = normal.z;

            vert.texCoord.x = mesh->mTextureCoords[0][i].x;
            vert.texCoord.y = mesh->mTextureCoords[0][i].y;
            vertices.push_back(vert);
        // Use pos.x, pos.y, pos.z
        }
        for (unsigned int j = 0; j < mesh->mNumFaces; j++)
        {
            aiFace face = mesh->mFaces[j];
            for (unsigned int k = 0; k < face.mNumIndices; k++)
            {
                indices.push_back(face.mIndices[k]);
            }
        }
    }

    inline void ProcessNode(aiNode* node, const aiScene* scene, std::vector<vertex>& vertices, std::vector<unsigned int>& indices) {
      /*  std::cout << "Node: " << node->mName.C_Str()
            << " | Meshes: " << node->mNumMeshes*/
          /*  << std::endl;*/

        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            ProcessMesh(mesh, vertices, indices);
        }

        // Then recursively process children
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            ProcessNode(node->mChildren[i], scene, vertices, indices);
        }
    }

    inline bool loadModel(const std::string path, std::vector<vertex>& vertices, std::vector<unsigned int>& indices)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, 
            aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_GenSmoothNormals |
            aiProcess_CalcTangentSpace |
            aiProcess_FlipUVs |
            aiProcess_ImproveCacheLocality |
            aiProcess_SortByPType);

        ProcessNode(scene->mRootNode, scene, vertices, indices);
        return true;
    }
}

namespace CUBE
{
    //inline std::vector<vertex> cubeVerts = {

    //    // position, color, texcoords
    //    // front
    //    {{-0.5f,-0.5f, 0.5f},{1,1,1},{0,0}},
    //    {{ 0.5f,-0.5f, 0.5f},{1,1,1},{1,0}},
    //    {{ 0.5f, 0.5f, 0.5f},{1,1,1},{1,1}},
    //    {{ 0.5f, 0.5f, 0.5f},{1,1,1},{1,1}},
    //    {{-0.5f, 0.5f, 0.5f},{1,1,1},{0,1}},
    //    {{-0.5f,-0.5f, 0.5f},{1,1,1},{0,0}},

    //    // back
    //    {{ 0.5f,-0.5f,-0.5f},{1,1,1},{0,0}},
    //    {{-0.5f,-0.5f,-0.5f},{1,1,1},{1,0}},
    //    {{-0.5f, 0.5f,-0.5f},{1,1,1},{1,1}},
    //    {{-0.5f, 0.5f,-0.5f},{1,1,1},{1,1}},
    //    {{ 0.5f, 0.5f,-0.5f},{1,1,1},{0,1}},
    //    {{ 0.5f,-0.5f,-0.5f},{1,1,1},{0,0}},

    //    // left
    //    {{-0.5f,-0.5f,-0.5f},{1,1,1},{0,0}},
    //    {{-0.5f,-0.5f, 0.5f},{1,1,1},{1,0}},
    //    {{-0.5f, 0.5f, 0.5f},{1,1,1},{1,1}},
    //    {{-0.5f, 0.5f, 0.5f},{1,1,1},{1,1}},
    //    {{-0.5f, 0.5f,-0.5f},{1,1,1},{0,1}},
    //    {{-0.5f,-0.5f,-0.5f},{1,1,1},{0,0}},

    //    // right
    //    {{ 0.5f,-0.5f, 0.5f},{1,1,1},{0,0}},
    //    {{ 0.5f,-0.5f,-0.5f},{1,1,1},{1,0}},
    //    {{ 0.5f, 0.5f,-0.5f},{1,1,1},{1,1}},
    //    {{ 0.5f, 0.5f,-0.5f},{1,1,1},{1,1}},
    //    {{ 0.5f, 0.5f, 0.5f},{1,1,1},{0,1}},
    //    {{ 0.5f,-0.5f, 0.5f},{1,1,1},{0,0}},

    //    // top
    //    {{-0.5f, 0.5f, 0.5f},{1,1,1},{0,0}},
    //    {{ 0.5f, 0.5f, 0.5f},{1,1,1},{1,0}},
    //    {{ 0.5f, 0.5f,-0.5f},{1,1,1},{1,1}},
    //    {{ 0.5f, 0.5f,-0.5f},{1,1,1},{1,1}},
    //    {{-0.5f, 0.5f,-0.5f},{1,1,1},{0,1}},
    //    {{-0.5f, 0.5f, 0.5f},{1,1,1},{0,0}},

    //    // bottom
    //    {{-0.5f,-0.5f,-0.5f},{1,1,1},{0,0}},
    //    {{ 0.5f,-0.5f,-0.5f},{1,1,1},{1,0}},
    //    {{ 0.5f,-0.5f, 0.5f},{1,1,1},{1,1}},
    //    {{ 0.5f,-0.5f, 0.5f},{1,1,1},{1,1}},
    //    {{-0.5f,-0.5f, 0.5f},{1,1,1},{0,1}},
    //    {{-0.5f,-0.5f,-0.5f},{1,1,1},{0,0}}
    //};

}