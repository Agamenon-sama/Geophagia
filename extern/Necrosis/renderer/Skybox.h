#pragma once

#include "Buffer.h"
#include "Renderer.h"
#include "Shader.h"
#include "../scene/Camera.h"

namespace Necrosis {

class Skybox : public Renderable {
public:
    Skybox(const Camera &camera);
    ~Skybox();

    void render() const override;
private:
    uint32_t _texture;
    
    VertexArray *_vao;
    VertexBuffer *_vbo;
    std::shared_ptr<Shader> _shader;
    const Camera &_camera;
};

}
