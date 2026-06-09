set(IMGUI_SOURCES
    imconfig.h
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_internal.h
    imgui_tables.cpp
    imgui_widgets.cpp
    imgui.cpp
    imgui.h
    imstb_rectpack.h
    imstb_textedit.h
    imstb_truetype.h
)

if (PX_INCLUDE_GLFW)
    list(APPEND IMGUI_SOURCES "backends/imgui_impl_glfw.cpp" "backends/imgui_impl_glfw.h")
endif()

if (PX_INCLUDE_OPENGL)
    list(APPEND IMGUI_SOURCES "backends/imgui_impl_opengl3.cpp" "backends/imgui_impl_opengl3.h")
endif()

if (PX_INCLUDE_VULKAN)
    list(APPEND IMGUI_SOURCES "backends/imgui_impl_vulkan.cpp" "backends/imgui_impl_vulkan.h")
endif()

add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(imgui PUBLIC .)
set_target_properties(imgui PROPERTIES LINKER_LANGUAGE C)

set_target_properties(imgui PROPERTIES FOLDER "Dependencies")

if (PX_INCLUDE_GLFW)
    target_link_libraries(imgui PUBLIC glfw)
endif()

if (PX_INCLUDE_VULKAN)
    target_link_libraries(imgui PUBLIC Vulkan::Vulkan)
endif()