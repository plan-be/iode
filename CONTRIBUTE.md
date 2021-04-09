# Requirements

- Compiler:
  - Visual Studio (2019)
  - Embarcadero
- Tools:
  - NSIS
  - miniconda or anaconda (for managing Python)
- Others:
  - Python 3.6, 3.7 and 3.8
  - cython conda package
  - larray conda package

# Repository Structure

- **api**: core functions of IODE
- **builder**: for building IODE
- **cmd**: command line version of IODE
- **doc**: documentation
- **dos**: user interface for IODE (old fashioned)
- **goodies**: syntax files for code highligthing in notepad++ and textpad
- **iodecom**: module for interacting with Excel, APL via the COM-technology
- **pyiode**: Python module of IODE
- **scr4**: utility functions
- **tests**: testing directory

# Building Project

The project is built using the script `remakeiode.bat` located in the /builder directory.

The `remakeiode.bat` script allows 5 optional arguments:
- *-objs*: do not recompile the source files (only create .lib and .exe)
- *-cmd*: build only the command line version of IODE
- *-man*: compile the IODE documentation
- *-py*: build pyiode only
- *-bc*: use Broland compiler instead of vc64 (Visula Studio 64bits)
- *-vc32*: use vc32 (Visual Studio 32bits) compiler instead of vc64 (Visual Studio 64bits)

Requirements:
- **full version**: You need to have Embarcadero, Visual Studio, NSIS, Python 3.6+, conda and Cython installed
- **command line version**: requires to only have Visual Studio installed
- **Python version**: requires Python 3.6+, (mini/ana)conda, cython and separated py36, py37 and base (Python 3.8) conda environments with larray package installed 

# Qt

Note: Since version 6, [Qt is build using CMake](https://www.qt.io/blog/qt-and-cmake-the-past-the-present-and-the-future).

## Warning

- Since version 6, Qt does no longer support 32 bit architecture. Only 64 bit.

## Installing Qt

- Download the Qt installer from this [link](https://www.qt.io/download-qt-installer?hsCtaTracking=99d9dd4f-5681-48d2-b096-470725510d34%7C074ddad0-fdef-4e53-8aa8-5e8a876d6ab4)
- Run the Qt installer and follow the steps

## Visual Studio plugin

- Qt VS Tools: https://doc.qt.io/qtvstools/qtvstools-getting-started.html

## Qt for windows
 
- General: https://doc.qt.io/qt-6/windows.html
- Deployment : https://doc-snapshots.qt.io/qt6-dev/windows-deployment.html

## CMake

- Getting Started: https://doc.qt.io/qt-6/cmake-get-started.html
- Porting Qmake .pro file to CMake: https://wiki.qt.io/CMake_Port/Porting_Guide