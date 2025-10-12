# Set the cmake minimum version to 3.5.1
cmake_minimum_required(VERSION 3.5.1)

project(CellsEvolution)

# Add SDL2 CMake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/sdl2)

find_package(OpenMP QUIET)

# Function to embed binary files as C symbols
function(embed_resource target_name input_file output_name)
    get_filename_component(input_filename ${input_file} NAME)
    set(output_file "${CMAKE_CURRENT_BINARY_DIR}/${output_name}.o")

    add_custom_command(
        OUTPUT ${output_file}
        COMMAND ${CMAKE_LINKER} -r -b binary -o ${output_file} ${input_file}
        DEPENDS ${input_file}
        COMMENT "Embedding ${input_filename}"
    )

    target_sources(${target_name} PRIVATE ${output_file})
endfunction()

# Add all c source files under the src directory
file(GLOB SOURCES "main.c" "src/*.c" "src/*/*.c" "src/*/*/*.c")
add_executable(${PROJECT_NAME} ${SOURCES})

# Set output directory for executable to project root
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

# Embed resources
embed_resource(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/ressources/bipboup/normal/skin.png" "skin_normal")
embed_resource(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/ressources/bipboup/normal/eyes.png" "eyes_normal")
embed_resource(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/ressources/bipboup/normal/leaf.png" "leaf_normal")
embed_resource(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/ressources/bipboup/normal/ass.png" "ass_normal")
embed_resource(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/ressources/bipboup/shiny/skin.png" "skin_shiny")
embed_resource(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/ressources/bipboup/shiny/eyes.png" "eyes_shiny")
embed_resource(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/ressources/bipboup/shiny/leaf.png" "leaf_shiny")
embed_resource(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/ressources/bipboup/shiny/ass.png" "ass_shiny")

# Add all headers files under the include directory
target_include_directories(${PROJECT_NAME} PRIVATE include)

# Add compiler errors/warnings flags
target_compile_options(${PROJECT_NAME} PRIVATE $<$<C_COMPILER_ID:MSVC>:/W4 /WX>)
target_compile_options(${PROJECT_NAME} PRIVATE $<$<NOT:$<C_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic -Werror>)

if (OpenMP_C_FOUND)
  target_link_libraries(${PROJECT_NAME} PRIVATE OpenMP::OpenMP_C)
  target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_OPENMP=1)
else()
  message(WARNING "OpenMP non trouvé : compilation sans parallélisme.")
  target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_OPENMP=0)
endif()

# Add SDL2 library
find_package(SDL2 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::Main)

# Add SDL2_image library
find_package(SDL2_image REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::Image)

# Add SDL2_ttf library
#find_package(SDL2_ttf REQUIRED)
#target_link_libraries(${PROJECT_NAME} SDL2::TTF)

# Add SDL2_net library
#find_package(SDL2_net REQUIRED)
#target_link_libraries(${PROJECT_NAME} SDL2::Net)

# Add SDL2_mixer library
#find_package(SDL2_mixer REQUIRED)
#target_link_libraries(${PROJECT_NAME} SDL2::Mixer)

# Add SDL2_gfx library
find_package(SDL2_gfx REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::GFX)

# Include SDL2 headers
target_include_directories(${PROJECT_NAME} PRIVATE
  ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_GFX_INCLUDE_DIRS})

# Link the math library
target_link_libraries(${PROJECT_NAME} PRIVATE m)

# Add pthread for multithreading
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

# Set compiler flags for debug builds (-g for debug symbols)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

# Optionally, set other flags for release (like optimizations)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
