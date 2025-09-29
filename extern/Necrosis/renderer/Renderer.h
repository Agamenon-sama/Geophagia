#pragma once

#include <vector>
#include <memory>

#include "Buffer.h"
#include "Shader.h"
#include "../scene/Camera.h"

namespace Necrosis {

/**
 * @brief Interface to all renderable objects by the renderer
*/
class Renderable {
public:
    /**
     * @brief Renders the renderable object to the screen
    */
    virtual void render() const = 0;
};

/**
 * @brief A struct with all the elements necessary to render the object
*/
struct RenderCommand {
    std::shared_ptr<Renderable> renderable;
    std::shared_ptr<Shader> shader;
};

class Renderer {
public:
    Renderer();
    ~Renderer();

    /**
     * @brief Turn on or off OpenGL's native wireframe view.
     * 
     * @param on Draw in wireframe mode if true. Draw in fill mode if false.
    */
    void setWireframeMode(bool on) const;

    [[deprecated("Use the renderAll api")]]
    void render(const VertexArray& va, const IndexBuffer &ib, const Shader &shader) const noexcept;
    [[deprecated("Use the renderAll api")]]
    void render(const VertexArray& va, const VertexBuffer &vb, const Shader &shader) const noexcept;

    /**
     * @brief Iterate through all the registered render commands and other
     * renderables and draws them to the screen
    */
    void renderAll() const noexcept;
    /**
     * @brief Fill screen with a default color of #1c1c1c as well as clear other buffers
     *
     * Should be called at the beginning of every frame before rendering
    */
    void clear() const noexcept;

    /**
     * @brief Add the renderable object and its data to the list of objects to render
    */
    void addRenderCommand(RenderCommand command);
    /**
     * @brief Set the skybox to be rendered, if any, in the rendered frame
    */
    void setSkybox(std::shared_ptr<Renderable> skybox) { _skybox = skybox; }
    /**
     * @brief Set the camera currently used in the rendered frame
     *
     * A camera should always be set to let the renderer automatically update
     * the view and projection matrices of the renderables
    */
    void setCamera(Camera *camera) { _camera = camera; }

private:
    // could change the data type for the purpose of ordering
    std::vector<RenderCommand> _renderCommands;
    std::shared_ptr<Renderable> _skybox;
    Camera *_camera;
};

}
