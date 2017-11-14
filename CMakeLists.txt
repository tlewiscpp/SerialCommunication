cmake_minimum_required(VERSION 3.1)
set(PROJECT_NAME SerialCommunication)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 11)

set (SOURCE_ROOT SerialCommunication)

set(${PROJECT_NAME}_SOURCE_FILES
        ${SOURCE_ROOT}/Main.cpp
        ${SOURCE_ROOT}/ApplicationUtilities.cpp
        ${SOURCE_ROOT}/MessageLogger.cpp)

set (${PROJECT_NAME}_HEADER_FILES
        ${SOURCE_ROOT}/ApplicationUtilities.h
        ${SOURCE_ROOT}/MessageLogger.h
        ${SOURCE_ROOT}/GlobalDefinitions.h)

add_executable(${PROJECT_NAME}
        ${${PROJECT_NAME}_SOURCE_FILES}
        ${${PROJECT_NAME}_HEADER_FILES})

target_link_libraries(${PROJECT_NAME}
        CppSerialPort
        ncurses)