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
- `iode_scr4`    -> C library providing many of the utility functions used in the C API.
- `iode_c_api`   -> Core API of IODE (in pure C).
- `iode_cpp_api` -> C++ classes that wrap IODE C structure (used in the GUI Qt part).
- `iode_gui`     -> Graphical user interface (GUI) based on Qt.
- `iode_python`  -> Builds Python binding for IODE (base on Cython).
- `nsis`         -> Builds a Windows Installer for the users.
- `test_c_api`   -> Builds the tests for the C API (based on Google Test).
- `test_cpp_api` -> Builds the tests for the C++ classes (based on Google Test).
- `keyboard_shortcuts` -> Builds the PDF referencing all the keyboard shortcuts in the GUI (required LaTeX).

To build Python IODE, please run the Batch script makepy.bat in the subdirectory pydiode 
and provide a conda environment as argument:
```bash
pyiode> makepy.bat py39 
```

# Testing Project

To test the C and C++ API, you first have to build them as explain above:
```bash
> cmake --build --preset <preset_config> --target iode_c_api
> cmake --build --preset <preset_config> --target iode_cpp_api
```

The second step is to build the tests:
```bash
> cmake --build --preset <preset_config> --target test_c_api
> cmake --build --preset <preset_config> --target test_cpp_api
```

The final and third step is to ask CMake to run the tests:
```bash
> ctest --preset c-api-<preset_config>
> ctest --preset cpp-api-<preset_config>
```

# Working On An Issue

To work on (an) issue(s), create a new local branch from the remote origin/master branch.
The name of the branch must follow the template: \<your initials\>\_\<keyword\>\_\<rest\> 
where `keyword` is:
- `cli` or `cmd` when working on the IODE CLI (cmd)
- `gui` when working on the graphical user interface
- `cython` when working on the Python binding for IODE
- `doc` when working on the user of dev documentation
- `nsis` when working on the Windows Installer and Updater
- `release` when preparing a new release 

Each keyword triggers a specific job in the defined Github Actions workflows (see below).
If none of the keywords above is used, the C and C++ API will be built and tested.

# Github Actions

The Github Actions workflows for IODE are defined in the directory ./.github/workflows.

The sub-directory ./.github/actions contains the local actions (used in workflows).

The jobs declared in ./.github/workflows/github-actions-debug.yml are triggered according to keywords present 
in the name of the pushed branch:
- `build_and_test_api`  -> NOT triggered if the branch name contains the word `cmd`, `cli`, `gui`, `cython`, `doc` or `nsis`
- `test_sanitize`       -> NOT triggered if the branch name contains the word `cmd`, `cli`, `gui`, `cython`, `doc` or `nsis`
- `build_cli`           -> triggered if the branch name contains the word `cmd` or `cli`
- `build_gui`           -> triggered if the branch name contains the word `gui`
- `build_cython`        -> triggered if the branch name contains either the word `cython` or `nsis`
- `build_doc`           -> triggered if the branch name contains either the word `doc` or `nsis`
- `build_nsis`          -> triggered if the branch name contains either the word `nsis` or `release`
