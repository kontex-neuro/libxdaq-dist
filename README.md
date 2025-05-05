# libxdaq

`libxdaq` is a C++ interface library for interacting with XDAQ hardware devices. This repository provides precompiled binary distributions for multiple platforms.

## Supported Platforms

Prebuilt binaries are available for:

* Windows (x64)
* macOS (x64 and arm64)
* Linux (x64)

## Requirements

* CMake ≥ 3.26
* A C++23-compatible compiler

Tested configurations:

* **Windows 11**: MSVC 194x (Visual Studio 2022)
* **Ubuntu 24.04**: GCC 14.2.0
* **macOS 15**: Clang 18.1.8 (via Homebrew)

## Installation

This package is **not available on Conan Center**. To use it, clone this repository and export it to your local Conan cache:

```bash
git clone https://github.com/kontex-neuro/libxdaq-dist.git
cd libxdaq-dist
conan create . -s build_type=Release --build=missing
```

## Usage

Add `libxdaq` as a dependency in your `conanfile.txt` or `conanfile.py`:

```
[requires]
libxdaq/x.y.z
```

Then link against it in your `CMakeLists.txt`:

```cmake
find_package(xdaq REQUIRED)
target_link_libraries(your_target PRIVATE xdaq::xdaq_device)
```

## Documentation

API documentation and setup guides will be added in a future update.

## Related Projects

* [XDAQ-OE](https://github.com/kontex-neuro/XDAQ-OE) — Open Ephys plugin using libxdaq
* [Intan-RHX](https://github.com/kontex-neuro/Intan-RHX) — RHX fork with libxdaq support
* [pylibxdaq](https://github.com/kontex-neuro/pylibxdaq) — Python interface for XDAQ