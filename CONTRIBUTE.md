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
- *-vc64*: use vc64 (Visual Studio) compiler instead of bc (Borland)

Requirements:
- **full version**: You need to have Embarcadero, Visual Studio, NSIS, Python 3.6+, conda and Cython installed
- **command line version**: requires to only have Visual Studio installed
- **Python version**: requires Python 3.6+, (mini/ana)conda, cython and separated py36, py37 and base (Python 3.8) conda environments with larray package installed 
