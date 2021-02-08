cmake_minimum_required(VERSION 3.18)


option(BUILD_TESTING "[ON/OFF] Build tests in addition to library." OFF)
option(BUILD_EXAMPLES "[ON/OFF] Build examples in addition to library." OFF)
option(VERBOSE "[ON/OFF] Emit verbose messages during configuration" OFF)
option(ANALYZE "[ON/OFF] Perform static analysis on project" OFF)


project(
    ADCS_JSONS 
    VERSION 1.0 
    DESCRIPTION "ADCS <-> OBC JSON COMMUNICATION LIBRARY FOR LORRIS PROJECT"
    LANGUAGES C ASM
)
set(CURRENT_TARGET ${PROJECT_NAME})
message("Configuring target : ${PROJECT_NAME}")


if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    set(BUILD_TESTING ON FORCE)
    set(BUILD_EXAMPLES ON FORCE)
    set(ANALYZE ON FORCE)

    set(${CMAKE_PROJECT_NAME}_SUPPORTED_BUILD_TYPES "")
    list(APPEND ${CMAKE_PROJECT_NAME}_SUPPORTED_BUILD_TYPES "Debug")
    list(APPEND ${CMAKE_PROJECT_NAME}_SUPPORTED_BUILD_TYPES "Release")
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${${CMAKE_PROJECT_NAME}_SUPPORTED_BUILD_TYPES})
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type chosen by the user at configure time" FORCE)
    elseif(NOT CMAKE_BUILD_TYPE IN_LIST ${CMAKE_PROJECT_NAME}_SUPPORTED_BUILD_TYPES)
        message("Build type : ${CMAKE_BUILD_TYPE} is not a supported build type.")
        message("Supported build types are:")
        foreach(type ${${CMAKE_PROJECT_NAME}_SUPPORTED_BUILD_TYPES})
            message("- ${type}")
        endforeach(type ${${CMAKE_PROJECT_NAME}_SUPPORTED_BUILD_TYPES})
        message(FATAL_ERROR "The configuration script will now exit.")
    endif()


    if(WIN32)
        set(CMAKE_EXECUTABLE_SUFFIX ".exe")
    elseif(UNIX AND NOT APPLE)
        set(CMAKE_EXECUTABLE_SUFFIX ".out")
    elseif(APPLE)
        set(CMAKE_EXECUTABLE_SUFFIX ".out")
    else()
        message(FATAL_ERROR "[ERROR] platform: ${CMAKE_HOST_SYSTEM_NAME} not supported")
    endif()

    foreach(btype ${${CMAKE_PROJECT_NAME}_SUPPORTED_BUILD_TYPES})
        string(TOUPPER ${btype} type)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${type} "${PROJECT_BINARY_DIR}/${btype}/bin" 
            CACHE INTERNAL "Output directory for executables built in \"${btype}\" mode"
        )

        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${type} "${PROJECT_BINARY_DIR}/${btype}/lib" 
            CACHE INTERNAL "Output directory for archived binaries built in \"${btype}\" mode"
        )

        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${type} "${PROJECT_BINARY_DIR}/${btype}/lib" 
            CACHE INTERNAL "Output directory for libraries built in \"${btype}\" mode"
        )
    endforeach(btype ${${CMAKE_PROJECT_NAME}_SUPPORTED_BUILD_TYPES})

    execute_process(
        COMMAND "git" "submodule" "update" "--remote" "--recursive"
        RESULT_VARIABLE GIT_SUBMODULE_UPDATE_FAILED
        OUTPUT_VARIABLE GIT_SUBMODULE_UPDATE_OUTPUT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ECHO_OUTPUT_VARIABLE
    )
    execute_process(COMMAND "sync")

