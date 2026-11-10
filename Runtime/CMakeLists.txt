file(GLOB_RECURSE RUNTIME_SOURCES
    "src/*.cpp"
    "src/*.h"
)

set(PX_PROJECT_NAME "${PX_PROJECT_NAME}")
set(PX_PROJECT_AUTHOR "${PX_PROJECT_AUTHOR}")
set(PX_PROJECT_COPYRIGHT "${PX_PROJECT_COPYRIGHT}")
set(PX_PROJECT_VERSION "${PX_PROJECT_VERSION}")
set(PX_PROJECT_ICON "${PX_PROJECT_ICON}")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${RUNTIME_SOURCES})

add_executable(Runtime ${RUNTIME_SOURCES})
if (MSVC)
    # This is a lot of code for such a small thing :/
    # Parses any version number from the project version (ex: 1.5.2b becomes 1.5.2.0)
    string(REPLACE "." ";" _px_ver_raw "${PX_PROJECT_VERSION}")
    set(_px_ver_parts "")
    foreach (_part IN LISTS _px_ver_raw)
        if (_part MATCHES "^([0-9]+)")
            list(APPEND _px_ver_parts ${CMAKE_MATCH_1})
        else()
            list(APPEND _px_ver_parts 0)
        endif()
    endforeach()

    list(LENGTH _px_ver_parts _px_ver_len)
    while (_px_ver_len LESS 4)
        list(APPEND _px_ver_parts 0)
        math(EXPR _px_ver_len "${_px_ver_len} + 1")
    endwhile()

    list(GET _px_ver_parts 0 PX_PROJECT_VERSION_MAJOR)
    list(GET _px_ver_parts 1 PX_PROJECT_VERSION_MINOR)
    list(GET _px_ver_parts 2 PX_PROJECT_VERSION_PATCH)
    list(GET _px_ver_parts 3 PX_PROJECT_VERSION_TWEAK)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/version.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/version.rc
        @ONLY)
    target_sources(Runtime PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
endif()

target_link_libraries(Runtime PRIVATE Paradox)

add_custom_command(TARGET Runtime PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "PX_BIN_PATH=$<TARGET_FILE:Runtime>"
    VERBATIM
)

if (MSVC AND PX_BUILD_DLL)
    set_target_properties(Runtime PROPERTIES VS_GLOBAL_DisableFastUpToDateCheck true)
    add_custom_command(TARGET Runtime POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_RUNTIME_DLLS:Runtime>
            $<TARGET_FILE_DIR:Runtime>
        COMMAND_EXPAND_LISTS
    )
endif()

set(WORKING_DIR "${CMAKE_SOURCE_DIR}/WorkingDirectories/Runtime")
file(MAKE_DIRECTORY "${WORKING_DIR}")

set_target_properties(Runtime PROPERTIES WIN32_EXECUTABLE $<CONFIG:Dist>)
set_target_properties(Runtime PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${WORKING_DIR}")

target_compile_definitions(Runtime PRIVATE
    $<$<CONFIG:Debug>:PX_DEBUG>
    $<$<CONFIG:Release>:PX_RELEASE>
    $<$<CONFIG:Dist>:PX_DIST>
)

# Debug symbols
#target_compile_options(Runtime PRIVATE
#    $<$<CONFIG:Debug>:/-g>
#)

# Optimization
target_compile_options(Runtime PRIVATE
    $<$<CONFIG:Release>:-O2>
    $<$<CONFIG:Dist>:-O2>
)

if (MSVC)
    target_compile_options(Runtime PRIVATE /MP)
endif()
