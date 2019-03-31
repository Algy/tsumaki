cmake_minimum_required(VERSION 3.12)
project(tsumaki CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options("/W3" "$<$<CONFIG:RELEASE>:/O2>")
else()
    add_compile_options("-Wall" "$<$<CONFIG:RELEASE>:-O3>")
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -O3")

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()


if(MSVC)
	set(tsumaki_PLATFORM_DEPS
		w32-pthreads)
endif()

set(tsumaki_SOURCES
	tsumaki-filter.cpp
	tsumaki-plugin.cpp
    tsumaki-math.cpp
    image-cvt.cpp)

add_library(tsumaki MODULE
	${tsumaki_SOURCES})
target_link_libraries(tsumaki
	libobs
	${tsumaki_PLATFORM_DEPS})


install_obs_plugin_with_data(tsumaki data)
