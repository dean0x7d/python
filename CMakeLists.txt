cmake_minimum_required(VERSION 3.0)
project(boost_python)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
find_package(PythonLibsNew ${PYTHON_VERSION} REQUIRED)

file(GLOB_RECURSE srcs FOLLOW_SYMLINKS include/*.hpp src/*.cpp)

add_library(boost_python ${srcs})
target_include_directories(boost_python SYSTEM
    PUBLIC ${PYTHON_INCLUDE_DIRS}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
    PUBLIC ${BOOST_ROOT}
)

target_compile_options(boost_python PUBLIC -std=c++11)
target_compile_definitions(boost_python PUBLIC BOOST_PYTHON_STATIC_LIB)
if(WIN32)
    target_compile_definitions(boost_python PUBLIC HAVE_ROUND)
else()
    target_compile_options(boost_python PUBLIC -fPIC)
endif()

# used to build boost_python modules
function(boost_python_add_module TARGET)
    python_add_module(${TARGET} ${ARGN})
    target_link_libraries(${TARGET} boost_python)
endfunction()
