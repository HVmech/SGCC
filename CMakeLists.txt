# version 3.12 or later of CMake or needed later for installing nana
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(myawesomewonderfullproject
	VERSION 0.1
	LANGUAGES CXX
)

include(FetchContent)
    FetchContent_Declare(
        nana
        GIT_REPOSITORY https://github.com/cnjinhao/nana.git
        GIT_TAG develop-1.8
)

include(FetchContent)
    FetchContent_Declare(
        xlnt
        GIT_REPOSITORY https://github.com/tfussell/xlnt.git
        GIT_TAG master
)

set (XLNT_STATIC = 1)

FetchContent_MakeAvailable(nana)
FetchContent_MakeAvailable(xlnt)

if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

add_executable(geom-test
	main.cpp
)

target_link_libraries(geom-test
	nana
    xlnt
)

if (MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG
  #[["/DWIN32 /D_WINDOWS /EHsc /WX /wd4355 /wd4251 /wd4250 /wd4996"]]
    CACHE STRING "Debug compiler flags" FORCE
    )
endif()

set_target_properties(geom-test PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)