set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE OFF)

file(GLOB_RECURSE PARADOX_SOURCES
    "src/*.cpp"
    "src/*.h"
)

# Filter out excluded libraries
if (NOT PX_INCLUDE_VULKAN)
    message(STATUS "Excluding Vulkan support")
    list(FILTER PARADOX_SOURCES EXCLUDE REGEX ".*/Paradox/Platform/Vulkan/.*")
endif()

#Temporary
list(FILTER PARADOX_SOURCES EXCLUDE REGEX ".*/Paradox/Platform/GLFW/.*")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${PARADOX_SOURCES})

if (PX_INCLUDE_VITASDK)
    add_library(Paradox STATIC ${PARADOX_SOURCES})
else()
    add_library(Paradox SHARED ${PARADOX_SOURCES})
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

# Paradox Options
if (PX_INCLUDE_SPDLOG)
    target_compile_definitions(Paradox PRIVATE PX_INCLUDE_SPDLOG)
    target_include_directories(Paradox PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog/include)
endif()

if (PX_INCLUDE_VULKAN)
    target_compile_definitions(Paradox PRIVATE PX_INCLUDE_VULKAN)
    find_package(Vulkan REQUIRED)
    target_link_libraries(Paradox PUBLIC Vulkan::Vulkan)
endif()

if (PX_INCLUDE_VITASDK)
    if (NOT CMAKE_HOST_UNIX)
        message(FATAL_ERROR "Support for PS Vita is only available on Linux.")
    endif()

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
    PX_BUILD_DLL
)

# Compiler options (GCC/Clang)
target_compile_options(Paradox PRIVATE
    $<$<CONFIG:Release>:-O2>
    $<$<CONFIG:Dist>:-O2>
)

if (NOT PX_INCLUDE_VITASDK)
    # GLFW (build from vendor)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS OFF)
    add_subdirectory(vendor/glfw)
    target_link_libraries(Paradox PUBLIC glfw)

    # GLAD
    add_subdirectory(vendor/glad)
    target_link_libraries(Paradox PUBLIC glad)
endif()
