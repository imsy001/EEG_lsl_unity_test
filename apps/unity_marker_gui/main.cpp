#include "log_buffer.hpp"
#include "lsl_reader.hpp"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

int main() {
    // GLFW
    if (!glfwInit())
        return 1;

    const char* glsl_version = "#version 130";
    GLFWwindow* window = glfwCreateWindow(
        900, 600, "Unity Marker GUI (LSL)", nullptr, nullptr);
    if (!window) return 1;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    LogBuffer log;
    LSLReader reader("UnityMarkers");
    bool auto_scroll = true;

    // ---------------- main loop ----------------
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Unity Marker Logger");

        ImGui::Text("Stream: UnityMarkers");
        ImGui::SameLine();
        ImGui::Text("| Status: %s", reader.running() ? "RUNNING" : "STOPPED");

        if (!reader.running()) {
            if (ImGui::Button("Connect")) {
                reader.start(log);
            }
        }
        else {
            if (ImGui::Button("Stop")) {
                reader.stop();
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Clear"))
            log.clear();

        ImGui::SameLine();
        ImGui::Checkbox("Auto-scroll", &auto_scroll);

        ImGui::Separator();

        ImGui::BeginChild("log", ImVec2(0, 0), true);
        auto lines = log.snapshot();
        for (const auto& s : lines)
            ImGui::TextUnformatted(s.c_str());

        if (auto_scroll &&
            ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 20.0f) {
            ImGui::SetScrollHereY(1.0f);
        }
        ImGui::EndChild();

        ImGui::End();

        ImGui::Render();
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    reader.stop();

    // shutdown
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
