file(GLOB_RECURSE PARADOX_SOURCES
    "src/*.cpp"
    "src/*.h"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${PARADOX_SOURCES})

# Set as DLL
add_library(Paradox SHARED ${PARADOX_SOURCES})

target_include_directories(Paradox PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog/include
)

target_precompile_headers(Paradox PRIVATE src/pxpch.h)

set(CMAKE_CXX_FLAGS_DIST "${CMAKE_CXX_FLAGS_RELEASE}")
set(CMAKE_C_FLAGS_DIST "${CMAKE_C_FLAGS_RELEASE}")

# Configuration defines
target_compile_definitions(Paradox PRIVATE
    $<$<CONFIG:Debug>:PX_DEBUG>
    $<$<CONFIG:Release>:PX_RELEASE>
    $<$<CONFIG:Dist>:PX_DIST>
)

target_compile_definitions(Paradox PRIVATE PX_BUILD_DLL)

# Debug symbols
target_compile_options(Paradox PRIVATE
    $<$<CONFIG:Debug>:/Zi>
)

# Optimization
target_compile_options(Paradox PRIVATE
    $<$<CONFIG:Release>:/O2>
    $<$<CONFIG:Dist>:/O2>
)

# Dependencies

# GLFW
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF) # static glfw
add_subdirectory(vendor/glfw)
add_dependency_folder(glfw)
add_dependency_folder(update_mappings)