cmake_minimum_required(VERSION 2.8)

project(land-robot LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
land-robot
${CMAKE_SOURCE_DIR}/src/drivers/motors.h
${CMAKE_SOURCE_DIR}/src/drivers/mpu9265.h

${CMAKE_SOURCE_DIR}/src/core/control.h
${CMAKE_SOURCE_DIR}/src/core/pid.h
${CMAKE_SOURCE_DIR}/src/core/comm.h

${CMAKE_SOURCE_DIR}/src/main/app.h


${CMAKE_SOURCE_DIR}/src/drivers/motors.cpp
${CMAKE_SOURCE_DIR}/src/drivers/mpu9265.cpp

${CMAKE_SOURCE_DIR}/src/core/control.cpp
${CMAKE_SOURCE_DIR}/src/core/pid.cpp
${CMAKE_SOURCE_DIR}/src/core/comm.cpp

${CMAKE_SOURCE_DIR}/src/main/app.cpp
)

find_library(WIRINGPI_LIBRARIES NAMES wiringPi)
target_link_libraries(land-robot ${WIRINGPI_LIBRARIES})

target_link_libraries(land-robot ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(land-robot pthread)
target_include_directories(land-robot PRIVATE ${CMAKE_SOURCE_DIR}/src/drivers)
target_include_directories(land-robot PRIVATE ${CMAKE_SOURCE_DIR}/src/core)
target_include_directories(land-robot PRIVATE ${CMAKE_SOURCE_DIR}/src/main)
