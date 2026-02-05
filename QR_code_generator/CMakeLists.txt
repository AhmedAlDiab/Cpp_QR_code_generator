# CMakeList.txt : CMake project for QR_code_generator, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (QR_code_generator "QR_code_generator.cpp" "QR_code_generator.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET QR_code_generator PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
