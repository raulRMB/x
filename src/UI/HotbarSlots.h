#include <imgui.h>
#include <iostream>

namespace HotbarSlots {

    static ImVec2 windowPosition;
    static bool isDragging = false;
    const int SLOTS = 6;
    int butnum[SLOTS];

    void Init() {
        for(int i = 0; i < SLOTS; ++i) {
            butnum[i] = i + 1;
        }
    }

    void Draw() {
        ImGuiWindowFlags window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoTitleBar;
        window_flags |= ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoResize;
        bool open = true;
        
        int windowX = 300;
        int windowY = 70;
        ImGui::SetNextWindowSize(ImVec2(400, 70), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x/2 - 200, ImGui::GetIO().DisplaySize.y - windowY), ImGuiCond_Always);
        ImGui::Begin("HotbarSlots", &open, window_flags);
       
        for (int i = 0; i < SLOTS; ++i) {
            ImGui::Button((std::to_string(butnum[i])).c_str(), ImVec2(50, 50));
            ImGuiDragDropFlags src_flags = 0;
            // src_flags |= ImGuiDragDropFlags_SourceNoDisableHover;
            // src_flags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers;
            //src_flags |= ImGuiDragDropFlags_SourceNoPreviewTooltip; // Hide the tooltip
            
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            if (ImGui::BeginDragDropSource(src_flags)) {
                ImGui::SetDragDropPayload("ITEMN", &i, sizeof(int), ImGuiCond_Once);
                ImGui::Button((std::to_string(butnum[i])).c_str(), ImVec2(50, 50));
                ImGui::EndDragDropSource();
            }
            ImGui::PopStyleVar();
            ImGui::PopStyleVar();

            if (ImGui::BeginDragDropTarget()) {
                ImGuiDragDropFlags target_flags = 0;
                const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ITEMN", target_flags);
                if (payload != nullptr) {
                    assert(payload->DataSize == sizeof(int));
                    int* numptr = static_cast<int*>(payload->Data);
                    std::swap(butnum[numptr[0]], butnum[i]);
                }
                ImGui::EndDragDropTarget();
            }

            ImGui::SameLine();
        }


        ImGui::End();
    }
}