cmake_minimum_required(VERSION 3.16)
project(MSP430F5529_DRIVERS_PROJECT VERSION 1.0 LANGUAGES C)

add_library(MSP430F5529_DRIVERS STATIC)

file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
foreach(hdr ${headers})
    get_filename_component(hdr_dir ${hdr} DIRECTORY)
    list(APPEND include_dirs ${hdr_dir})
endforeach(hdr ${headers})
list(REMOVE_DUPLICATES include_dirs)
target_include_directories(MSP430F5529_DRIVERS PUBLIC ${include_dirs})
target_sources(MSP430F5529_DRIVERS PRIVATE ${sources})