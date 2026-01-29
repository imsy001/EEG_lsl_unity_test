cmake_minimum_required(VERSION 3.16)

add_executable(unity_marker_gui
    main.cpp
    lsl_reader.cpp
)

target_compile_definitions(unity_marker_gui PRIVATE LSL_NO_CPP=0)
target_compile_definitions(unity_marker_gui PRIVATE __cplusplus=201703L)



# Always refer to the top-level folder
set(ROOT_DIR  "${CMAKE_SOURCE_DIR}")
set(EXT_DIR   "${ROOT_DIR}/external")

set(IMGUI_DIR "${EXT_DIR}/imgui")
set(GLFW_DIR  "${EXT_DIR}/glfw")
set(LSL_DIR   "${EXT_DIR}/liblsl")

# ---- includes (ONE block only) ----
target_include_directories(unity_marker_gui PRIVATE
    "${LSL_DIR}/include"          # <- contains lsl_cpp.h
    "${IMGUI_DIR}"
    "${IMGUI_DIR}/backends"
    "${GLFW_DIR}/include"
)

# ---- ImGui sources ----
target_sources(unity_marker_gui PRIVATE
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
)

# ---- link (Windows) ----
if (WIN32)
    target_link_libraries(unity_marker_gui PRIVATE
        "${LSL_DIR}/lib/lsl.lib"
        "${GLFW_DIR}/lib/glfw3.lib"
        opengl32
    )

    # Copy lsl.dll next to the exe after build
    add_custom_command(TARGET unity_marker_gui POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${LSL_DIR}/bin/lsl.dll"
            "$<TARGET_FILE_DIR:unity_marker_gui>"
    )

    target_compile_definitions(unity_marker_gui PRIVATE LSL_NO_CPP=0)

endif()
