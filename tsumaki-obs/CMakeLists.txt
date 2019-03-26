cmake_minimum_required(VERSION 3.12)
project(tsumaki)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
	set(tsumaki_PLATFORM_DEPS
		w32-pthreads)
endif()

set(tsumaki_SOURCES
	tsumaki-filter.cpp
	tsumaki-plugin.cpp)

add_library(tsumaki MODULE
	${tsumaki_SOURCES})
target_link_libraries(tsumaki
	libobs
	${tsumaki_PLATFORM_DEPS})

install_obs_plugin_with_data(tsumaki data)
