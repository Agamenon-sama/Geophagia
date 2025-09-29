#include "DevFrame.h"

#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

namespace Necrosis {

DevFrame::DevFrame(){}

void DevFrame::uiRender() {
    ImGui::Begin("Developer window");

        ImGui::Text("last frame: %.3f ms, fps: %.3f", 1000.f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Separator();

        ImGui::Text("OpenGL info:");
        ImGui::BulletText("vendor: %s", (const char*)glGetString(GL_VENDOR));
        ImGui::BulletText("version: %s", (const char*)glGetString(GL_VERSION));
        ImGui::BulletText("renderer: %s", (const char*)glGetString(GL_RENDERER));
        ImGui::BulletText("extensions: %s", (const char*)glGetString(GL_EXTENSIONS));
        ImGui::Separator();

        ImGui::Text("Camera position:");
        ImGui::BulletText("x: %f", camera.position.x);
        ImGui::BulletText("y: %f", camera.position.y);
        ImGui::BulletText("z: %f", camera.position.z);
        ImGui::BulletText("fov: %f", camera.fov);
        ImGui::Separator();

        if (ImGui::SliderFloat("mouse sensitivity", &(camera.sensitivity), 0.1f, 0.5f, "%.2f")) {
            dispatcher.dispatch({
                .type = DevFrameEventType::Sensitivity,
                .fval = camera.sensitivity
            });
        }
        if (ImGui::SliderFloat("movement speed", &(camera.movementSpeed), 0.5f, 15.f, "%.2f")) {
            dispatcher.dispatch({
                .type = DevFrameEventType::Speed,
                .fval = camera.movementSpeed
            });
        }

        if (ImGui::Button("Toggle wireframe")) {
            _wireframeOn = !_wireframeOn;
            dispatcher.dispatch({
                .type = DevFrameEventType::WireframeMode,
                .wireframe = _wireframeOn
            });
        } ImGui::SameLine();

    ImGui::End();
}

void DevFrame::render() {
    beginRender();
    uiRender();
    endRender();
}

}
