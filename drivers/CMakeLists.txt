cmake_minimum_required(VERSION 3.16)
if(CMAKE_CROSSCOMPILING)
    project(
        ADCS_drivers 
        VERSION 1.0 
        LANGUAGES C
    )
    
    message("CONFIGURING ${PROJECT_NAME}")
    set(BUILD_TARGET ${PROJECT_NAME})
    ADCS_add_library(${BUILD_TARGET} STATIC)

    file(GLOB_RECURSE sources 
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c" 
        "${CMAKE_CURRENT_SOURCE_DIR}/BSP/*.c"
    )
    
    file(GLOB_RECURSE headers 
        "${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h" 
        "${CMAKE_CURRENT_SOURCE_DIR}/BSP/*.h"
    )

    foreach(hdr ${headers})
        get_filename_component(hdr_dir ${hdr} DIRECTORY)
        list(APPEND include_dirs ${hdr_dir})
    endforeach(hdr ${headers})
    list(REMOVE_DUPLICATES include_dirs)

    target_include_directories(${BUILD_TARGET} PUBLIC ${include_dirs})
    target_sources(${BUILD_TARGET} PRIVATE ${sources})
    target_link_libraries(${BUILD_TARGET} PUBLIC INJECTION_API)
endif(CMAKE_CROSSCOMPILING)


