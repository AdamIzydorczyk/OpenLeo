# toolchain-mingw-x86_64.cmake - cross-build the Windows exe with the
# vendored llvm-mingw + vendor/SDL2 (tips T-20).  Use with:
#   PKG_CONFIG_LIBDIR=vendor/SDL2-2.32.10/x86_64-w64-mingw32/lib/pkgconfig \
#     cmake -B build-win -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-mingw-x86_64.cmake
get_filename_component(ROOT "${CMAKE_CURRENT_LIST_DIR}" DIRECTORY)  # repo root
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(CMAKE_C_COMPILER "${ROOT}/vendor/llvm-mingw/bin/x86_64-w64-mingw32-clang")
set(CMAKE_RC_COMPILER "${ROOT}/vendor/llvm-mingw/bin/x86_64-w64-mingw32-windres")
set(CMAKE_FIND_ROOT_PATH "${ROOT}/vendor/SDL2-2.32.10/x86_64-w64-mingw32")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
