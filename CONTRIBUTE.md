# Requirements

- Required tools:
  - **Windows**:
    - Compiler **MSVC 2022**.
    - [CMake](https://cmake.org/download) (>= 3.20) (*) -> controls the software compilation process.
    - [Ninja](https://ninja-build.org/) -> build system used by CMake. 
    <br> Can be installed using [winget](https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages#windows) on Windows:
      > winget install Ninja-build.Ninja
    - LaTeX ([Tex Live](https://www.tug.org/texlive)) -> Scripting language used in the IODE user documentation.
    - [miniconda](https://docs.conda.io/en/latest/miniconda.html) (*) -> Python package manager.
    - [NSIS](https://nsis.sourceforge.io/Download) (*) -> creates Windows installers
    - SCR4 executables -> used to build the user documentation. 
    <br> Ask maintainers to get access.
  - **Linux (Ubuntu)**:
    - Compiler **GCC and G++**: sudo apt install gcc g++ gdb build-essential
    - **CMake**: sudo apt install cmake
    - **Ninja**: sudo apt install ninja-build
    - **LaTeX**: sudo apt install texlive-latex-extra
    - [Anaconda](https://docs.anaconda.com/anaconda/install/linux/)

- C++ libraries:
  - [Qt 6.4](https://www.qt.io/download)
  <br> Please install it in C:/Qt.

 - Python libraries:
   - cython
   - mypy
   - *(nanobind)*
   - scikit-build-core
   - pyside6
   - qtconsole
   - ipython
   - pytest
   - numpy
   - pandas
   - larray
   - sphinx 
   - numpydoc
   - nbsphinx
   - sphinx_rtd_theme
   - myst-parser
   - build
   - twine

**note**: To install `larray`, you have to run:
```bash
conda install -c larray-project larray
```
For all other Python libraries:
```bash
conda install -c conda-forge <list of libraries>
```

(*) already installed on the Github Actions host-runner [windows-xxx](https://docs.github.com/en/actions/using-github-hosted-runners/about-github-hosted-runners#supported-runners-and-hardware-resources)

# Repository Structure

- **.github\workflows**: Script for Github Actions (Tests + Build GUI + Build LaTeX doc)
- **api**: C API
- **cpp_api**: C++ API
- **doc**: User documentation
- **gui**: Qt GUI
- **nsis**: NSIS Windows installer
- **pyiode**: Python module of IODE (Cython)
- **scr4**: Utility functions required to compile the C API
- **tests**: Testing directory

# Preparing Python IODE Build

To prepare the building of Python IODE, please create the following conda environments:
```bash
> conda config --add channels larray-project
> conda config --add channels conda-forge
> conda create --name py310 python=3.10 numpy pandas larray cython pytest scikit-build-core (nanobind)
> conda create --name py311 python=3.11 numpy pandas larray cython pytest scikit-build-core (nanobind)
> conda create --name py312 python=3.12 numpy pandas larray cython pytest scikit-build-core (nanobind)
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
- `iode_scr4`        -> C library providing many of the utility functions used in the C API.
- `iode_c_api`       -> Core API of IODE (in pure C).
- `iode_cpp_api`     -> C++ classes that wrap IODE C structure (used in the GUI Qt part).
- `iode_gui`         -> Graphical user interface (GUI) based on Qt (C++)
- `iode_generate_ui_python_files` -> Generates Python GUI scripts from Qt Designer files *.ui + Qt resource file *.qrc
- `iode_gui_super`   -> Generates iode_gui_super.pyd (Python module that wraps the C API 'super' functions)
- `nsis`             -> Builds a Windows Installer for the users.
- `test_c_api`       -> Builds the tests for the C API (based on Google Test).
- `test_cpp_api`     -> Builds the tests for the C++ classes (based on Google Test).
- `iode_doc`         -> Builds the user documentation (using Python Sphinx)
- `keyboard_shortcuts` -> Builds the PDF referencing all the keyboard shortcuts in the GUI (required LaTeX).

To build the Python module for IODE (Cython), please:
1. open a Developer Command Prompt (Windows)
2. go to the root of the project
2. activate the conda environment with the Python version for which you want to create an iode package 
```bash
(...)iode> conda activate py312
```
4. run the Batch script makepy.bat
```bash
(py312) (...)iode> makepy.bat
```

# Testing Project

## C And C++ API

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

## Python API
To test the Python API:

You first have to build it and install it locally.
In a console, go to root directory of your local `iode` project.
Then type:
```bash
root_dir_iode> cd pyiode
pyiode> pip install .
```
Note: you must have cython, numpy, pandas, larray, scikit-build-core and pytest installed.

Finally, return to the root project directory and type `pytest` :
```bash
pyiode> cd ..
root_dir_iode> pytest
```

**Note**: to only create the wheel file, type:
```bash
pyiode> pip wheel .
```
The, to install the wheel file, type:
```bash
pyiode> pip install <iode-wheel-file>.whl
```

# Python API

## stub files

To help IDE like PyCharm to understand the Python API of `iode`, 
stub files *.pyi must be generated and added to installed package.

The stub files are generated using the tool `stubgen` from the package `mypy`.
They are automatically generated when the CMake target `iode_cython` is built.

See pyiode/CMakeLists.txt and generate_stub_files.py for more details.

## Generating distribution archives

To create a source distribution (SDist) of the Python API, you have to first install 
the `build` package:
```bash
root_dir_iode> conda install build
```

Then, go to the `pyiode` sub-directory:
```bash
root_dir_iode> cd pyiode
pyiode>
```

To create the [SDist tarfile](https://scikit-build-core.readthedocs.io/en/latest/build.html#sdist) 
(with all the code required to build the project, along with a little bit of metadata), type:
```bash
pyiode> python -m build --sdist
```

Then, to create the [Wheel file](https://scikit-build-core.readthedocs.io/en/latest/build.html#wheel) (a ZIP file containing the compiled Python package), type:
```bash
pyiode> python -m build --wheel
```

You can combine the two above process by typing:
```bash
pyiode> python -m build --sdist --wheel
```

## Testing the distribution archives

Once the SDist tarfile and Wheel files have been generated, the `iode` package can be 
tested by installing it locally:
```bash
pyiode> pip install sdist/<iode-weel-file>.
```

Then, run *pytest* from the root directory:
```bash
pyiode> cd ..
root_dir_iode> pytest
```

## Uploading the distribution archives to TestPyPI

See explanation [here](https://packaging.python.org/en/latest/tutorials/packaging-projects/#uploading-the-distribution-archives).

The first thing you'll need to do is register an account on [TestPyPI](https://test.pypi.org/account/register/). 
For more details, see [Using TestPyPI](https://packaging.python.org/en/latest/guides/using-testpypi/).

To securely upload your project, you'll need a PyPI [API token](https://test.pypi.org/help/#apitoken). 
Create one at https://test.pypi.org/manage/account/#api-tokens, setting the "Scope" to "Entire account". 
Don't close the page until you have copied and saved the token — you won't see that token again.

Now that you are registered, you can use twine to upload the distribution packages.
To install the `twine` package, type:
```bash
root_dir_iode> conda install twine
```

Once installed, run Twine to upload all of the archives under dist:
```bash
root_dir_iode> twine upload --repository testpypi dist/*
```
You will be prompted for a username and password. For the username, use `__token__`. 
For the password, use the token value, including the `pypi-` prefix.

```bash
Uploading distributions to https://test.pypi.org/legacy/
Enter your username: __token__
Uploading example_package_YOUR_USERNAME_HERE-0.0.1-py3-none-any.whl
100% ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ 8.2/8.2 kB • 00:01 • ?
Uploading example_package_YOUR_USERNAME_HERE-0.0.1.tar.gz
100% ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ 6.8/6.8 kB • 00:00 • ?
```

You can use pip to install your package and verify that it works
```bash
root_dir_iode> pip install --index-url https://test.pypi.org/simple/ --no-deps <project_name_from_pyproject.toml>
```

## Uploading the distribution archives to PyPI

See explanation [here](https://packaging.python.org/en/latest/tutorials/packaging-projects/#next-steps).

1. Register an account on https://pypi.org.
2. Use 
```bash
root_dir_iode> twine upload dist/* 
```
to upload your package and enter your credentials for the account you registered on PyPI.
3. Install your package from the PyPI using 
```bash
root_dir_iode> pip install <project_name_from_pyproject.toml>
```

# GUI (Python)

Before to work on any issue related to the GUI (Python), you have to build the CMake target `iode_generate_ui_python_files` and `iode_gui_super`.
The `iode_generate_ui_python_files target` generates the Python scripts from the Qt Designer files *.ui and the Qt resource files *.qrc. 
The `iode_gui_super` generates the iode_gui_super.pyd file that wraps the C API 'super' functions.

For rapid testing, you may run the script `gui/run_test_gui.py`.

## Generating distribution archives

To create the [SDist tarfile](https://scikit-build-core.readthedocs.io/en/latest/build.html#sdist), type:
```bash
root_dir_iode> cd gui
gui> python3 -m build --sdist
```

Then, to create the [Wheel file](https://scikit-build-core.readthedocs.io/en/latest/build.html#wheel), type:
```bash
gui> python -m build --wheel
```

You can combine the two above process by typing:
```bash
gui> python -m build --sdist --wheel
```
## Testing the distribution archives

Once the SDist tarfile and Wheel files have been generated, the `iode-gui` package can be 
tested by installing it locally:
```bash
gui> pip install sdist/<iode-gui-weel-file>.
```

Then, run *iode-gui* from any terminal:
```bash
root_dir_iode> iode-gui
```
which should open a window with the GUI of IODE.

# Working On An Issue

To work on (an) issue(s), create a new local branch from the remote origin/master branch.
The name of the branch must follow the template: \<your initials\>\_\<keyword\>\_\<rest\> 
where `keyword` is:
- `gui` when working on the graphical user interface
- `python` when working on the Python binding for IODE
- `doc` when working on the user of dev documentation
- `nsis` when working on the Windows Installer and Updater
- `release` when preparing a new release 

Each keyword triggers a specific job in the defined Github Actions workflows (see below).
If none of the keywords above is used, the C and C++ API will be built and tested.

**IMPORTANT**: Do not forget to update the change log file of the current version. 
The change log files for each version are located in the directory doc/changes.

# Create A Release

## Before the release

1. Create a local branch based on `origin master` with the name containing the keyword `release`.
2. Replace "In development" in the last change log file (directory doc/changelog/versions) by "Released on `<current date>`".
3. Commit your changes (the commit message should be something like `release <tag> on <current date>`)
4. Create an annotated tag on the new commit.
   The name of the tag is the name of the last change log file in doc/changelog/versions (`v<major>.<minor>.<patch>(-beta.<beta>)`).
   The annotation should be something like `RELEASE: release <tag> on <current date>`.

## Push The New Tag To Github

1. Push the tag to Github: git push origin <tag_name>.
2. Check that the pushed tag has triggered the Workflow `github-actions-release.yml` in the Github Actions. Wait until the end of the `build_and_release` job. 
3. Check on Github if the new release has been created.

## After the release

1. In the directory doc/changelog/versions, create a copy of the template.rst.inc file and named it as <next_version_tag>.rst.inc.
2. Add the line `.. include:: ./changes/<next_version_tag>.rst.inc` to the changes.rst file in the directory doc/changelog.
3. Commit the changes.
4. Update all references MAJOR_VERSION.MINOR_VERSION everywhere in the project (in iode.h, documentation, ...)
5. Commit the changes.
6. Create a Pull Request and push to master.

# Github Actions

The Github Actions workflows for IODE are defined in the directory ./.github/workflows.

The sub-directory ./.github/actions contains the local actions (used in workflows).

The jobs declared in ./.github/workflows/github-actions-debug.yml are triggered according to keywords present 
in the name of the pushed branch:
- `build_and_test_api`  -> NOT triggered if the branch name contains the word `gui`, `python`, `doc` or `nsis`
- `test_sanitize`       -> NOT triggered if the branch name contains the word `gui`, `python`, `doc` or `nsis`
- `build_gui`           -> triggered if the branch name contains the word `gui`
- `build_python`        -> triggered if the branch name contains either the word `python` or `nsis`
- `build_doc`           -> triggered if the branch name contains either the word `doc` or `nsis`
- `build_nsis`          -> triggered if the branch name contains either the word `nsis` or `release`

# Troubleshooting

## VS Code - IntelliSense

The IntelliSense feature (= autocomplete for C/C++) of VS Code may not be able to find where the headers files of the 
std and Qt libraries are located. 

First, add a `c_cpp_properties.json` file as indicated in the documentation for 
[developing with the C/C++ compiler MSVC with VS Code](https://code.visualstudio.com/docs/cpp/config-msvc#_cc-configurations).

Then, in the `c_cpp_properties.json` file:

1. add the following content for `includePath`:
```
	"includePath": [
        "${workspaceFolder}/**",
        "${workspaceFolder}",
        "${workspaceFolder}/scr4",
        "C:/Qt/6.4.2/msvc2019_64/include",
        "C:/Qt/6.4.2/msvc2019_64/include/QtCore",
        "C:/Qt/6.4.2/msvc2019_64/include/QtGui",
        "C:/Qt/6.4.2/msvc2019_64/include/QtWidgets",
        "C:/Qt/6.4.2/msvc2019_64/include/QtCharts",
        "C:/Qt/6.4.2/msvc2019_64/include/QtPrintSupport"
  ],
```
You may have to adapt the path to the Qt libraries depending on where those libraries are installed on your computer.

2. Update the value of `configurationProvider` and `compilerPath`. To find the path to your MSVC compiler, open a 
`Developer Command Prompt` and type:
```
> where cl
C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.38.33130\bin\Hostx64\x64\cl.exe
```

3. Restart VS Code
