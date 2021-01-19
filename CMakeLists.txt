cmake_minimum_required(VERSION 3.16)
################################################################################
# @brief Top level CMake file for the ADCS project
# @author: Carl Mattatall (cmattatall2@gmail.com)
#
################################################################################
# USEFUL DOCUMENTATION SECTION FOR THOSE NEW TO CMAKE
################################################################################
#
# best practices:
#   https://cliutils.gitlab.io/modern-cmake/
#
# variables:
#   https://cmake.org/cmake/help/latest/manual/cmake-variables.7.html
#
# booleans:
#   https://stackoverflow.com/questions/41079291/cmake-use-of-true-false-or-on-off-or-yes-no-in-generator-expressions
#
# behaviour of >if< 
#   https://cmake.org/cmake/help/v3.18/command/if.html
#
#
# OTHER : 
#   on windows, you probably need to install gcc
#   https://sourceforge.net/projects/mingw-w64/
#   
#
#
################################################################################
option(BUILD_TESTING "[ON/OFF] Boolean to choose to cross compile or not" OFF)
option(BUILD_EXAMPLES "[ON/OFF] Build examlples in addition to library" OFF)
option(ANALYZE "[ON/OFF] Perform static analysis on project" OFF)



set(MSP430_MCU "msp430f5529" CACHE STRING "Lowercase MPN of target MCU. Used by msp430-elf-gcc" FORCE)

################################################################################
#                       PROJECT START                                          #
################################################################################

project(
    LORIS_ADCS_PROJECT
    VERSION 0.1
    DESCRIPTION "ADCS Firmware Project for DSS LORIS Satellite"
    LANGUAGES C ASM
)

set(SUPPORTED_BUILD_TYPES "")
list(APPEND SUPPORTED_BUILD_TYPES "Debug")
list(APPEND SUPPORTED_BUILD_TYPES "Release")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${SUPPORTED_BUILD_TYPES})
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type chosen by the user at configure time")
else()
    if(NOT CMAKE_BUILD_TYPE IN_LIST SUPPORTED_BUILD_TYPES)
        message("Build type : ${CMAKE_BUILD_TYPE} is not a supported build type.")
        message("Supported build types are:")
        foreach(type ${SUPPORTED_BUILD_TYPES})
            message("- ${type}")
        endforeach(type ${SUPPORTED_BUILD_TYPES})
        message(FATAL_ERROR "The configuration script will now exit.")
    endif(NOT CMAKE_BUILD_TYPE IN_LIST SUPPORTED_BUILD_TYPES)
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR} CACHE INTERNAL "")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR} CACHE INTERNAL "")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR} CACHE INTERNAL "")

message("\nPROJECT : ${CMAKE_PROJECT_NAME}")
message("BUILD_TARGET : ${CMAKE_SYSTEM_NAME}")
message("PROCESSOR : ${CMAKE_SYSTEM_PROCESSOR}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message("\n")
execute_process(
    COMMAND "git" "submodule" "update" "--remote" "--recursive" "--force"
    RESULT_VARIABLE GIT_SUBMODULE_UPDATE_FAILED
    OUTPUT_VARIABLE GIT_SUBMODULE_UPDATE_OUTPUT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ECHO_OUTPUT_VARIABLE
    )

execute_process(
    COMMAND "sync"
)


if(BUILD_TESTING)
    enable_testing()
    include(CTest)
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test)
        add_subdirectory(test)
    endif(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test)
else()
    if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
        enable_testing()
        include(CTest)
        if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test)
            add_subdirectory(test)
        endif(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test)
    endif()
endif()

include_directories(share)
add_subdirectory(injection_api)

add_subdirectory(JTOK)
add_subdirectory(bufferlib)
add_subdirectory(drivers)
add_subdirectory(core)



