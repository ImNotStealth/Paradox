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

px_print_bin_path(Runtime-Vita)

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


string(REGEX MATCH "^([0-9]+)(\\.([0-9]+))?" _ver "${PX_PROJECT_VERSION}")
set(_major "${CMAKE_MATCH_1}")
set(_minor "${CMAKE_MATCH_3}")

if(_major STREQUAL "")
    message(FATAL_ERROR "PX_PROJECT_VERSION '${PX_PROJECT_VERSION}' must start with a number")
endif()
if(_minor STREQUAL "")
    set(_minor 0)
endif()
string(REGEX REPLACE "^([0-9])$" "0\\1" _major "${_major}")
string(REGEX REPLACE "^([0-9])$" "0\\1" _minor "${_minor}")

set(PX_PROJECT_VERSION_FORMAT "${_major}.${_minor}")

set_target_properties(Runtime-Vita PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Runtime-Vita"
)
vita_create_self(Runtime.self Runtime-Vita)
vita_create_vpk(Runtime.vpk ${PX_PROJECT_TITLEID} Runtime.self
    VERSION ${PX_PROJECT_VERSION_FORMAT}
    NAME    ${PX_PROJECT_NAME}
    FILE    ${PX_PROJECT_TEMP_PATH} .
)
