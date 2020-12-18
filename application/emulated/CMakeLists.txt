cmake_minimum_required(VERSION 3.16)

if(NOT CMAKE_CROSSCOMPILING)
    project(
        ADCS_emulated_interfaces 
        VERSION 1.0 
        LANGUAGES C
    )
    
    message("CONFIGURING ${PROJECT_NAME}")
    set(BUILD_TARGET ${PROJECT_NAME})

    ADCS_add_library(${BUILD_TARGET} STATIC)
    file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
    file(GLOB_RECURSE headers "${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h")
    foreach(hdr ${headers})
        get_filename_component(hdr_dir ${hdr} DIRECTORY)
        list(APPEND include_dirs ${hdr_dir})
    endforeach(hdr ${headers})
    list(REMOVE_DUPLICATES include_dirs)

    target_include_directories(${BUILD_TARGET} PUBLIC ${include_dirs})
    target_sources(${BUILD_TARGET} PRIVATE ${sources})

    find_package(Threads REQUIRED)
    if(Threads_FOUND) 
        # https://sourceforge.net/projects/mingw-w64/
        if(WIN32)
            if(CMAKE_USE_WIN32_THREADS_INIT)
                target_link_libraries(${BUILD_TARGET} PUBLIC pthread)
                target_compile_options(${BUILD_TARGET} PUBLIC "-lpthread")
            else()
                message(FATAL_ERROR "BLAHHH ")
            endif()
        elseif(UNIX)
            if(CMAKE_USE_PTHREADS_INIT)
                target_link_libraries(${BUILD_TARGET} PUBLIC pthread)
                target_compile_options(${BUILD_TARGET} PUBLIC "-lpthread")
            else()
                message(FATAL_ERROR "BLAHHH ")
            endif()
        endif()
    else()
        message(FATAL_ERROR "No threading libraries have been found. Aborting!")
    endif()
endif(NOT CMAKE_CROSSCOMPILING)
