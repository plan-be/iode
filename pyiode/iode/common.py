from .iode_cython import IodeType, IodeFileType, TableLang, EqMethod, EqTest, _get_file_types

from typing import List, Tuple
from enum import IntEnum


PRINT_DEF_TABLES: Tuple[str, str] = (
    "Print full definitions",
    "Print only titles", 
    "Print computed tables")

class PrintTablesAs(IntEnum):
    FULL = 0
    TITLES = 1
    COMPUTED = 2

PRINT_DEF_EQUATIONS: Tuple[str, str, str] = (
    "Equation only",
    "Equation + comments",
    "All (equation, comment, estimation)")

class PrintEquationsAs(IntEnum):
    EQ_ONLY = 0
    EQ_COMMENTS = 1
    EQ_COMMENTS_ESTIMATION = 2

IODE_DATABASE_TYPE_NAMES: List[str] = [member.name.title() for member in IodeType]

IODE_FILE_TYPE_NAMES: List[str] = [member.name.title() for member in IodeFileType]

IODE_LANGUAGES_LIST: List[str] = [member.name.title() for member in TableLang]

EQUATION_METHODS_LIST: List[str] = [member.name.title() for member in EqMethod]

EQ_TEST_NAMES: Tuple[str, ...] = ("Correlation", "St dev of residuals",
                                  "Mean of YOBS", "∑ Residuals²", "Standard error",
                                  "Standard error in %", "F-stat", "R²",
                                  "R² adjusted", "Durbin-Watson", "Log Likelihood")

SIMULATION_INITIALIZATION_METHODS: Tuple[str, ...] = (
    "Y := Y[-1], if Y null or NA",         
    "Y := Y[-1], always",                  
    "Y := extrapolation, if Y null or NA", 
    "Y := extrapolation, always",          
    "Y unchanged",                         
    "Y := Y[-1], if Y = NA",               
    "Y := extrapolation, if Y = NA")

SIMULATION_SORT_ALGORITHMS: Tuple[str, ...] = (
    "Connex compon. decomposition",
    "Connex compon. + Triangulation",
    "None")

PRINT_FORMATS: Tuple[str, ...] = (
    "A2M file (IODE ...)", 
    "RTF file (Word, ...)", 
    "HTML file (Internet)",
    "MIF file (Frame Maker)", 
    "CSV file (Excel, ....)", 
    "No format")

IMPORT_FORMATS: Tuple[str, ...] = (
    "Ascii", 
    "Rotated_Ascii", 
    "DIF",
    "BISTEL", 
    "NIS", 
    "GEM", 
    "PRN", 
    "TXT_Belgostat")

EXPORT_FORMATS: Tuple[str, ...] = (
    "CSV", 
    "DIF", 
    "WKS", 
    "TSP", 
    "Reverse CSV")

GRAPHS_COLORS: Tuple[str, ...] = (
    "Blue", 
    "Yellow", 
    "Black", 
    "White", 
    "Red", 
    "Green", 
    "Cyan", 
    "Magenta")

IODE_FONTS: Tuple[str, ...] = (
    "Helvetica", 
    "Times", 
    "Courier", 
    "Bookman", 
    "Palatino", 
    "Trebuchet MS")

class FileType:
    def __init__(self, name: str, extensions: List[str]):
        self._name: str = name
        self._extensions: List[str] = extensions

    @property
    def name(self) -> str:
        return self._name

    @property
    def extensions(self) -> List[str]:
        return self._extensions

    def __str__(self) -> str:
        return f"name='{self.name}', extensions={self.extensions}"

    def __repr__(self) -> str:
       return f"FileType(name='{self.name}', extensions={self.extensions})"


IODE_FILE_TYPES: List[FileType] = [FileType(name, extensions) for name, extensions in _get_file_types()]
