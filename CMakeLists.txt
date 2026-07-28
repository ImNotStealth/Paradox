cmake_minimum_required(VERSION 3.23)

project(Paradox CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_SUPPRESS_REGENERATION true)   # removes ZERO_CHECK
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Runtime)

set(OUTPUT_DIR "$<CONFIG>-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${OUTPUT_DIR}/$<TARGET_PROPERTY:NAME>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${OUTPUT_DIR}/$<TARGET_PROPERTY:NAME>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${OUTPUT_DIR}/$<TARGET_PROPERTY:NAME>")

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Dist" CACHE STRING "" FORCE)

set(CMAKE_SHARED_LINKER_FLAGS_DIST "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")
set(CMAKE_EXE_LINKER_FLAGS_DIST "${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
set(CMAKE_MODULE_LINKER_FLAGS_DIST "${CMAKE_MODULE_LINKER_FLAGS_RELEASE}")

if(NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type")
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release Dist)
endif()

if (MSVC)
    add_compile_options(/utf-8)
endif()

# Paradox Options
option(PX_BUILD_DLL "Build Paradox as a DLL" ON)
option(PX_INCLUDE_SPDLOG "Use spdlog for logging" ON)
option(PX_INCLUDE_GLFW "Include GLFW support" ON)
option(PX_INCLUDE_VULKAN "Include Vulkan support" ON)
option(PX_INCLUDE_OPENGL "Include OpenGL support" ON)
option(PX_INCLUDE_GLAD "Include GLAD (required for desktop OpenGL)" ON)
option(PX_INCLUDE_IMGUI "Include ImGui" ON)
option(PX_INCLUDE_VITASDK "Include PS Vita support" OFF)

add_subdirectory(Paradox)
add_subdirectory(Paradox-Editor)
add_subdirectory(Runtime)
if (PX_INCLUDE_VITASDK)
    add_subdirectory(Runtime-Vita)
endif()
