add_library(stb_image STATIC src/stb_image.cpp)
target_include_directories(stb_image PUBLIC include)
set_target_properties(stb_image PROPERTIES LINKER_LANGUAGE C)

set_target_properties(stb_image PROPERTIES FOLDER "Dependencies")