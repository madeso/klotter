# Building klotter

This document describes how to build the **klotter** project from source.

## Prerequisites

- **C++ Compiler**: GCC 12+, Clang 14+ or MSVC 19/VS 2022
- **CMake**: Version 3.16 or newer
- **SDL2** development libraries

On Ubuntu, you can install dependencies with:
```sh
sudo apt update
sudo apt install build-essential cmake libsdl2-dev git
```

## Cloning with git

```sh
git clone --recursive https://github.com/madeso/klotter.git
cd klotter
```

If you forgot `--recursive`, run:
```sh
git submodule update --init --recursive
```

## Building

Create a build directory and run CMake:

```sh
cmake -E make_directory build
cd build
cmake ..
cmake --build .
```

You can specify build type and options:
```sh
cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_GL_DEBUG=ON -DBLUR_USE_GAUSS=ON ..
```

## Running

After building, the binaries will be in the `build` directory. Run the main executable:

```sh
./klotter
```
or on Windows:
```sh
klotter.exe
```

## Optional: Running Tests

To run the tests, use:
```sh
ctest
```

## Troubleshooting

- Make sure all dependencies are installed.
- If you encounter compiler errors related to C++20/23 features, ensure your compiler is up to date.
- For issues with submodules, run `git submodule update --init --recursive`.

## Notes

- For documentation generation, use Doxygen (`doxygen Doxyfile`).

---

For more details, see the [README.md](README.md) or open an issue on GitHub.
