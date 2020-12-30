
cmake_minimum_required(VERSION 3.16)

if(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(BACKUP_CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif(CMAKE_RUNTIME_OUTPUT_DIRECTORY)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

if(CMAKE_CROSSCOMPILING)
    file(GLOB example_sources "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
    foreach(src ${example_sources})
        get_filename_component(example_suffix ${src} NAME_WLE)
        set(example_target "${LIB}_${example_suffix}")
        if(NOT TARGET ${example_target})
            ADCS_add_executable(${example_target})
            target_sources(${example_target} PRIVATE ${src})
            target_link_libraries(${example_target} PRIVATE ${LIB})
        endif(NOT TARGET ${example_target})
        unset(example_target)
    endforeach(src ${example_sources})
else()
    message("Peripheral driver tests will not be built because we are not cross compiling")
endif(CMAKE_CROSSCOMPILING)

if(BACKUP_CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BACKUP_CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif(BACKUP_CMAKE_RUNTIME_OUTPUT_DIRECTORY)