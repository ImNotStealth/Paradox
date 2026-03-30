set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

file(GLOB_RECURSE PARADOX_SOURCES
    "src/*.cpp"
    "src/*.h"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${PARADOX_SOURCES})

add_library(Paradox SHARED ${PARADOX_SOURCES})

target_include_directories(Paradox PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm/include
)

target_precompile_headers(Paradox PRIVATE src/pxpch.h)

target_compile_definitions(Paradox PRIVATE
    $<$<CONFIG:Debug>:PX_DEBUG>
    $<$<CONFIG:Release>:PX_RELEASE>
    $<$<CONFIG:Dist>:PX_DIST>
    PX_BUILD_DLL
)

# Compiler options (GCC/Clang)
target_compile_options(Paradox PRIVATE
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Release>:-O2>
    $<$<CONFIG:Dist>:-O2>
)

# GLFW (build from vendor)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(vendor/glfw)
target_link_libraries(Paradox PUBLIC glfw)

# Vulkan
find_package(Vulkan REQUIRED)
target_link_libraries(Paradox PUBLIC Vulkan::Vulkan)
