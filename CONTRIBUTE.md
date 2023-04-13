# Requirements

- Compiler:
  - MSVC 2019
- To compile the C API:
  - [CMake](https://cmake.org, https://cmake.org/download) (>= 3.25) (*)
- To compile the C++ API:
  - [Chocolatey](https://chocolatey.org/install) (to install Boost) (*)
  - Boost (>= 1.74). Can be installed with Chocolatey. For VS2019, execute: choco install boost-msvc-14.2. 
    For other VS versions, check on https://community.chocolatey.org/packages?q=boost
- To compile the Qt GUI:
  - [Qt 6.4](https://www.qt.io/download) (to be installed in C:/Qt)
- To compile the documentation:
  - SCR4 executables -> ask maintainers
  - LaTeX ([Tex Live](https://www.tug.org/texlive))
- To compile pyiode:
  - [miniconda](https://docs.conda.io/en/latest/miniconda.html) (for managing Python) (*)
  - Python 3.9+
  - cython conda package
  - larray conda package
- To create an installer:
  - [NSIS](https://nsis.sourceforge.io/Download) (*)
- To compile the old GUI (DOS):
  - [Embarcadero C++ Compiler](https://www.embarcadero.com/free-tools/ccompiler)
  - SCR4 executables + lib -> ask maintainers

(*) already installed on the Github Actions host-runner [windows-xxx](https://docs.github.com/en/actions/using-github-hosted-runners/about-github-hosted-runners#supported-runners-and-hardware-resources)

# Repository Structure

- **.github\workflows**: script for Github Actions (Tests + Build GUI + Build LaTeX doc)
- **api**: C API
- **cpp_api**: C++ API
- **doc**: user + dev documentation
- **gui**: Qt GUI
- **pyiode**: Python module of IODE
- **scr4**: utility functions required to compile the C API
- **tests**: testing directory

# Building Project

Either find the IDE Extension which is right for you or:
```bash
> cmake --preset <preset_config>
> cmake --build --preset <preset_config> --target <target>
```
where the list of `<preset_config>` can be found in CMakePresets.json (buildPresets section) 
and `<target>` are deduced from CMakeLists.txt:
- scr4iode
- iodeapi
- iodecppapi
- iode_gui
- test_c_api
- test_cpp_api
- keyboard_shortcuts
