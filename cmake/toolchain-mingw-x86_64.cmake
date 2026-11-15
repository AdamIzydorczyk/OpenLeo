# toolchain-mingw-x86_64.cmake - cross-build the Windows exe with the
# vendored llvm-mingw + vendor/SDL2 (tips T-20).  Use with:
#   PKG_CONFIG_LIBDIR=vendor/SDL2-2.32.10/x86_64-w64-mingw32/lib/pkgconfig \
#     cmake -B build-win -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-mingw-x86_64.cmake
# Compiler resolution mirrors the Makefile XC rule: vendored llvm-mingw if
# present, else the system x86_64-w64-mingw32 tools (CI runs without the
# gitignored vendor/llvm-mingw; the SDL2 pkgconfig path is passed via env).
get_filename_component(ROOT "${CMAKE_CURRENT_LIST_DIR}" DIRECTORY)  # repo root
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
find_program(TC_MINGW_CLANG x86_64-w64-mingw32-clang
             HINTS "${ROOT}/vendor/llvm-mingw/bin" NO_DEFAULT_PATH)
if(TC_MINGW_CLANG)
  set(CMAKE_C_COMPILER  "${TC_MINGW_CLANG}")
  set(CMAKE_RC_COMPILER "${ROOT}/vendor/llvm-mingw/bin/x86_64-w64-mingw32-windres")
else()
  find_program(TC_MINGW_CC x86_64-w64-mingw32-gcc)
  find_program(TC_MINGW_RC x86_64-w64-mingw32-windres)
  if(NOT TC_MINGW_CC OR NOT TC_MINGW_RC)
    message(FATAL_ERROR
      "no x86_64-w64-mingw32 cross toolchain: vendor/llvm-mingw absent "
      "and no system mingw-w64 installed")
  endif()
  set(CMAKE_C_COMPILER  "${TC_MINGW_CC}")
  set(CMAKE_RC_COMPILER "${TC_MINGW_RC}")
endif()
set(CMAKE_FIND_ROOT_PATH "${ROOT}/vendor/SDL2-2.32.10/x86_64-w64-mingw32")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
