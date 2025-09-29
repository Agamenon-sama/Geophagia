#include "Model.h"

#include <assimp/material.h>
#include <assimp/types.h>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <slog/slog.h>

namespace Necrosis {

Model::Model(const std::filesystem::path &path) {
    _directory = path.parent_path().string() + "/";
    
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path.string(), 
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::MODEL::Failed to load model at " << path << "\n";
        std::cerr << importer.GetErrorString() << "\n";
        return;
    }

    _processNode(scene->mRootNode, scene);

}

// void Model::setShader(std::shared_ptr<Shader> s) {
//     for (auto &m : _meshes) {
//         m.setShader(shader);
//     }
// }

void Model::render() const {
    for (auto &m : _meshes) {
        for (size_t i = 0; i < m.textures.size(); i++) {
            TextureManager::bind(m.textures[i], i);
        }
        m.render();
    }
}

void Model::_processNode(aiNode *node, const aiScene *scene) {
    // add all meshes in the node
    for (size_t i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        auto m = _processMesh(mesh, scene);
        _meshes.push_back(m);
        // _meshes.push_back(_processMesh(mesh, scene));
    }

    // pass to all child nodes of this node
    for (size_t i = 0; i < node->mNumChildren; i++) {
        _processNode(node->mChildren[i], scene);
    }
}

Mesh Model::_processMesh(aiMesh *mesh, const aiScene *scene) {
    // Mesh m;
    std::vector<Vertex> verts;
    std::vector<uint32_t> inds;
    std::vector<TextureID> texs;

    for (size_t i = 0; i < mesh->mNumVertices; i++) {
        Vertex vert;

        if (mesh->HasPositions()) {
            vert.position = glm::vec3(
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z
            );
        }
        else {
            vert.position = glm::vec3(0.f);
        }

        if (mesh->HasNormals()) {
            vert.normal = glm::vec3(
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            );
        }
        else {
            vert.normal = glm::vec3(0.f);
        }

        if (mesh->HasTextureCoords(0)) {
            vert.texCoord = glm::vec2(
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            );
        }
        else {
            vert.texCoord = glm::vec2(0.f);
        }

        verts.push_back(vert);
    }

    for (size_t i = 0; i < mesh->mNumFaces; i++) {
        aiFace index = mesh->mFaces[i];
        for (size_t j = 0; j < index.mNumIndices; j++) {
            inds.push_back(index.mIndices[j]);
        }
    }

    if (scene->HasMaterials()) {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<TextureID> diffuseMaps  = _loadMaterialTextures(material, aiTextureType_DIFFUSE);
        std::vector<TextureID> specularMaps = _loadMaterialTextures(material, aiTextureType_SPECULAR);

        texs.insert(texs.end(), diffuseMaps.begin(), diffuseMaps.end());
        texs.insert(texs.end(), specularMaps.begin(), specularMaps.end());

    }
    // slog::message("number of textures " + std::to_string(texs.size()));
    // for (const auto &t : texs) {
    //     slog::message(t.getFileName());
    // }

    return Mesh(verts, inds, texs);
}

std::vector<TextureID> Model::_loadMaterialTextures(aiMaterial *mat, aiTextureType type) {
    std::vector<TextureID> textures;

    for (size_t i = 0; i < mat->GetTextureCount(type); i++) {
        aiString path;
        mat->GetTexture(type, i, &path);
        // slog::debug("loading texture: {}", path.data);
        TextureID tex = TextureManager::makeTextureFromFile(_directory + std::string(path.data));
        textures.push_back(tex);

        int diffuse;        
        if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse) == AI_SUCCESS) {
            // slog::debug("diffuse value is {}", diffuse);
        }
    }

    return textures;
}

}
