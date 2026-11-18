# Generate a file containing the path (apparently Ninja doesn't print the path if nothing is compiled so this is a quick fix)
function(px_print_bin_path target)
    set(_file "${CMAKE_CURRENT_BINARY_DIR}/px_bin_path_$<CONFIG>.txt")
    file(GENERATE OUTPUT "${_file}" CONTENT "PX_BIN_PATH=$<TARGET_FILE:${target}>\n")

    add_custom_target(Print${target}BinPath
        COMMAND ${CMAKE_COMMAND} -E cat "${_file}"
        VERBATIM
    )
    add_dependencies(${target} Print${target}BinPath)
endfunction()
