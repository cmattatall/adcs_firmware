cmake_minimum_required(VERSION 3.16)


project(adcs_firmware LANGUAGES C ASM)

set(BUILD_TARGET ${CMAKE_PROJECT_NAME})
add_executable(${BUILD_TARGET} "test_compile.c")