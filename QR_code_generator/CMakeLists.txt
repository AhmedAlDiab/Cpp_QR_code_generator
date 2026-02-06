cmake_minimum_required(VERSION 3.21)
project(QR_code_generator)

set(CMAKE_CXX_STANDARD 20)

# ---- Select Conan generators folder (per-arch + per-config) ----

# Arch folder: x64 or x86
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(_conan_arch "x64")
else()
    set(_conan_arch "x86")
endif()

# Config folder: deb or rel
if(CMAKE_CONFIGURATION_TYPES) # Multi-config (VS, Xcode)
    set(_conan_cfg "$<IF:$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>,rel,deb>")
else() # Single-config (Ninja/Makefiles)
    if(NOT CMAKE_BUILD_TYPE)
        message(FATAL_ERROR "CMAKE_BUILD_TYPE is required (Debug/Release/RelWithDebInfo/MinSizeRel).")
    endif()

    string(TOLOWER "${CMAKE_BUILD_TYPE}" _bt)
    if(_bt STREQUAL "release" OR _bt STREQUAL "relwithdebinfo" OR _bt STREQUAL "minsizerel")
        set(_conan_cfg "rel")
    else()
        set(_conan_cfg "deb")
    endif()
endif()

list(APPEND CMAKE_PREFIX_PATH
    "${CMAKE_SOURCE_DIR}/conan/${_conan_arch}/${_conan_cfg}/build/generators"
)

# Optional debug print
message(STATUS "Conan generators: ${CMAKE_SOURCE_DIR}/conan/${_conan_arch}/${_conan_cfg}/build/generators")

# ---------------------------------------------------------------

find_package(wxWidgets CONFIG REQUIRED)

if (WIN32)
  add_executable(QR_code_generator WIN32
    QR_code_generator.cpp
    qrcodegen.cpp
  )
else()
  add_executable(QR_code_generator
    QR_code_generator.cpp
    qrcodegen.cpp
  )
endif()

# Keep your variable-based linking:
target_include_directories(QR_code_generator PRIVATE ${wxWidgets_INCLUDE_DIRS})
target_compile_definitions(QR_code_generator PRIVATE ${wxWidgets_DEFINITIONS})
target_link_libraries(QR_code_generator PRIVATE ${wxWidgets_LIBRARIES})
