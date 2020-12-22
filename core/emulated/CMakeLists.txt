cmake_minimum_required(VERSION 3.16)

if(NOT CMAKE_CROSSCOMPILING)
    project(
        ADCS_IF_EMU 
        VERSION 1.0
        DESCRIPTION "ADCS interface emulation for execution on a native machine"
        LANGUAGES C
    )
    message("CONFIGURING ${PROJECT_NAME}")
    set(BUILD_TARGET ${PROJECT_NAME})

    ADCS_add_library(${BUILD_TARGET})
    file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
    file(GLOB_RECURSE headers "${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h")
    foreach(hdr ${headers})
        get_filename_component(hdr_dir ${hdr} DIRECTORY)
        list(APPEND include_dirs ${hdr_dir})
    endforeach(hdr ${headers})
    list(REMOVE_DUPLICATES include_dirs)

    target_include_directories(${BUILD_TARGET} PUBLIC ${include_dirs})
    target_sources(${BUILD_TARGET} PRIVATE ${sources})
    target_link_libraries(${BUILD_TARGET} PRIVATE INJECTION_API)
    target_link_libraries(${BUILD_TARGET} PRIVATE ADCS_OBC_INTERFACE)

    find_package(Threads REQUIRED)
    if(Threads_FOUND) 
        if(WIN32)
            if(CMAKE_USE_WIN32_THREADS_INIT)
                target_link_libraries(${BUILD_TARGET} PRIVATE pthread)
            else()
                message(FATAL_ERROR "BLAHHH ")
            endif()
        elseif(UNIX)
            if(CMAKE_USE_PTHREADS_INIT)
                target_link_libraries(${BUILD_TARGET} PRIVATE pthread)
            else()
                message(FATAL_ERROR "BLAHHH ")
            endif()
        endif()
    else()
        message(FATAL_ERROR "No threading libraries have been found. Aborting!")
    endif()
endif(NOT CMAKE_CROSSCOMPILING)
