# Requirements

- Required tools:
  - Compiler MSVC 2019 or 2022.
  - [Chocolatey](https://chocolatey.org/install) (*) -> C/C++ package manager.
  - [CMake](https://cmake.org/download) (>= 3.25) (*) -> controls the software compilation process.
  - [Ninja](https://ninja-build.org/) -> build system used by CMake. 
  <br> Can be installed using choco:
    > choco install ninja
  - LaTeX ([Tex Live](https://www.tug.org/texlive)) -> Scripting language used in the IODE user documentation.
  - [miniconda](https://docs.conda.io/en/latest/miniconda.html) (*) -> Python package manager.
  - [NSIS](https://nsis.sourceforge.io/Download) (*) -> creates Windows installers
  - SCR4 executables -> used to build the user documentation. 
  <br> Ask maintainers to get access.

- C++ libraries:
  - Boost (>= 1.74). 
  <br> Can be installed with Chocolatey. 
  <br> Install the package corresponding to your VS version by checking [here](https://community.chocolatey.org/packages?q=boost):
    > choco install boost-msvc-14.X
  - [Qt 6.4](https://www.qt.io/download)
  <br> Please install Qt in C:/Qt.

(*) already installed on the Github Actions host-runner [windows-xxx](https://docs.github.com/en/actions/using-github-hosted-runners/about-github-hosted-runners#supported-runners-and-hardware-resources)

# Repository Structure

- **.github\workflows**: Script for Github Actions (Tests + Build GUI + Build LaTeX doc)
- **api**: C API
- **cpp_api**: C++ API
- **doc**: User documentation
- **gui**: Qt GUI
- **nsis**: NSIS Windows installer
- **pyiode**: Python module of IODE
- **scr4**: Utility functions required to compile the C API
- **tests**: Testing directory

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
- nsis
- test_c_api
- test_cpp_api
- keyboard_shortcuts
