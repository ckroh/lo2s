cmake_minimum_required(VERSION 3.5)
project(lo2s)

include(CMakeDependentOption)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

include(cmake/DefaultBuildType.cmake)

# Intialize git submodules if not done already
file(GLOB SUBMODULE_FILES "lib/otf2xx/*")
list(LENGTH SUBMODULE_FILES COUNT_OTF2XX)
file(GLOB SUBMODULE_FILES "lib/ins_dec/*")
list(LENGTH SUBMODULE_FILES COUNT_InsDec)
file(GLOB SUBMODULE_FILES "lib/nitro/*")
list(LENGTH SUBMODULE_FILES COUNT_NITRO)
if(${COUNT_OTF2XX} EQUAL 0 OR ${COUNT_NITRO} EQUAL 0)
    message(STATUS "Initializing git submodule")
    execute_process(COMMAND "git" "submodule" "init" WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
    execute_process(COMMAND "git" "submodule" "update" WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif()

include(lib/otf2xx/OTF2XX.cmake)
include(lib/ins_dec/InsDec.cmake)

include(lib/nitro/Nitro.cmake)

include(lib/x86_adapt/x86_adapt.cmake)

if (X86Adapt_FOUND)
    set(X86_ADAPT_SOURCE_FILES src/metric/x86_adapt/metrics.cpp src/metric/x86_adapt/monitor.cpp)
else()
    set(X86_ADAPT_SOURCE_FILES "")
endif()

find_package(Boost REQUIRED COMPONENTS system program_options filesystem)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

find_package(Binutils REQUIRED)
include_directories(SYSTEM ${Binutils_INCLUDE_DIRS})

# TODO Shouldn't this be part of libc and handlded more elegantly?
find_package(Dl REQUIRED)
include_directories(SYSTEM ${Dl_INCLUDE_DIRS})

find_package(Radare)

CMAKE_DEPENDENT_OPTION(USE_RADARE "Enable Radare support." ON "Radare_FOUND" OFF)

if (USE_RADARE)
    if (Radare_FOUND)
        include_directories(SYSTEM ${Radare_INCLUDE_DIRS})
        set(RADARE_SOURCE_FILES src/radare.cpp)
#        add_definitions(-DHAVE_RADARE)
    else()
        message(SEND_ERROR "Radare not found but requested.")
        set(RADARE_SOURCE_FILES "")
    endif()
else()
    set(RADARE_SOURCE_FILES "")
endif()
set(InsDec_LIBRARIES "/home/tinker/Arbeit/SHK/ScorePInstructionDec/Capstone/ins_dec/libinsdec.so")
set(Capstone_LIBRARIES "/home/tinker/Arbeit/SHK/ScorePInstructionDec/Capstone/capstone/libcapstone.so.4")
add_definitions(-DHAVE_INS_DEC)
add_definitions(-DID_X86)

INCLUDE(CheckIncludeFiles)
CHECK_INCLUDE_FILES(linux/hw_breakpoint.h HAVE_HW_BREAKPOINT_H)

option(HW_BREAKPOINT_COMPAT "Time synchronization fallback for old kernels without hardware breakpoint support." OFF)

if (HW_BREAKPOINT_COMPAT)
    add_definitions(-DHW_BREAKPOINT_COMPAT)
    if (HAVE_HW_BREAKPOINT_H)
        message(WARNING "Using inaccurate compatibility time synchronization while hardware breakpoints are available. Consider deactivating HW_BREAKPOINT_COMPAT")
    endif()
else()
    if (NOT HAVE_HW_BREAKPOINT_H)
        message(SEND_ERROR "Missing linux/hw_breakpoint.h, install that or consider activating HW_BREAKPOINT_COMPAT for old kernels with compatibility time synchronization.")
    endif()
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GNU_SOURCE")

# Doesn't seem to work with clang, even though it should,
# but at least it doesn't complain about it either
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc -lcapstone")

include_directories(include)

set(SOURCE_FILES

	
        src/metric/plugin/plugin.cpp src/metric/plugin/channel.cpp src/metric/plugin/metrics.cpp
        src/metric/perf_counter.cpp

        src/monitor/cpu_set_monitor.cpp
        src/monitor/cpu_switch_monitor.cpp
        src/monitor/fd_monitor.cpp
        src/monitor/interval_monitor.cpp
        src/monitor/main_monitor.cpp
        src/monitor/process_monitor.cpp
        src/monitor/process_monitor_main.cpp
        src/monitor/thread_monitor.cpp
        src/monitor/threaded_monitor.cpp

        src/perf/sample/writer.cpp
        src/perf/time/converter.cpp src/perf/time/reader.cpp
        src/perf/tracepoint/format.cpp src/perf/tracepoint/metric_monitor.cpp
        src/perf/tracepoint/writer.cpp src/perf/tracepoint/switch_writer.cpp
        src/perf/tracepoint/exit_reader.cpp

        src/trace/counters.cpp src/trace/trace.cpp

        src/config.cpp src/main.cpp src/monitor/process_monitor.cpp src/thread_map.cpp
        src/platform.cpp
        src/topology.cpp src/bfd_resolve.cpp src/pipe.cpp
        src/mmap.cpp
        src/util.cpp	
        src/ins_dec.cpp
        /home/tinker/Arbeit/SHK/ScorePInstructionDec/Capstone/ins_dec/ins_dec.c
        ${X86_ADAPT_SOURCE_FILES}
        ${RADARE_SOURCE_FILES}
)


add_executable(lo2s ${SOURCE_FILES})
target_link_libraries(lo2s ${Capstone_LIBRARIES} ${InsDec_LIBRARIES} ${OTF2_LIBRARIES} ${Boost_LIBRARIES} ${Radare_LIBRARIES}
        ${Binutils_LIBRARIES} ${Dl_LIBRARIES} ${X86_ADAPT_LIBRARIES})

option(IWYU "Developer option for include what you use." OFF)

if (IWYU)
    find_program(iwyu_path NAMES include-what-you-use iwyu)
    if(NOT iwyu_path)
        message(FATAL_ERROR "Could not find the program include-what-you-use")
    endif()
    set_property(TARGET lo2s PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})
endif()


# Convince clion to accept headers as part of the project by adding them to a dummy target
# Should not interfer with normal opratation.
FILE(GLOB_RECURSE clion_dummy_headers *.hpp *.h)
FILE(GLOB_RECURSE clion_dummy_source main.cpp)
add_executable(clion_dummy_executable EXCLUDE_FROM_ALL ${clion_dummy_source} ${clion_dummy_headers})

install(TARGETS lo2s RUNTIME DESTINATION bin)
