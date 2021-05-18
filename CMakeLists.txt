# CMake minimum version
cmake_minimum_required(VERSION 3.0)

# project name and languages
project(SFMT C)

# options
option(USE_SSE2 "Use SSE2" OFF)
option(USE_AVX2 "Use AVX2" OFF)
option(USE_AVX512 "Use AVX512" OFF)
option(USE_ALTI "Use ALTI" OFF)
option(USE_OSX_ALTI "Use OSX ALTI" OFF)
option(USE_BIG64 "Use BIG64" OFF)

option(USE_M607 "Use M607" OFF)
option(USE_M1279 "Use M1279" OFF)
option(USE_M2281 "Use M2281" OFF)
option(USE_M11213 "Use M11213" OFF)
option(USE_M19937 "Use M19937" OFF)
option(USE_M44497 "Use M44497" OFF)
option(USE_M86243 "Use M86243" OFF)
option(USE_M132049 "Use M132049" OFF)
option(USE_M216091 "Use M216091" OFF)

# MEXP variable
if(USE_M607)
    set(MEXP 607)
elseif(USE_M1279)
    set(MEXP 1279)
elseif(USE_M2281)
    set(MEXP 2281)
elseif(USE_M11213)
    set(MEXP 11213)
elseif(USE_M19937)
    set(MEXP 19937)
elseif(USE_M44497)
    set(MEXP 44497)
elseif(USE_M86243)
    set(MEXP 86243)
elseif(USE_M132049)
    set(MEXP 132049)
elseif(USE_M216091)
    set(MEXP 216091)
else()
    set(MEXP 19937) # default
endif()

# optimize
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -DNDEBUG")

# SFMT_DEFS and CMAKE_C_FLAGS
if(MSVC) # Visual C++
    if(USE_SSE2)
        set(SFMT_DEFS SFMT_MEXP=${MEXP} HAVE_SSE2=1)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /arch:SSE2")
    elseif(USE_AVX2)
        set(SFMT_DEFS SFMT_MEXP=${MEXP} HAVE_SSE2=1)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /arch:AVX2")
    elseif(USE_AVX512)
        set(SFMT_DEFS SFMT_MEXP=${MEXP} HAVE_SSE2=1)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /arch:AVX512")
    else()
        set(SFMT_DEFS SFMT_MEXP=${MEXP}) # default
    endif()
else()
    if(USE_SSE2)
        set(SFMT_DEFS SFMT_MEXP=${MEXP} HAVE_SSE2=1)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse")
    elseif(USE_AVX2)
        set(SFMT_DEFS SFMT_MEXP=${MEXP} HAVE_SSE2=1)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mavx2")
    elseif(USE_AVX512)
        set(SFMT_DEFS SFMT_MEXP=${MEXP} HAVE_SSE2=1)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mavx512vl")
    elseif(USE_ALTI)
        set(SFMT_DEFS SFMT_MEXP=${MEXP} HAVE_ALTIVEC=1)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mabi=altivec -maltivec")
    elseif(USE_OSX_ALTI)
        set(SFMT_DEFS SFMT_MEXP=${MEXP} HAVE_ALTIVEC=1)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -faltivec -maltivec")
    elseif(USE_BIG64)
        set(SFMT_DEFS SFMT_MEXP=${MEXP} ONLY64=1)
    else()
        set(SFMT_DEFS SFMT_MEXP=${MEXP}) # default
    endif()
endif()

# libSFMT.a --- the static library
add_library(SFMT STATIC SFMT.c)
target_compile_definitions(SFMT PRIVATE ${SFMT_DEFS})

# test.exe --- the test program
add_executable(test test.c)
target_compile_definitions(test PRIVATE ${SFMT_DEFS})
target_link_libraries(test PRIVATE SFMT)
