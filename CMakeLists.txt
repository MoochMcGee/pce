cmake_minimum_required(VERSION 3.1)
project(pce C CXX)

# Set _DEBUG macro for Debug builds.
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

# Detect C++ version support.
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  include(CheckCXXCompilerFlag)
  CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
  if(NOT COMPILER_SUPPORTS_CXX17)
    message(ERROR "No C++17 support. Please use a more recent compiler.")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
  endif()

  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-switch")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-switch -Wno-class-memaccess -Wno-invalid-offsetof")
endif()

# Enable threads everywhere.
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

option(ENABLE_OPENGL "Enables OpenGL support in renderer" ON)
option(ENABLE_STANDALONE_DISASM "Compiles the standalone disassembler" ON)
option(ENABLE_SDL_FRONTEND "Compiles the SDL frontend" ON)
option(ENABLE_QT_FRONTEND "Compiles the Qt frontend" ON)
option(ENABLE_TESTS "Compiles the tests" ON)

option(ENABLE_VOODOO "Enables Voodoo Graphics emulation based on MAME" ON)

#if(ENABLE_OPENGL)
#find_package(OpenGL REQUIRED)
#endif()
if(ENABLE_SDL_FRONTEND)
  find_package(SDL2 REQUIRED)
endif()

enable_testing()
add_subdirectory(dep)
add_subdirectory(src)
