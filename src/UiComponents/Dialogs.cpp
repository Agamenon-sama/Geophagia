#include "Dialogs.h"

#include <imgui/imgui.h>

void showSaveDialog(const char title[], const std::function<void(const char[])> &callback) {
    static char filename[64] = {};

    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal(title, nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Set the name of file");
        ImGui::InputText("file name", filename, 64);
        if (ImGui::Button("Save", ImVec2(120, 0))) {
            callback(filename);
            ImGui::CloseCurrentPopup();
        }; ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}
