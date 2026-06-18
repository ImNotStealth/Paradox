file(GLOB_RECURSE EDITOR_SOURCES
    "src/*.cpp"
    "src/*.h"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${EDITOR_SOURCES})

add_executable(Paradox-Editor ${EDITOR_SOURCES})

target_link_libraries(Paradox-Editor PRIVATE Paradox)

if (MSVC AND PX_BUILD_DLL)
    set_target_properties(Paradox-Editor PROPERTIES VS_GLOBAL_DisableFastUpToDateCheck true)
    add_custom_command(TARGET Paradox-Editor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_RUNTIME_DLLS:Paradox-Editor>
            $<TARGET_FILE_DIR:Paradox-Editor>
        COMMAND_EXPAND_LISTS
    )
endif()

set(WORKING_DIR "${CMAKE_SOURCE_DIR}/WorkingDirectories/Paradox-Editor")
file(MAKE_DIRECTORY "${WORKING_DIR}")

target_include_directories(Paradox-Editor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set_target_properties(Paradox-Editor PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${WORKING_DIR}"
)

target_compile_definitions(Paradox-Editor PRIVATE
    $<$<CONFIG:Debug>:PX_DEBUG>
    $<$<CONFIG:Release>:PX_RELEASE>
    $<$<CONFIG:Dist>:PX_DIST>
)

# Debug symbols
#target_compile_options(Paradox-Editor PRIVATE
#    $<$<CONFIG:Debug>:/-g>
#)

# Optimization
target_compile_options(Paradox-Editor PRIVATE
    $<$<CONFIG:Release>:-O2>
    $<$<CONFIG:Dist>:-O2>
)

if (MSVC)
    target_compile_options(Paradox-Editor PRIVATE /MP)
endif()