cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME freertos)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/portable/MemMang/heap_4.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Systemview/Config/SEGGER_SYSVIEW_Config_FreeRTOS.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Systemview/OS/SEGGER_SYSVIEW_FreeRTOS.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Systemview/SEGGER/SEGGER_SYSVIEW.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Systemview/SEGGER/SEGGER_RTT.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Systemview/SEGGER/SEGGER_RTT_printf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Systemview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Systemview/SEGGER/SEGGER_RTT_ASM_ARMv7M.S
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/tasks_handle.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    ${CMAKE_CURRENT_SOURCE_DIR}/Systemview/Config
    ${CMAKE_CURRENT_SOURCE_DIR}/Systemview/OS
    ${CMAKE_CURRENT_SOURCE_DIR}/Systemview/SEGGER
   
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Tell FreeRTOS where FreeRTOSConfig.h is
add_library(freertos_config INTERFACE)

target_include_directories(freertos_config
    SYSTEM INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc
        ${CMAKE_CURRENT_SOURCE_DIR}/Systemview/Config
        ${CMAKE_CURRENT_SOURCE_DIR}/Systemview/OS
        ${CMAKE_CURRENT_SOURCE_DIR}/Systemview/SEGGER
)

target_compile_definitions(freertos_config
    INTERFACE
        projCOVERAGE_TEST=0
)

set(FREERTOS_PORT GCC_ARM_CM4F CACHE STRING "")

# Add FreeRTOS as a subdirectory
add_subdirectory(FreeRTOS)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
    freertos_kernel
)
