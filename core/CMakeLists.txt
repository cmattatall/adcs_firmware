cmake_minimum_required(VERSION 3.16)
project(adcs LANGUAGES C ASM)
set(BUILD_TARGET ${PROJECT_NAME})

ADCS_add_executable(${BUILD_TARGET})


#headers and sources
file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE headers "${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h")

foreach(hdr ${headers})
    get_filename_component(hdr_dir ${hdr} DIRECTORY)
    list(APPEND include_dirs ${hdr_dir})
endforeach(hdr ${headers})
list(REMOVE_DUPLICATES include_dirs)

target_sources(${BUILD_TARGET} PRIVATE "${sources}")
target_include_directories(${BUILD_TARGET} PUBLIC "${include_dirs}")

target_link_libraries(${BUILD_TARGET} PUBLIC JTOK)
target_link_libraries(${BUILD_TARGET} PUBLIC INJECTION_API)


if(CMAKE_CROSSCOMPILING)
    target_link_libraries(${BUILD_TARGET} PUBLIC ADCS_drivers)
else()
    add_subdirectory(emulated)
    target_link_libraries(${BUILD_TARGET} PUBLIC ADCS_emulated_interfaces)
endif(CMAKE_CROSSCOMPILING)

