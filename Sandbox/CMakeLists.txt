file(GLOB_RECURSE SANDBOX_SOURCES
    "src/*.cpp"
    "src/*.h"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${SANDBOX_SOURCES})

add_executable(Sandbox ${SANDBOX_SOURCES})

target_link_libraries(Sandbox PRIVATE Paradox)

if (MSVC AND PX_BUILD_DLL)
    set_target_properties(Sandbox PROPERTIES VS_GLOBAL_DisableFastUpToDateCheck true)
    add_custom_command(TARGET Sandbox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_RUNTIME_DLLS:Sandbox>
            $<TARGET_FILE_DIR:Sandbox>
        COMMAND_EXPAND_LISTS
    )
endif()

set(WORKING_DIR "${CMAKE_SOURCE_DIR}/WorkingDirectories/Sandbox")
file(MAKE_DIRECTORY "${WORKING_DIR}")

set_target_properties(Sandbox PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${WORKING_DIR}"
)

target_compile_definitions(Sandbox PRIVATE
    $<$<CONFIG:Debug>:PX_DEBUG>
    $<$<CONFIG:Release>:PX_RELEASE>
    $<$<CONFIG:Dist>:PX_DIST>
)

# Debug symbols
#target_compile_options(Sandbox PRIVATE
#    $<$<CONFIG:Debug>:/-g>
#)

# Optimization
target_compile_options(Sandbox PRIVATE
    $<$<CONFIG:Release>:-O2>
    $<$<CONFIG:Dist>:-O2>
)

if (PX_INCLUDE_VITASDK)
    set_target_properties(Sandbox PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Sandbox"
    )
    vita_create_self(Sandbox.self Sandbox)
    vita_create_vpk(Sandbox.vpk ${VITA_TITLEID} Sandbox.self
        VERSION ${VITA_VERSION}
        NAME    ${VITA_APP_NAME}
        FILE ../WorkingDirectories/Sandbox/shaders/psvita/shader.vert shaders/psvita/shader.vert
        FILE ../WorkingDirectories/Sandbox/shaders/psvita/shader.frag shaders/psvita/shader.frag
    )
endif()
