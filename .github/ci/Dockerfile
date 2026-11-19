# CI toolchain image for the linux + asan jobs of .github/workflows/ci.yml
# (built and pushed by .github/workflows/ci-image.yml).
#
# WHY: the measured cost of the old "Install toolchain" step in the linux
# job was ~4m07s, dominated by a 149 MB apt download (the mingw-w64
# cross-toolchain ~120 MB, cmake, the libsdl2-dev dependency closure).
# Baking the SAME package list into the image turns that whole step into
# an image pull. The image carries ONLY the toolchain - game data never
# enters it, exactly like the old apt step, so every fixture probe in the
# Makefile still sees a bare machine and prints its named SKIP.
#
# Base: plain Ubuntu LTS, i.e. the same apt world the old
# `runs-on: ubuntu-latest` + apt-get recipe produced. Pinned to 26.04
# ("resolute") on purpose: that is the release today's runners resolve
# against (run 44523679146's log shows azure.archive.ubuntu.com
# resolute/* with cmake 4.2.3 and gcc-mingw-w64 13.2), so the toolchain
# versions here match the ones every past green run used. Do NOT drop to
# an older LTS to save bytes: gcc/cmake/mingw drift is exactly what the
# ZERO-warnings gate and the build-parity legs are sensitive to.
# The Makefile's own XC/WINDRES fallbacks (system prefixed mingw names
# first) match this image; the local vendored vendor/llvm-mingw
# (gitignored) is untouched.
#
# NOT baked on purpose: the SDL2 mingw DEVEL tarball (the "Provision SDL2"
# step in ci.yml keeps it ~2 s and keeps its sha256 pin live and auditable).
# If it is ever baked, keep the version aligned with the Makefile's SDL_WIN
# path (vendor/SDL2-<ver>/x86_64-w64-mingw32) and bump both together.

FROM ubuntu:26.04
ENV DEBIAN_FRONTEND=noninteractive

# build-essential pkg-config libsdl2-dev        <- native target + the
#   cmake python3                                 verify-cmake / py oracle legs
# gcc-mingw-w64-x86-64 binutils-mingw-w64-x86-64 <- the `make win` cross target
# ca-certificates curl                              <- the Provision-SDL2 leg (https + sha256sum -c)
# git                                              <- checkout actions run on the host,
#   but keep git inside for any in-job `git` probing
RUN apt-get update \
 && apt-get -y install --no-install-recommends \
      build-essential pkg-config libsdl2-dev \
      gcc-mingw-w64-x86-64 binutils-mingw-w64-x86-64 \
      cmake python3 ca-certificates curl git \
 && rm -rf /var/lib/apt/lists/*

# The asan job passes -fsanitize=address,undefined through plain gcc: the
# asan/ubsan runtimes ship with gcc (the old CI proved it), so no extra
# packages are needed here either.

# Default: root (plain `make`, artifacts and mounted workspace all fine
# under the Actions container contract).
