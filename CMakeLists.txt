cmake_minimum_required(VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
 
# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Builds SSD Emulator
add_executable(emulator page_md.cpp page_group.cpp coder.cpp block.cpp ssd.cpp main.cpp)
 
add_executable(main_gc page_md.cpp page_group.cpp coder.cpp block.cpp ssd.cpp main_gc.cpp)

add_executable(tests test.cpp)

# Link runTests with what we want to test and the GTest and pthread library
target_link_libraries(tests ${GTEST_LIBRARIES} pthread)

