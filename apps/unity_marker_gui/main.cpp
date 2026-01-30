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

    LogBuffer log_rx;   // received from Unity
    LogBuffer log_tx;   // sent from this GUI
    LSLReader reader("UnityMarkers");

    bool auto_scroll = true;

    // LSL outlet (marker sender)
    lsl::stream_info info(
        "CppToUnityMarkers",   // ✅ TX stream name (different!)
        "Markers",
        1,
        0,
        lsl::cf_string,
        "cpp_gui_sender"
    );


    lsl::stream_outlet outlet(info);
    log_tx.push("[TX] Outlet advertised: name=CppToUnityMarkers type=Markers source_id=cpp_gui_sender");



    // ---------------- main loop ----------------
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Unity Marker Logger");

        ImGui::Text("RX: UnityMarkers   |   TX: CppToUnityMarkers");

        ImGui::SameLine();
        ImGui::Text("| Status: %s", reader.running() ? "RUNNING" : "STOPPED");

        if (!reader.running()) {
            if (ImGui::Button("Connect")) {
                reader.start(log_rx);
            }
        }
        else {
            if (ImGui::Button("Stop")) {
                reader.stop();
            }
        }

        if (ImGui::Button("Clear All")) {
            log_rx.clear();
            log_tx.clear();
        }


        ImGui::SameLine();
        ImGui::Checkbox("Auto-scroll", &auto_scroll);

        ImGui::SeparatorText("Send Marker");

        if (ImGui::Button("SPACE_DOWN (Send)", ImVec2(220, 40))) {
            std::string sample[1];
            sample[0] = "SPACE_DOWN";
            outlet.push_sample(sample);

            log_tx.push("[TX][GUI] Sent SPACE_DOWN");
        }

        ImGui::SameLine();

        if (ImGui::Button("SPACE_UP (Send)", ImVec2(160, 40))) {
            std::string sample[1];
            sample[0] = "SPACE_UP";
            outlet.push_sample(sample);

            log_tx.push("[TX][GUI] Sent SPACE_UP");
        }

        ImGui::SeparatorText("Logs");

        ImGui::Columns(2, "logs_cols", true);

        ImGui::SameLine();
        if (ImGui::Button("PING", ImVec2(80, 40))) {
            std::string marker = "PING_FROM_CPP";
            outlet.push_sample(&marker);
            log_tx.push("[TX][GUI] Sent PING_FROM_CPP");
        }


        // ---------- RX LOG (Unity -> C++) ----------
        ImGui::TextUnformatted("RX (from Unity)");
        ImGui::SameLine();
        if (ImGui::SmallButton("Clear##rx")) log_rx.clear();

        ImGui::BeginChild("rx_log", ImVec2(0, 0), true);
        {
            auto lines = log_rx.snapshot();
            for (const auto& s : lines) ImGui::TextUnformatted(s.c_str());
        
            if (auto_scroll &&
                ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 20.0f) {
                ImGui::SetScrollHereY(1.0f);
            }
        }
        ImGui::EndChild();

        ImGui::NextColumn();

        // ---------- TX LOG (C++ -> Unity) ----------
        ImGui::TextUnformatted("TX (to Unity)");
        ImGui::SameLine();
        if (ImGui::SmallButton("Clear##tx")) log_tx.clear();

        ImGui::BeginChild("tx_log", ImVec2(0, 0), true);
        {
            auto lines = log_tx.snapshot();
            for (const auto& s : lines) ImGui::TextUnformatted(s.c_str());
        
            if (auto_scroll &&
                ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 20.0f) {
                ImGui::SetScrollHereY(1.0f);
            }
        }
        ImGui::EndChild();

        ImGui::Columns(1);


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
