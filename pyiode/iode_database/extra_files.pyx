from pathlib import Path
from typing import Union, List
from pyiode.iode_database.cpp_api_database cimport load_reference_kdb, clear_all_reference_kdbs


def cython_load_extra_files(extra_files: Union[str, Path, List[Union[str, Path]]]) -> List[Path]:    
    for i, extra_file in enumerate(extra_files):
        load_reference_kdb(i + 2, str(extra_file).encode())
    return extra_files


def cython_reset_extra_files():
    clear_all_reference_kdbs()
