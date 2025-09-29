#pragma once

#include <filesystem>
#include <vector>

#include <assimp/scene.h>

#include "Mesh.h"
#include "../renderer/Renderer.h"

namespace Necrosis {

class Model : public Renderable {
public:
    Model() = default;
    Model(const std::filesystem::path &path);

    void render() const override;

private:
    std::string _directory;
    std::vector<Mesh> _meshes;

    void _processNode(aiNode *node, const aiScene *scene);
    Mesh _processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<TextureID> _loadMaterialTextures(aiMaterial *mat, aiTextureType type);
};

}
