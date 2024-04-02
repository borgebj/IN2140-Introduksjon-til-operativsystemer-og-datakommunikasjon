cmake_minimum_required(VERSION 3.0)
project(IN2140-Introduksjon-til-operativsystemer-og-datakommunikasjon)

# Function to create executable targets for a given directory
# goes through all weeks
# requirement: folder must have "uke#" folders
function(create_executables directory_name)
    file(GLOB WEEK_DIRECTORIES "${directory_name}/uke*")
    foreach(week_dir ${WEEK_DIRECTORIES})
        get_filename_component(week_number ${week_dir} NAME)
        file(GLOB_RECURSE WEEK_SOURCE_FILES "${week_dir}/*.c")
        foreach(file ${WEEK_SOURCE_FILES})
            get_filename_component(filename ${file} NAME_WE)
            # Append directory name to ensure unique target names
            string(REPLACE "/" "_" target_name_prefix ${week_dir})
            add_executable(${target_name_prefix}_${week_number}_${filename}_executable ${file})
        endforeach()
    endforeach()
endfunction()

# CBra files
create_executables("CBra")

# gruppetimer files
create_executables("gruppetimer")

# plenum files
create_executables("plenum")

# oppgaver files
create_executables("oppgaver")

# Oblig files
file(GLOB OBLIG_FILES "oblig/*.c")
foreach(file ${OBLIG_FILES})
    get_filename_component(filename ${file} NAME_WE)
    add_executable(${filename}_executable ${file} ${OBLIG_FILES})
endforeach()

# annet
file(GLOB SOURCES "*.c")
foreach(source_file ${SOURCES})
    get_filename_component(filename ${source_file} NAME_WE)
    add_executable(${filename}_executable ${source_file}
            CBra/uke9/lenkeliste.c)
endforeach()