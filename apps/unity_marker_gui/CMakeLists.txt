cmake_minimum_required(VERSION 3.16)
project(unity_marker_gui LANGUAGES CXX)

add_executable(unity_marker_gui
    main.cpp
    lsl_reader.cpp
)

# -----------------------
# Paths
# -----------------------
set(ROOT_DIR  "${CMAKE_SOURCE_DIR}")
set(EXT_DIR   "${ROOT_DIR}/external")

set(IMGUI_DIR "${EXT_DIR}/imgui")
set(GLFW_DIR  "${EXT_DIR}/glfw")
set(LSL_DIR   "${EXT_DIR}/liblsl")

# -----------------------
# Compiler options
# -----------------------
target_compile_features(unity_marker_gui PRIVATE cxx_std_17)

if (MSVC)
    # correct __cplusplus value in MSVC (DON'T #define __cplusplus)
    target_compile_options(unity_marker_gui PRIVATE /Zc:__cplusplus /utf-8)
endif()

# If you use lsl_cpp.h (C++ wrapper), keep this 0. (You already do.)
target_compile_definitions(unity_marker_gui PRIVATE LSL_NO_CPP=0)

# -----------------------
# Includes
# -----------------------
target_include_directories(unity_marker_gui PRIVATE
    "${LSL_DIR}/include"
    "${IMGUI_DIR}"
    "${IMGUI_DIR}/backends"
    "${GLFW_DIR}/include"
)

# -----------------------
# ImGui sources
# -----------------------
target_sources(unity_marker_gui PRIVATE
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
)

# -----------------------
# GLFW: build from source (NO prebuilt glfw3.lib needed)
# -----------------------
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)

add_subdirectory("${GLFW_DIR}" "${CMAKE_BINARY_DIR}/_deps/glfw")

# -----------------------
# Link
# -----------------------
if (WIN32)
    target_link_libraries(unity_marker_gui PRIVATE
        "${LSL_DIR}/lib/lsl.lib"
        glfw
        opengl32
    )

    add_custom_command(TARGET unity_marker_gui POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${LSL_DIR}/bin/lsl.dll"
            "$<TARGET_FILE_DIR:unity_marker_gui>"
    )
endif()
