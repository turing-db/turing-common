cmake_minimum_required(VERSION 3.20)

project(turing-common)

# C++ standard
set(CPP_STANDARD "c++20")

# C++ warning flags
set(CPP_WARNINGS "-Wall -Werror -Wunused-function -Wunused-variable")

# Detect debug build
set(DEBUG_BUILD FALSE)
if (DEFINED ENV{DEBUG_BUILD})
    if ($ENV{DEBUG_BUILD} EQUAL "1")
        set(DEBUG_BUILD TRUE)
    endif()
endif()

# Detect RELDEBUG
set(RELDEBUG FALSE)
if (DEFINED ENV{RELDEBUG})
    if ($ENV{RELDEBUG} EQUAL "1")
        set(RELDEBUG TRUE)
    endif()
endif()

# Detect CALLGRIND_PROFILE
set(CALLGRIND_PROFILE FALSE)
if (DEFINED ENV{CALLGRIND_PROFILE})
    if ($ENV{CALLGRIND_PROFILE} EQUAL "1")
        set(CALLGRIND_PROFILE TRUE)
    endif()
endif()

if(${CALLGRIND_PROFILE})
    if (NOT ${DEBUG_BUILD})
        set(RELDEBUG TRUE)
    endif()
    add_definitions(-DCALLGRIND_PROFILE)
endif()

# Detect TURING_DEV
set(TURING_DEV FALSE)
if (DEFINED ENV{TURING_DEV})
    set(TURING_DEV TRUE)
endif()

if (${TURING_DEV})
    add_definitions(-DTURING_DEV)
endif()

# Detect TURING_PROFILE
set(TURING_PROFILE 0)
if ($ENV{TURING_PROFILE})
    set(TURING_PROFILE 1)
    message(STATUS "Turing profile mode enabled")
endif()

# Debug settings
if (${DEBUG_BUILD})
    message(STATUS "Debug build")
    set(OPTIMIZE_FLAGS "-O0 -g -fcf-protection=none")
else()
    message(STATUS "Optimized build")
    add_definitions(-DNDEBUG)
    set(OPTIMIZE_FLAGS "-O3 -fcf-protection=none")

    if (${RELDEBUG})
        set(OPTIMIZE_FLAGS "${OPTIMIZE_FLAGS} -g")
    endif()
endif()

# C++ flags
set(CMAKE_CXX_FLAGS "-std=${CPP_STANDARD} ${CPP_WARNINGS} ${OPTIMIZE_FLAGS}")

# Create compilation database and symlink in the src folder
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
)

# Threads
find_package(Threads REQUIRED)

add_subdirectory(external)
add_subdirectory(lib)