# STATIC ANALYSIS
if(ANALYZE)

    function(get_all_targets var)
        set(targets)
        get_all_targets_recursive(targets ${CMAKE_CURRENT_SOURCE_DIR})
        set(${var} ${targets} PARENT_SCOPE)
    endfunction()

    macro(get_all_targets_recursive targets dir)
        get_property(subdirectories DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
        foreach(subdir ${subdirectories})
            get_all_targets_recursive(${targets} ${subdir})
        endforeach()

        get_property(current_targets DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)
        list(APPEND ${targets} ${current_targets})
    endmacro()

    get_all_targets(${CMAKE_PROJECT_NAME}_target_list)
    message("targets for ${CMAKE_PROJECT_NAME} : ${${CMAKE_PROJECT_NAME}_target_list}")

    foreach(target ${${CMAKE_PROJECT_NAME}_target_list})
        get_target_property(type ${target} TYPE)

        if(type STREQUAL STATIC_LIBRARY)
        elseif(type STREQUAL MODULE_LIBRARY)
        elseif(type STREQUAL SHARED_LIBRARY)
        elseif(type STREQUAL OBJECT_LIBRARY)
        elseif(type STREQUAL EXECUTABLE)
        else()
            # if the target doesn't have code associated with it, remove it from
            # the static analysis dependency list
            list(REMOVE_ITEM ${CMAKE_PROJECT_NAME}_target_list ${target})
        endif()
    endforeach(target ${${CMAKE_PROJECT_NAME}_target_list})

    find_program(CPPCHECK_BIN NAMES cppcheck)
    if(CPPCHECK_BIN)
        execute_process(COMMAND ${CPPCHECK_BIN} --version
            OUTPUT_VARIABLE CPPCHECK_VERSION
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
 
        set(CPPCHECK_THREADS_ARG "-j4" CACHE STRING "The number of threads to use")
        if(NOT CMAKE_EXPORT_COMPILE_COMMANDS)
            set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # can't use cppcheck without compile commands
        endif(NOT CMAKE_EXPORT_COMPILE_COMMANDS)
        set(CPPCHECK_PROJECT_ARG "--project=${PROJECT_BINARY_DIR}/compile_commands.json")
        set(CPPCHECK_BUILD_DIR_ARG "--cppcheck-build-dir=${PROJECT_BINARY_DIR}/analysis/cppcheck" CACHE STRING "The build directory to use")

        # Don't show thise errors
        if(EXISTS "${CMAKE_SOURCE_DIR}/.cppcheck_suppressions")
            set(CPPCHECK_SUPPRESSIONS "--suppressions-list=${CMAKE_SOURCE_DIR}/.cppcheck_suppressions" CACHE STRING "The suppressions file to use")
        else()
            set(CPPCHECK_SUPPRESSIONS "" CACHE STRING "The suppressions file to use")
        endif()
    
        # Show these errors but don't fail the build
        # These are mainly going to be from the "warning" category that is enabled by default later
        if(EXISTS "${CMAKE_SOURCE_DIR}/.cppcheck_exitcode_suppressions")
            set(CPPCHECK_EXITCODE_SUPPRESSIONS "--exitcode-suppressions=${CMAKE_SOURCE_DIR}/.cppcheck_exitcode_suppressions" CACHE STRING "The exitcode suppressions file to use")
        else()
            set(CPPCHECK_EXITCODE_SUPPRESSIONS "" CACHE STRING "The exitcode suppressions file to use")
        endif()
    
        set(CPPCHECK_ERROR_EXITCODE_ARG "--error-exitcode=1" CACHE STRING "The exitcode to use if an error is found")
        set(CPPCHECK_CHECKS_ARGS "--enable=warning" CACHE STRING "Arguments for the checks to run")
        set(CPPCHECK_OTHER_ARGS "" CACHE STRING "Other arguments")
        set(_CPPCHECK_EXCLUDES)
    
        ## set exclude files and folders
        foreach(ex ${CPPCHECK_EXCLUDES})
            list(APPEND _CPPCHECK_EXCLUDES "-i${ex}")
        endforeach(ex)
    
        set(CPPCHECK_ALL_ARGS 
            ${CPPCHECK_THREADS_ARG} 
            ${CPPCHECK_PROJECT_ARG} 
            ${CPPCHECK_BUILD_DIR_ARG} 
            ${CPPCHECK_ERROR_EXITCODE_ARG} 
            ${CPPCHECK_SUPPRESSIONS} 
            ${CPPCHECK_EXITCODE_SUPPRESSIONS}
            ${CPPCHECK_CHECKS_ARGS} 
            ${CPPCHECK_OTHER_ARGS}
            ${_CPPCHECK_EXCLUDES}
        )
    
        # run cppcheck command with optional xml output for CI system
        if(NOT CPPCHECK_XML_OUTPUT)
            set(CPPCHECK_COMMAND 
                ${CPPCHECK_BIN}
                ${CPPCHECK_ALL_ARGS}
            )
        else()
            set(CPPCHECK_COMMAND
                ${CPPCHECK_BIN}
                ${CPPCHECK_ALL_ARGS}
                --xml 
                --xml-version=2
                2> ${CPPCHECK_XML_OUTPUT})
        endif()
    else()
        message(FATAL_ERROR "Could not find cppcheck executable. Either it is not installed or not added to PATH. If you are on a debian-based linux distribution, you can try installing the cppcheck and clang-tools packages with your package manager.")
    endif(CPPCHECK_BIN)
    
    # handle the QUIETLY and REQUIRED arguments 
    # and set YAMLCPP_FOUND to TRUE if all listed variables are TRUE
    include(FindPackageHandleStandardArgs)
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(
        CPPCHECK 
        DEFAULT_MSG 
        CPPCHECK_BIN
    )
    
    # prevent user from tampering with internal vars
    mark_as_advanced(
        CPPCHECK_BIN  
        CPPCHECK_THREADS_ARG
        CPPCHECK_PROJECT_ARG
        CPPCHECK_BUILD_DIR_ARG
        CPPCHECK_ERROR_EXITCODE_ARG
        CPPCHECK_SUPPRESSIONS
        CPPCHECK_EXITCODE_SUPPRESSIONS
        CPPCHECK_CHECKS_ARGS
        CPPCHECK_EXCLUDES
        CPPCHECK_OTHER_ARGS
    )
    
    # If found add a cppcheck-analysis target
    if(CPPCHECK_FOUND)
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/analysis/cppcheck)
        add_custom_target(cppcheck-analysis ALL DEPENDS ${${CMAKE_PROJECT_NAME}_target_list})
        add_custom_command(
            TARGET cppcheck-analysis
            POST_BUILD
            DEPENDS ${${CMAKE_PROJECT_NAME}_target_list}
            COMMENT "Performing static analysis with cppcheck"
            COMMAND ${CPPCHECK_COMMAND}
        )
        message("cppcheck found. Use cppccheck-analysis targets to run it")
    else()
        message("cppcheck not found. No cppccheck-analysis targets")
    endif()
endif(ANALYZE)


