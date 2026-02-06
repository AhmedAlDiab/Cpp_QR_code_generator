cmake_minimum_required(VERSION 3.21)
project(QR_code_generator)

set(CMAKE_CXX_STANDARD 20)

# Point CMake to Conan CMakeDeps output
list(APPEND CMAKE_PREFIX_PATH "C:/AAD-Projects/QR code generator/Code/conan/deb/build/generators")

find_package(wxWidgets CONFIG REQUIRED)

add_executable(QR_code_generator
  WIN32
  QR_code_generator.cpp
  QR_code_generator.h
)

# wxWidgets CMake package usually provides these variables:
# wxWidgets_LIBRARIES, wxWidgets_INCLUDE_DIRS, wxWidgets_DEFINITIONS
target_include_directories(QR_code_generator PRIVATE ${wxWidgets_INCLUDE_DIRS})
target_compile_definitions(QR_code_generator PRIVATE ${wxWidgets_DEFINITIONS})
target_link_libraries(QR_code_generator PRIVATE ${wxWidgets_LIBRARIES})
