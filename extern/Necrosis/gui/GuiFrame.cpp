#include "GuiFrame.h"

namespace Necrosis {

GuiFrame::GuiFrame() {}

void GuiFrame::handleEvents(SDL_Event &event) {
    ImGui_ImplSDL3_ProcessEvent(&event);
}

void GuiFrame::beginRender() {
    ImGui_ImplOpenGL3_NewFrame();
    // ImGui_ImplSDL2_NewFrame(_parentWindow->getSDLWindow());
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void GuiFrame::endRender() {
    ImGui::Render();
    // ImGuiIO &io = ImGui::GetIO();
    // glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    // glClearColor(0.11, 0.11, 0.11, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

}
