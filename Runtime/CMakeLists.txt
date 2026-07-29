file(GLOB_RECURSE RUNTIME_SOURCES
    "src/*.cpp"
    "src/*.h"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${RUNTIME_SOURCES})

add_executable(Runtime ${RUNTIME_SOURCES})

target_link_libraries(Runtime PRIVATE Paradox)

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

set_target_properties(Runtime PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${WORKING_DIR}"
)

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
