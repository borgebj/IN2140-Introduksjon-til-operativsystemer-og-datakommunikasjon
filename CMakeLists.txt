cmake_minimum_required(VERSION 3.0)
project(IN2140-Introduksjon-til-operativsystemer-og-datakommunikasjon)

# Add source files for each category and week combination
file(GLOB_RECURSE CATEGORY_WEEKS "CBra/*/*.c" "gruppetimer/*/*.c" "oblig/*/*.c" "plenum/*/*.c" "ukesoppgaver/*/*.c")

# Create an executable target for each combination of category and week
foreach(program ${CATEGORY_WEEKS})
    # Extract the filename without the extension
    get_filename_component(file_name ${program} NAME_WE)
    # Extract the directory path of the file
    get_filename_component(dir_name ${program} DIRECTORY)
    # Get the relative path from the project root to the file's directory
    file(RELATIVE_PATH relative_dir ${CMAKE_SOURCE_DIR} ${dir_name})
    # Replace directory separators with underscores to create a valid target name
    string(REPLACE "/" "_" target_name "${relative_dir}_${file_name}_executable")

    # Add the executable target for this category-week combination
    add_executable(${target_name} ${program}
            oblig/inode.c)
endforeach()