endif(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

if(TARGET ${CURRENT_TARGET})
    get_target_property(type ${CURRENT_TARGET} TYPE)
    message(FATAL_ERROR "Target ${CURRENT_TARGET} already exists with type ${type}")
else()
    add_library(${CURRENT_TARGET})
endif(TARGET ${CURRENT_TARGET})


# PRIVATE SOURCES
get_property(${CURRENT_TARGET}_languages GLOBAL PROPERTY ENABLED_LANGUAGES)
foreach(LANG ${${CURRENT_TARGET}_languages})
    foreach(EXTENSION ${CMAKE_${LANG}_SOURCE_FILE_EXTENSIONS})
        set(GLOBBING_EXPR "${CMAKE_CURRENT_SOURCE_DIR}/src/*.${EXTENSION}")
        file(GLOB_RECURSE ${LANG}_${EXTENSION}_sources "${GLOBBING_EXPR}")
        list(APPEND ${CURRENT_TARGET}_${LANG}_sources ${${LANG}_${EXTENSION}_sources})
    endforeach(EXTENSION ${CMAKE_${LANG}_SOURCE_FILE_EXTENSIONS})
    list(REMOVE_DUPLICATES ${CURRENT_TARGET}_${LANG}_sources)
    target_sources(${CURRENT_TARGET} PRIVATE ${${CURRENT_TARGET}_${LANG}_sources})
endforeach(LANG ${${CURRENT_TARGET}_languages})


# PRIVATE HEADERS
foreach(LANG ${${CURRENT_TARGET}_languages})
    if(${LANG} STREQUAL CXX)
        file(GLOB_RECURSE ${CURRENT_TARGET}_${LANG}_private_hdrs 
            "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
        )
    elseif(${LANG} STREQUAL C)
        file(GLOB_RECURSE ${CURRENT_TARGET}_${LANG}_private_hdrs 
            "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
        )
    elseif(${LANG} STREQUAL ASM)
    
    else()
        # language not supported
    endif()

    set(${CURRENT_TARGET}_${LANG}_private_inc_dirs "")
    foreach(hdr ${${CURRENT_TARGET}_${LANG}_private_hdrs})
        get_filename_component(hdr_dir ${hdr} DIRECTORY)
        list(APPEND ${CURRENT_TARGET}_${LANG}_private_inc_dirs ${hdr_dir})
    endforeach(hdr ${${CURRENT_TARGET}_${LANG}_private_hdrs})
    list(REMOVE_DUPLICATES ${CURRENT_TARGET}_${LANG}_private_inc_dirs)
    target_include_directories(${CURRENT_TARGET} PRIVATE ${${CURRENT_TARGET}_${LANG}_private_inc_dirs})
endforeach(LANG ${${CURRENT_TARGET}_languages})


# PUBLIC HEADERS
foreach(LANG ${${CURRENT_TARGET}_languages})
    if(LANG STREQUAL CXX)
        file(GLOB_RECURSE ${CURRENT_TARGET}_${LANG}_public_hdrs 
            "${CMAKE_CURRENT_SOURCE_DIR}/inc/*.hpp"
        )
    elseif(LANG STREQUAL C)
        file(GLOB_RECURSE ${CURRENT_TARGET}_${LANG}_public_hdrs 
            "${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h"
        )
    elseif(LANG STREQUAL ASM)
    
    else()
        # language not supported
    endif()

    set(${CURRENT_TARGET}_${LANG}_public_inc_dirs "")
    foreach(hdr ${${CURRENT_TARGET}_${LANG}_public_hdrs})
        get_filename_component(hdr_dir ${hdr} DIRECTORY)
        list(APPEND ${CURRENT_TARGET}_${LANG}_public_inc_dirs ${hdr_dir})
    endforeach(hdr ${${CURRENT_TARGET}_${LANG}_public_hdrs})
    list(REMOVE_DUPLICATES ${CURRENT_TARGET}_${LANG}_public_inc_dirs)
    target_include_directories(${CURRENT_TARGET} PUBLIC ${${CURRENT_TARGET}_${LANG}_public_inc_dirs})
endforeach(LANG ${${CURRENT_TARGET}_languages})





# CONFIGURE TESTS
if(BUILD_TESTING)
    enable_testing()
    include(CTest)
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests")
        add_subdirectory(tests)
    elseif(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/test")
        add_subdirectory(test)
    endif()
endif(BUILD_TESTING)

# CONFIGURE EXAMPLES
if(BUILD_EXAMPLES)
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/examples")
        add_subdirectory(examples)
    elseif(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/example")
        add_subdirectory(example)
    endif()
endif(BUILD_EXAMPLES)


# STATIC ANALYSIS
if(ANALYZE AND PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME)

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

    find_program(CPPCHECK_BIN NAMES cppcheck REQUIRED)
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
        if(NOT TARGET cppcheck-analysis)
            if(NOT IS_DIRECTORY "${CMAKE_BINARY_DIR}/analysis")
                file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/analysis")
            endif(NOT IS_DIRECTORY "${CMAKE_BINARY_DIR}/analysis")

            if(NOT IS_DIRECTORY "${CMAKE_BINARY_DIR}/analysis/cppcheck")
                file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/analysis/cppcheck")
            endif(NOT IS_DIRECTORY "${CMAKE_BINARY_DIR}/analysis/cppcheck")

            if(NOT IS_DIRECTORY "${CMAKE_BINARY_DIR}/analysis/cppcheck")
                file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/analysis/cppcheck")
            endif(NOT IS_DIRECTORY "${CMAKE_BINARY_DIR}/analysis/cppcheck")
            
            add_custom_target(cppcheck-analysis ALL DEPENDS ${${CMAKE_PROJECT_NAME}_target_list})
            add_custom_command(
                TARGET cppcheck-analysis
                POST_BUILD
                DEPENDS ${${CMAKE_PROJECT_NAME}_target_list}
                COMMENT "Performing static analysis with cppcheck"
                COMMAND ${CPPCHECK_COMMAND}
            )
        endif(NOT TARGET cppcheck-analysis)
    else()
        message("cppcheck not found. No cppccheck-analysis targets")
    endif()
endif(ANALYZE AND PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME)


################################################################################
#   USER CONFIGURATION STARTS HERE
################################################################################
target_link_libraries(${CURRENT_TARGET} PRIVATE JTOK)
target_link_libraries(${CURRENT_TARGET} PRIVATE ADCS_OBC_INTERFACE)
target_link_libraries(${CURRENT_TARGET} PRIVATE ADCS_REACTIONWHEELS)
target_link_libraries(${CURRENT_TARGET} PRIVATE ADCS_MAGNETORQUERS)
target_link_libraries(${CURRENT_TARGET} PRIVATE ADCS_SUN_SENSORS)
