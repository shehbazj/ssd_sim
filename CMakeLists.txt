cmake_minimum_required(VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -pg)
 
# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Locate Boost
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost COMPONENTS thread) 

# Builds SSD Emulator
add_executable(emulator page_md.cpp page_group.cpp coder.cpp block.cpp ssd.cpp main.cpp)
 
add_executable(main_gc page_md.cpp page_group.cpp coder.cpp block.cpp ssd.cpp main_gc.cpp)

add_executable(main_visualize page_md.cpp page_group.cpp coder.cpp block.cpp ssd.cpp main_visualize.cpp)

add_executable(tests test.cpp)

add_executable(parallel_tests parallel_test.cpp)

# Link runTests with what we want to test and the GTest and pthread library
target_link_libraries(tests ${GTEST_LIBRARIES} pthread)
target_link_libraries(parallel_tests ${GTEST_LIBRARIES} pthread)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
    add_executable(main_parallel page_md.cpp page_group.cpp coder.cpp block.cpp ssd.cpp main_parallel.cpp)
    target_link_libraries(main_parallel ${Boost_LIBRARIES})
endif()