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

# Preparing Python IODE Build

To prepare the building of Python IODE, please create the following conda environments:
```bash
> conda config --add channels larray-project
> conda create --name py39 python=3.9 numpy pandas larray cython
> conda create --name py310 python=3.10 numpy pandas larray cython
> conda create --name py311 python=3.11 numpy pandas larray cython
```

# Building Project

The different targets (libraries and executables) of the project are build using the tool CMake.

Before to build any target, make sure your CMake cache is up to date.
To rebuild the CMake cache, execute:
```bash
> cmake --preset <preset_config>
```
where the list of `<preset_config>` can be found in CMakePresets.json (buildPresets section):
- `windows-debug`
- `windows-debug-sanitize`
- `windows-release`

To build a specific target, you need to run
```bash
> cmake --build --preset <preset_config> --target <target>
```
where `<target>` is one the item in the list below:
- `iode_scr4`     -> C library providing many of the utility functions used in the C API.
- `iode_c_api`      -> Core API of IODE (in pure C).
- `iodecppapi`   -> C++ classes that wrap IODE C structure (used in the GUI Qt part).
- `iode_gui`     -> Graphical user interface (GUI) based on Qt.
- `pyiode`       -> Builds Python binding for IODE (base on Cython).
- `nsis`         -> Builds a Windows Installer for the users.
- `test_c_api`   -> Builds the tests for the C API (based on Google Test).
- `test_cpp_api` -> Builds the tests for the C++ classes (based on Google Test).
- `keyboard_shortcuts` -> Builds the PDF referencing all the keyboard shortcuts in the GUI (required LaTeX).

To build Python IODE, please run the Batch script makepy.bat in the subdirectory pydiode 
and provide a conda environment as argument:
```bash
pyiode> makepy.bat py39 
```
