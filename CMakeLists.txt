cmake_minimum_required(VERSION 3.0)
project(MyProject)

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
            add_executable(${week_number}_${filename}_executable ${file})
        endforeach()
    endforeach()
endfunction()

# CBra files
create_executables("CBra")

# gruppetimer files
create_executables("gruppetimer")

# plenum files
create_executables("plenum")


## Oblig files
#file(GLOB OBLIG_FILES "oblig/*.c")
#add_executable(oblig_executables ${OBLIG_FILES})

# Oblig files
file(GLOB OBLIG_FILES "oblig/*.c")
foreach(file ${OBLIG_FILES})
    get_filename_component(filename ${file} NAME_WE)
    add_executable(${filename}_executable ${file} ${OBLIG_FILES})
endforeach()