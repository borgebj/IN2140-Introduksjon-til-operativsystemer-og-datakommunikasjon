cmake_minimum_required(VERSION 3.0)
project(MyProject)

# Add all source files
file(GLOB_RECURSE SOURCES *.c)

# CBra
add_executable(unbuffered CBra/uke8/unbuffered.c)

# Oblig
add_executable(inode obliger/inode.c
        obliger/inode.c
        obliger/allocation.c
        obliger/create_fs.c
        obliger/create_fs_1.c
        obliger/create_fs_2.c
        obliger/create_fs_3.c
        obliger/del_fs.c
        obliger/load_fs.c
        CBra/uke8/buffered_vs_unbuffered.c
        CBra/uke8/buffered.c)
add_executable(buffered CBra/uke8/buffered.c)
add_executable(buffered_vs_unbuffered CBra/uke8/buffered_vs_unbuffered.c)
add_executable(bin_files CBra/uke8/bin_files.c
        CBra/uke8/bin_files.c)