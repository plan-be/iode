from .iode_cython import IodeType, IodeFileType, TableLang, EqMethod, EqTest, _get_file_types

from typing import List, Tuple


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
