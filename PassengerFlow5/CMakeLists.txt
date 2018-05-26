cmake_minimum_required(VERSION 3.5)

project(passenger_flow_lib)

# Build options
option(BUILD_EXAMPLES  "Set to ON to build examples"  ON)

# Use C++11
#set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
message(STATUS "C++11 support has been enabled by default.")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1")

include_directories(include)

set(src_files 
    src/BGDifference.cpp
        
    )

add_library(passenger_flow_lib SHARED ${src_files})
set(fa_required_libs passenger_flow_lib)

if (BUILD_EXAMPLES)
    message(STATUS "Build with examples.")
    find_package(OpenCV)
    if (NOT OpenCV_FOUND)
        message(WARNING "OpenCV not found. Test will not be built.")
    else()
        include_directories(${OpenCV_INCLUDE_DIRS} build)
	link_directories(build)
        list(APPEND fa_required_libs ${OpenCV_LIBS} passenger_flow_lib)
        add_executable(pa_flow src/test/main.cpp)
        target_link_libraries(pa_flow ${fa_required_libs})
    endif()
endif()
