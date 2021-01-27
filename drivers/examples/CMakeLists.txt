
cmake_minimum_required(VERSION 3.18)

# preserve top level project output directory config
if(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(RUNTIME_OUTPUT_DIRECTORY_VAR "CMAKE_RUNTIME_OUTPUT_DIRECTORY")
    set(BACKUP_CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
elseif(${CMAKE_PROJECT_NAME}_SUPPORTED_BUILD_TYPES)
    if(CMAKE_BUILD_TYPE)
        foreach(type ${${CMAKE_PROJECT_NAME}_SUPPORTED_BUILD_TYPES})
            if(type STREQUAL CMAKE_BUILD_TYPE)
                string(TOUPPER ${type} build_type_upper)
                if(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${build_type_upper})
                    set(BACKUP_CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_${build_type_upper}})
                    set(RUNTIME_OUTPUT_DIRECTORY_VAR "CMAKE_RUNTIME_OUTPUT_DIRECTORY_${build_type_upper}")
                endif(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${build_type_upper})
            endif(type STREQUAL CMAKE_BUILD_TYPE)
        endforeach(type ${${${CMAKE_PROJECT_NAME}_SUPPORTED_BUILD_TYPES}})
    endif(CMAKE_BUILD_TYPE)
endif()

if(BACKUP_CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(${RUNTIME_OUTPUT_DIRECTORY_VAR} ${CMAKE_CURRENT_BINARY_DIR})
endif(BACKUP_CMAKE_RUNTIME_OUTPUT_DIRECTORY)

if(NOT ${CURRENT_TARGET}_languages)
	get_property(${CURRENT_TARGET}_languages GLOBAL PROPERTY ENABLED_LANGUAGES)
	set(${CURRENT_TARGET}_languages ${${CURRENT_TARGET}_languages} PARENT_SCOPE)
endif(NOT ${CURRENT_TARGET}_languages)

foreach(LANG ${${CURRENT_TARGET}_languages})
    foreach(EXTENSION ${CMAKE_${LANG}_SOURCE_FILE_EXTENSIONS})
        set(GLOBBING_EXPR "${CMAKE_CURRENT_SOURCE_DIR}/*.example.${EXTENSION}")
        file(GLOB_RECURSE ${LANG}_${EXTENSION}_examples "${GLOBBING_EXPR}")
        list(APPEND ${CURRENT_TARGET}_${LANG}_examples ${${LANG}_${EXTENSION}_examples})
        foreach(example ${${LANG}_${EXTENSION}_examples})
            get_filename_component(example_suffix ${example} NAME_WLE)
            set(example_target "${CURRENT_TARGET}_${example_suffix}")
            if(NOT TARGET ${example_target})
                add_executable(${example_target})
                target_sources(${example_target} PRIVATE ${example})


                get_target_property(PROJ_TYPE ${CURRENT_TARGET} TYPE)
                string(REGEX MATCH ".*_LIBRARY" PROJ_IS_LIBRARY ${PROJ_TYPE})
                if(PROJ_IS_LIBRARY)
                    target_link_libraries(${example_target} PRIVATE ${CURRENT_TARGET})

                elseif(PROJ_TYPE STREQUAL EXECUTABLE)

                    #[[
                    # What we'd like to do is take the exact link dependencies
                    # that the top level exectuable requires and link
                    # the example against them. Sadly that doesn't seem
                    # to be possible with the current iteration of cmake (3.19)
                    get_target_property(PROJ_LIBS ${CURRENT_TARGET} LINK_LIBRARIES)
                    message("PROJ_LIBS = ${PROJ_LIBS}")

                    get_target_property(INTER_PROJ_LIBS ${CURRENT_TARGET} INTERFACE_LINK_LIBRARIES)
                    message("INTER_PROJ_LIBS ${INTER_PROJ_LIBS}")

                    if(PROJ_LIBS)
                        foreach(lib ${PROJB_LIBS})
                            target_link_libraries(${example_target} PRIVATE ${lib})
                        endforeach(lib ${PROJB_LIBS})
                    else()
                        message("${CURRENT_TARGET} has no link libraries to pass on to ${example_target}")
                    endif(PROJ_LIBS)
                    #]]
                endif() # END PROJECT TYPE 
                
            endif(NOT TARGET ${example_target})
        endforeach(example ${${LANG}_${EXTENSION}_examples})
    endforeach(EXTENSION ${CMAKE_${LANG}_SOURCE_FILE_EXTENSIONS})
endforeach(LANG ${${CURRENT_TARGET}_languages})

# restore output configuration
if(BACKUP_CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    if(${RUNTIME_OUTPUT_DIRECTORY_VAR})
        set(${RUNTIME_OUTPUT_DIRECTORY_VAR} ${BACKUP_CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    endif(${RUNTIME_OUTPUT_DIRECTORY_VAR})
endif(BACKUP_CMAKE_RUNTIME_OUTPUT_DIRECTORY)

