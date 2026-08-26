file(GLOB_RECURSE RUNTIME_VITA_SOURCES
    "src/*.cpp"
    "src/*.h"
)

if (NOT PX_INCLUDE_VITASDK)
    message(FATAL_ERROR "Paradox: PX_INCLUDE_VITASDK must be enabled to build for PS Vita.")
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${RUNTIME_VITA_SOURCES})

add_executable(Runtime-Vita ${RUNTIME_VITA_SOURCES})

target_link_libraries(Runtime-Vita PRIVATE Paradox)

set(WORKING_DIR "${CMAKE_SOURCE_DIR}/WorkingDirectories/Runtime-Vita")
file(MAKE_DIRECTORY "${WORKING_DIR}")

set_target_properties(Runtime-Vita PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${WORKING_DIR}"
)

target_compile_definitions(Runtime-Vita PRIVATE
    $<$<CONFIG:Debug>:PX_DEBUG>
    $<$<CONFIG:Release>:PX_RELEASE>
    $<$<CONFIG:Dist>:PX_DIST>
)

# Debug symbols
#target_compile_options(Runtime-Vita PRIVATE
#    $<$<CONFIG:Debug>:/-g>
#)

# Optimization
target_compile_options(Runtime-Vita PRIVATE
    $<$<CONFIG:Release>:-O2>
    $<$<CONFIG:Dist>:-O2>
)

if (MSVC)
    target_compile_options(Runtime-Vita PRIVATE /MP)
endif()

set_target_properties(Runtime-Vita PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Runtime-Vita"
)
vita_create_self(Runtime.self Runtime-Vita)
vita_create_vpk(Runtime.vpk ${VITA_TITLEID} Runtime.self
    VERSION ${VITA_VERSION}
    NAME    ${VITA_APP_NAME}
    FILE ../WorkingDirectories/Runtime-Vita/sce_sys                                     sce_sys
    FILE ../WorkingDirectories/Runtime-Vita/Assets/Shaders/PSVita/shader.vert 		    Assets/Shaders/PSVita/shader.vert
    FILE ../WorkingDirectories/Runtime-Vita/Assets/Shaders/PSVita/shader.frag 		    Assets/Shaders/PSVita/shader.frag
    FILE ../WorkingDirectories/Runtime-Vita/Assets/Shaders/PSVita/presentShader.vert 	Assets/Shaders/PSVita/presentShader.vert
    FILE ../WorkingDirectories/Runtime-Vita/Assets/Shaders/PSVita/presentShader.frag 	Assets/Shaders/PSVita/presentShader.frag
    FILE ../WorkingDirectories/Runtime-Vita/Assets/Shaders/PSVita/Quad2D.vert           Assets/Shaders/PSVita/Quad2D.vert
    FILE ../WorkingDirectories/Runtime-Vita/Assets/Shaders/PSVita/Quad2D.frag           Assets/Shaders/PSVita/Quad2D.frag
    FILE ../WorkingDirectories/Runtime-Vita/Assets/Textures/texture.jpg 			    Assets/Textures/texture.jpg
    FILE ../WorkingDirectories/Runtime-Vita/Assets/Textures/Controls.png 		        Assets/Textures/Controls.png
)
