# CMakeLists.txt - alternate build alongside the Makefile (tips T-20).
#
# The Makefile stays the reference + CI build (every verify-* target is
# shell-driven); this file exists for IDE / vcpkg / Conan users:
#
#   linux:   cmake -B build && cmake --build build
#   vcpkg:   cmake -B build -DCMAKE_TOOLCHAIN_FILE=%VCPKG%/scripts/buildsystems/vcpkg.cmake
#   mingw:   cmake -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=vendor/SDL2-2.32.10/x86_64-w64-mingw32
#
# Scope: the game binary only. The test tools and the verify-* suite are
# Makefile-only. Engine code is gnu11 (strcasecmp / strtok_r / dirent.h):
# works on gcc, clang and MinGW today; MSVC needs the compat.h shim that
# tips T-20 sketches - not ported yet.
#
# Linkage note: this build resolves SDL through its package config, so
# the windows exe links SDL2.dll (put it next to the exe - the common
# shipping choice, tips T-21). The Makefile's `make win` stays the
# static single-file build.
cmake_minimum_required(VERSION 3.16)
project(ll_cutscene C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS ON)              # gnu11: POSIX strtok_r, strcasecmp, dirent.h

# Same source list as the Makefile's SRC.
add_executable(ll_cutscene
    ported/main.c
    ported/is_stream.c
    ported/platform_sdl/is_player.c
    ported/platform_sdl.c
    ported/platform_common.c
    ported/llrs.c
    ported/ll_pcx.c
    ported/ll_menu.c
    ported/ll_level.c
    ported/ll_play.c
    ported/ll_seq.c
    ported/ll_stage.c
    ported/ll_pcm.c
    ported/ll_mod.c
    ported/ll_save.c
    ported/fs_ci.c
    ported/ll_view.c
    ported/ll_audio.c
    ported/ll_paths.c)
target_include_directories(ll_cutscene PRIVATE ported)

# One platform knob; everything else derives from it in ll_config.h
# (FS_CASE_INSENSITIVE, audio/device defaults...).
if(WIN32)
    target_compile_definitions(ll_cutscene PRIVATE PLATFORM_WIN=1 main=SDL_main)
    # icon + VERSIONINFO + app.manifest (review P-14b); rc.exe/llvm-rc/WindRes
    # all accept the numeric statements used in the .rc.
    target_sources(ll_cutscene PRIVATE resources/ll_cutscene.rc)
else()
    target_compile_definitions(ll_cutscene PRIVATE PLATFORM_LINUX=1)
endif()

if(MSVC)
    target_compile_options(ll_cutscene PRIVATE /W3)
else()
    target_compile_options(ll_cutscene PRIVATE -Wall -Wextra)
endif()

# SDL2 import target: vcpkg and SDL's own configs export SDL2::SDL2 (+
# SDL2::SDL2main for the main=SDL_main shim); older distro packaging
# exports SDL2::SDL. The libsdl.org mingw-xc builds (vendor/SDL2-*) only
# ship pkg-config, so fall back to that.
find_package(SDL2 CONFIG QUIET)
if(TARGET SDL2::SDL2)
    if(WIN32 AND TARGET SDL2::SDL2main)
        target_link_libraries(ll_cutscene PRIVATE SDL2::SDL2main)
    endif()
    target_link_libraries(ll_cutscene PRIVATE SDL2::SDL2)
    set(_sdl2_target SDL2::SDL2)
elseif(TARGET SDL2::SDL)
    target_link_libraries(ll_cutscene PRIVATE SDL2::SDL)
    set(_sdl2_target SDL2::SDL)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    target_include_directories(ll_cutscene PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_directories(ll_cutscene PRIVATE ${SDL2_LIBRARY_DIRS})
    if(WIN32)
        target_link_libraries(ll_cutscene PRIVATE mingw32 SDL2main)
    endif()
    target_link_libraries(ll_cutscene PRIVATE ${SDL2_LIBRARIES})
    set(_sdl2_target "")
    foreach(_d IN LISTS SDL2_INCLUDE_DIRS)   # same <SDL2/SDL.h> parenting
        if(_d MATCHES "/SDL2$")
            target_include_directories(ll_cutscene PRIVATE "${_d}/..")
        endif()
    endforeach()
endif()

# This codebase includes <SDL2/SDL.h> (the Makefile passes the PARENT of
# the SDL headers).  Some configs export .../include/SDL2 directly; also
# accept those by exposing the parent directory.
if(_sdl2_target)
    get_target_property(_sdl2_inc ${_sdl2_target} INTERFACE_INCLUDE_DIRECTORIES)
    foreach(_d IN LISTS _sdl2_inc)
        if(_d MATCHES "/SDL2$")
            target_include_directories(ll_cutscene PRIVATE "${_d}/..")
        endif()
    endforeach()
endif()

# W-04 (deep dive): this build links SDL2.dll dynamically (unlike the
# Makefile's static single-file `make win`), so ship the DLL next to the
# exe - without it the exe dies at LoadLibrary before main(). Manifest +
# icon are already embedded via resources/ll_cutscene.rc above; MSVC
# additionally needs the compat.h shim (header note, still open).
if(WIN32)
    set(_sdl2_dll "")
    if(_sdl2_target)
        get_target_property(_sdl2_type ${_sdl2_target} TYPE)
        if(_sdl2_type STREQUAL "SHARED_LIBRARY")     # vcpkg-static etc: skip
            set(_sdl2_dll "$<TARGET_FILE:${_sdl2_target}>")
        endif()
    else()
        # pkg-config path (libsdl.org mingw-xc builds): SDL2.dll lives in
        # the bin/ sibling of the reported lib dir, or beside it.
        foreach(_d IN LISTS SDL2_LIBRARY_DIRS)
            if(_sdl2_dll)
                continue()
            endif()
            foreach(_c "${_d}/../bin/SDL2.dll" "${_d}/../SDL2.dll"
                       "${_d}/SDL2.dll")
                if(EXISTS "${_c}")
                    set(_sdl2_dll "${_c}")
                    break()
                endif()
            endforeach()
        endforeach()
    endif()
    if(_sdl2_dll)
        add_custom_command(TARGET ll_cutscene POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${_sdl2_dll}" "$<TARGET_FILE_DIR:ll_cutscene>"
            COMMENT "copying SDL2 runtime DLL next to ll_cutscene (W-04)"
            VERBATIM)
    else()
        message(WARNING
            "W-04: no SDL2.dll found to copy next to ll_cutscene.exe - "
            "the build will need it shipped manually.")
    endif()
endif()
