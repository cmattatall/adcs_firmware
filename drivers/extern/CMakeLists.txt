cmake_minimum_required(VERSION 3.18)

add_library(EXTERN_DRIVERS STATIC)

add_library(BNO055_DRIVER STATIC)

target_sources(BNO055_DRIVER PUBLIC BNO055_driver/bno055_support.c)
target_sources(BNO055_DRIVER PUBLIC BNO055_driver/bno055.c)
target_include_directories(BNO055_DRIVER PUBLIC BNO055_driver)

target_link_libraries(EXTERN_DRIVERS PUBLIC BNO055_DRIVER)

