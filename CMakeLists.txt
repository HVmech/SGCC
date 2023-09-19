
# CMake version requirements
cmake_minimum_required(VERSION 3.1.5)

# sgcc project
project(sgcc
	VERSION 0.3
	LANGUAGES CXX
)

# select static MSVC runtime (really has effect?!)
if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# include FetchContent
include(FetchContent)

# declaring nana library
# branch 'master' is unavailable for Win32
if(MSVC)
    FetchContent_Declare(
        nana
        GIT_REPOSITORY https://github.com/cnjinhao/nana.git
        GIT_TAG        develop-1.8
    )
else()
    FetchContent_Declare(
        nana
        GIT_REPOSITORY https://github.com/cnjinhao/nana.git
        GIT_TAG        master
    )
endif()

# declaring xlnt library
FetchContent_Declare(
    xlnt
    GIT_REPOSITORY https://github.com/tfussell/xlnt.git
    GIT_TAG master
)

# make libraries available
FetchContent_MakeAvailable(nana)
FetchContent_MakeAvailable(xlnt)

# add executable
add_executable(sgcc
	main.cpp
    src/info.cpp
    src/shape_constructor.cpp
    src/point.cpp
    src/axis.cpp
    src/shapes.cpp
)

# include
target_include_directories(sgcc PRIVATE include)

target_link_libraries(sgcc
	nana
    xlnt
)

# for xlnt static linkage
set(XLNT_STATIC = 1)

# Some usefull compiler flags
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
    CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(sgcc PRIVATE 
        -g
        -Wall
    )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(sgcc PRIVATE
        /EHsc
        /W2
    )
endif()

# Force both release and debug CXX flags to prevent dynamic linkage of MSVC runtime
if (MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "/W2 /EHsc" CACHE STRING "Debug compiler flags" FORCE)
    set(CMAKE_CXX_FLAGS_RELEASE "/W2 /EHsc" CACHE STRING "Release compiler flags" FORCE)
endif()

# target properties
set_target_properties(sgcc PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)