cmake_minimum_required(VERSION 3.12)
project(tsumaki CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options("/W3" "$<$<CONFIG:RELEASE>:/O2>")
else()
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -O3")
endif()


find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()


find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIR})



if(MSVC)
	set(tsumaki_PLATFORM_DEPS w32-pthreads wsock32 ws2_32)
endif()

add_compile_definitions(USE_AVX2)
if (MSVC)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2")
else()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2")
endif()

set(tsumaki_SOURCES
    obs-filter.cpp
	tsumaki-filter.cpp
    tsumaki-api-thread.cpp
	tsumaki-plugin.cpp
    tsumaki-math.cpp
    frame.cpp
    image-cvt.cpp
    ipc.cpp
    ipc-connection.cpp
    ipc-frame.cpp
    ipc-spec.cpp)

if(WIN32)
    set(tsumaki_PLATFORM_SPECIFIC_SOURCES
        ipc_platforms/win32.cpp)
else()
    set(tsumaki_PLATFORM_SPECIFIC_SOURCES
        ipc_platforms/unix.cpp)
endif()

set(tsumaki_PROTOBUF_SOURCES
    protobuf/DetectPerson.pb.cc
    protobuf/ErrorResponse.pb.cc
    protobuf/SetupDevice.pb.cc
    protobuf/Heartbeat.pb.cc
    protobuf/MaskImage.pb.cc
    protobuf/RGBAImage.pb.cc)

set(tsumaki_DEP_SOURCES
    deps/fast-slic/fast-slic.cpp
    deps/fast-slic/fast-slic-avx2.cpp
    deps/fast-slic/simple-crf.cpp)


add_library(tsumaki MODULE
	${tsumaki_SOURCES}
    ${tsumaki_PLATFORM_SPECIFIC_SOURCES}
    ${tsumaki_PROTOBUF_SOURCES}
    ${tsumaki_DEP_SOURCES}
)
target_link_libraries(tsumaki
	libobs
	${tsumaki_PLATFORM_DEPS}
    ${Protobuf_LIBRARY})


install_obs_plugin_with_data(tsumaki data)
