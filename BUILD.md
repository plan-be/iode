# Building IODE

This document describes how to build IODE from source using CMake presets.

## Prerequisites

- CMake 3.31 or higher
- Ninja build system
- One of the following compilers:
  - **Windows**: Visual Studio (cl.exe) or LLVM Clang-CL (clang-cl.exe)
  - **Linux**: GCC (gcc/g++)
- Python 3.12 or higher (with NumPy)
- Google Test (automatically downloaded during configuration)

## CMake Presets

The project uses CMake presets to simplify the build configuration. Presets are defined in `CMakePresets.json`.

### Available Configure Presets

#### Windows (MSVC)
- `windows-debug` - Windows Debug build with MSVC (cl.exe)
- `windows-debug-sanitize` - Windows Debug with AddressSanitizer
- `windows-release` - Windows Release build with MSVC

#### Windows (Clang-CL)
- `windows-clang-debug` - Windows Debug build with Clang-CL
- `windows-clang-debug-sanitize` - Windows Debug with AddressSanitizer using Clang-CL
- `windows-clang-release` - Windows Release build with Clang-CL

#### Linux (GCC)
- `linux-debug` - Linux Debug build with GCC
- `linux-debug-sanitize` - Linux Debug with AddressSanitizer
- `linux-release` - Linux Release build with GCC

## Building with Clang-CL on Windows

Clang-CL is LLVM's Clang compiler with an MSVC-compatible command-line interface. It provides better standards compliance and potentially better diagnostics while maintaining compatibility with the MSVC toolchain.

### Step 1: Verify Clang-CL Installation

```powershell
clang-cl --version
```

This should display the Clang-CL version and installation path.

### Step 2: Configure the Project

```powershell
# For Debug build
cmake --preset windows-clang-debug

# For Release build
cmake --preset windows-clang-release

# For Debug with sanitizers
cmake --preset windows-clang-debug-sanitize
```

### Step 3: Build the Project

```powershell
# Build using the configured preset
cmake --build --preset windows-clang-debug

# Or for release
cmake --build --preset windows-clang-release
```

## Building with MSVC on Windows

### Step 1: Configure the Project

```powershell
# For Debug build
cmake --preset windows-debug

# For Release build
cmake --preset windows-release
```

### Step 2: Build the Project

```powershell
cmake --build --preset windows-debug
# Or
cmake --build --preset windows-release
```

## Building on Linux

### Step 1: Configure the Project

```bash
# For Debug build
cmake --preset linux-debug

# For Release build
cmake --preset linux-release
```

### Step 2: Build the Project

```bash
cmake --build --preset linux-debug
# Or
cmake --build --preset linux-release
```

## Running Tests

Test presets are available for each configuration:

```powershell
# Windows MSVC
ctest --preset c-api-windows-debug
ctest --preset cpp-api-windows-debug

# Windows Clang-CL
ctest --preset c-api-windows-clang-debug
ctest --preset cpp-api-windows-clang-debug

# Linux
ctest --preset c-api-linux-debug
ctest --preset cpp-api-linux-debug
```

## AddressSanitizer

AddressSanitizer (ASan) is a memory error detector that helps find:
- Use after free
- Heap buffer overflow
- Stack buffer overflow
- Memory leaks
- And more...

To use AddressSanitizer, use the `-sanitize` presets:

```powershell
# Windows Clang-CL with sanitizer
cmake --preset windows-clang-debug-sanitize
cmake --build --preset windows-clang-debug-sanitize

# Linux with sanitizer
cmake --preset linux-debug-sanitize
cmake --build --preset linux-debug-sanitize
```

**Note**: Clang-CL generally provides better AddressSanitizer support than MSVC.

## Listing Available Presets

To see all available presets:

```powershell
# Configure presets
cmake --list-presets

# Build presets
cmake --list-presets=build

# Test presets
cmake --list-presets=test
```

## Compiler Detection

CMake automatically detects the compiler:
- MSVC (cl.exe): Detected as `CMAKE_CXX_COMPILER_ID=MSVC`
- Clang-CL: Detected as `CMAKE_CXX_COMPILER_ID=Clang` with `CMAKE_CXX_SIMULATE_ID=MSVC`
- GCC: Detected as `CMAKE_CXX_COMPILER_ID=GNU`

The CMakeLists.txt files handle both MSVC and Clang-CL through the `if(MSVC)` checks, as Clang-CL is designed to be MSVC-compatible.

## Build Output

Build artifacts are placed in:
- Configuration: `out/build/<preset-name>/`
- Installation: `out/install/<preset-name>/`

## Troubleshooting

### Clang-CL Not Found
If CMake cannot find clang-cl, ensure it's in your PATH or specify it explicitly:

```powershell
$env:PATH += ";C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\Llvm\x64\bin"
```

### Ninja Not Found
Install Ninja using:
- Windows: `choco install ninja` or download from [ninja-build.org](https://ninja-build.org/)
- Linux: `sudo apt-get install ninja-build` (Ubuntu/Debian) or `sudo yum install ninja-build` (RHEL/CentOS)

## Additional Resources

- [CMake Presets Documentation](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html)
- [Clang-CL Documentation](https://clang.llvm.org/docs/UsersManual.html#clang-cl)
- [IODE Documentation](https://iode.readthedocs.io/en/stable/)
