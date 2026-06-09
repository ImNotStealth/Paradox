set(CMAKE_CXX_STANDARD 17)

if (PX_BUILD_DLL)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
else()
    set(CMAKE_POSITION_INDEPENDENT_CODE OFF)
endif()

file(GLOB_RECURSE PARADOX_SOURCES
    "src/*.cpp"
    "src/*.h"
)

# Filter out every platform specific file, they'll be added back in based on options.
list(FILTER PARADOX_SOURCES EXCLUDE REGEX ".*/Paradox/Platform/.*")

if (PX_INCLUDE_VULKAN)
    message(STATUS "Paradox: Including Vulkan")
    file(GLOB_RECURSE VULKAN_SOURCES "src/Paradox/Platform/Vulkan/*.cpp" "src/Paradox/Platform/Vulkan/*.h")
    list(APPEND PARADOX_SOURCES ${VULKAN_SOURCES})
endif()

if (PX_INCLUDE_OPENGL)
    message(STATUS "Paradox: Including OpenGL")
    file(GLOB_RECURSE OPENGL_SOURCES "src/Paradox/Platform/OpenGL/*.cpp" "src/Paradox/Platform/OpenGL/*.h")
    list(APPEND PARADOX_SOURCES ${OPENGL_SOURCES})
endif()

if (PX_INCLUDE_GLFW)
    message(STATUS "Paradox: Including GLFW")
    file(GLOB_RECURSE GLFW_SOURCES "src/Paradox/Platform/GLFW/*.cpp" "src/Paradox/Platform/GLFW/*.h")
    list(APPEND PARADOX_SOURCES ${GLFW_SOURCES})
endif()

if (PX_INCLUDE_VITASDK)
    if (NOT CMAKE_HOST_UNIX)
        message(FATAL_ERROR "Paradox: Support for PS Vita is only available on Linux.")
    endif()
    
    message(STATUS "Paradox: Including VitaSDK")
    file(GLOB_RECURSE VITASDK_SOURCES "src/Paradox/Platform/Vita/*.cpp" "src/Paradox/Platform/Vita/*.h")
    list(APPEND PARADOX_SOURCES ${VITASDK_SOURCES})
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${PARADOX_SOURCES})

if (PX_BUILD_DLL)
    message(STATUS "Paradox: Building as shared library")
    add_library(Paradox SHARED ${PARADOX_SOURCES})
else()
    message(STATUS "Paradox: Building as static library")
    add_library(Paradox STATIC ${PARADOX_SOURCES})
endif()

if (MSVC)
    set_target_properties(Paradox PROPERTIES VS_GLOBAL_DisableFastUpToDateCheck true)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    target_link_options(Paradox PRIVATE $<$<CONFIG:Debug>:/NODEFAULTLIB:LIBCMT>)
endif()

target_include_directories(Paradox PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm/include
)

if (PX_INCLUDE_SPDLOG)
    message(STATUS "Paradox: Using spdlog for logging")
    target_compile_definitions(Paradox PRIVATE PX_INCLUDE_SPDLOG)
    target_include_directories(Paradox PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog/include)
endif()

if (PX_INCLUDE_VULKAN)
    target_compile_definitions(Paradox PRIVATE PX_INCLUDE_VULKAN)
    find_package(Vulkan REQUIRED)
    target_link_libraries(Paradox PUBLIC Vulkan::Vulkan)
endif()

if (PX_INCLUDE_GLFW)
    target_compile_definitions(Paradox PRIVATE PX_INCLUDE_GLFW)
endif()

if (PX_INCLUDE_OPENGL)
    target_compile_definitions(Paradox PRIVATE PX_INCLUDE_OPENGL)
endif()

if (PX_INCLUDE_GLAD)
    message(STATUS "Paradox: Including GLAD")
    target_compile_definitions(Paradox PRIVATE PX_INCLUDE_GLAD)
endif()

if (PX_INCLUDE_VITASDK)
    include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)
    target_compile_definitions(Paradox PRIVATE PX_INCLUDE_VITASDK)
    target_include_directories(Paradox PUBLIC
        $ENV{VITASDK}/arm-vita-eabi/include
        $ENV{VITASDK}/arm-vita-eabi/include/freetype2
    )
    target_link_libraries(Paradox PUBLIC
        freetype
        png
        z
        vitaGL
        vita2d
        SceLibKernel_stub
        ScePower_stub
        SceAppMgr_stub
        SceAppUtil_stub
        mathneon
        c
        SceCommonDialog_stub
        m
        SceGxm_stub
        SceDisplay_stub
        ScePgf_stub
        SceSysmodule_stub
        SceUsbSerial_stub
        SceUsbd_stub
        SceTouch_stub
        SceAudio_stub
        SceNet_stub
        zip
        bz2
        vitashark
        SceShaccCgExt
        taihen_stub
        SceShaccCg_stub
        SceKernelDmacMgr_stub
        SceCtrl_stub
        SceRtc_stub
        stdc++
        pthread
    )
endif()

target_precompile_headers(Paradox PRIVATE src/pxpch.h)

target_compile_definitions(Paradox PRIVATE
    $<$<CONFIG:Debug>:PX_DEBUG>
    $<$<CONFIG:Release>:PX_RELEASE>
    $<$<CONFIG:Dist>:PX_DIST>
    $<$<BOOL:${PX_BUILD_DLL}>:PX_DLL_EXPORT>
)

target_compile_definitions(Paradox PUBLIC
    $<$<BOOL:${PX_BUILD_DLL}>:PX_BUILD_DLL>
)

# Compiler options (GCC/Clang)
target_compile_options(Paradox PRIVATE
    $<$<CONFIG:Release>:-O2>
    $<$<CONFIG:Dist>:-O2>
)

# Building from vendor
if (PX_INCLUDE_GLFW)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS OFF)
    add_subdirectory(vendor/glfw)
    target_link_libraries(Paradox PUBLIC glfw)
endif()

if (PX_INCLUDE_GLAD)
    add_subdirectory(vendor/glad)
    target_link_libraries(Paradox PUBLIC glad)
endif()

add_subdirectory(vendor/imgui)
target_link_libraries(Paradox PUBLIC imgui)
