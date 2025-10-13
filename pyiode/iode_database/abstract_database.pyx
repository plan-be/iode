import warnings
from pathlib import Path
from collections.abc import Iterable
from typing import Union, Tuple, List, Optional, Any

import pandas as pd

from cython.operator cimport dereference
from libc.string cimport strlen
from libcpp.string cimport string
from pyiode.iode_cython cimport SCR_free_tbl, SCR_tbl_size, SCR_free
from pyiode.iode_database.cpp_api_database cimport KDBAbstract as CKDBAbstract
from pyiode.iode_database.cpp_api_database cimport KCPTR, KIPTR, KLPTR, KVPTR
from pyiode.iode_database.cpp_api_database cimport K_NBDEC
from pyiode.iode_database.cpp_api_database cimport error_manager
from pyiode.iode_database.cpp_api_database cimport KDB, K_expand, K_expand_kdb
from pyiode.iode_database.cpp_api_database cimport B_DataCompare
from pyiode.iode_database.cpp_api_database cimport B_DataPattern
from pyiode.iode_database.cpp_api_database cimport B_PrintNbDec
from pyiode.iode_database.cpp_api_database cimport B_PrintDest
from pyiode.iode_database.cpp_api_database cimport B_PrintObjDef
from pyiode.iode_database.cpp_api_database cimport W_dest, W_flush, W_close


cdef class CythonIodeDatabase:
    cdef CKDBAbstract* abstract_db_ptr 

    def __cinit__(self):
        # the C++ attribute abstract_db_ptr is initialized in the subclasses
        pass

    def __dealloc__(self):
        # the C++ attribute abstract_db_ptr is destroyed in the subclasses
        pass

    def get_is_global_workspace(self) -> bool:
        return self.abstract_db_ptr.is_global_database()

    def get_is_detached(self) -> bool:
        return self.abstract_db_ptr.is_local_database()

    def get_iode_type(self) -> IodeType:
        int_iode_type: int = self.abstract_db_ptr.get_iode_type()
        return IodeType(int_iode_type)

    def get_filename(self) -> str:
        return self.abstract_db_ptr.get_filename().decode()

    def set_filename(self, value: str):
        self.abstract_db_ptr.set_filename(value.encode())

    def get_description(self) -> str:
        return self.abstract_db_ptr.get_description().decode()

    def set_description(self, value: str):
        self.abstract_db_ptr.set_description(value.encode())

    def _get_print_nb_decimals(self) -> int:
        return K_NBDEC

    def _set_print_nb_decimals(self, value: int):
        cdef bytes b_value = str(value).encode()
        cdef char* c_value = b_value
        B_PrintNbDec(c_value)

    def get_position(self, name: str) -> int:
        return self.abstract_db_ptr.get_position(name.encode())

    def get_name(self, pos: int) -> str:
        return self.abstract_db_ptr.get_name(pos).decode()

    def get_names(self, pattern: str, filepath: str=None) -> str:
        cdef int i_iode_type = self.abstract_db_ptr.get_iode_type()
        cdef int _all = ord('*')
        cdef KDB* kdb_ptr = NULL
        cdef size_t length = 0
        cdef char* c_list = NULL
        
        if filepath is not None:
            c_list = K_expand(i_iode_type, filepath.encode(), pattern.encode(), _all)
        else:
            kdb_ptr = self.abstract_db_ptr.get_database()
            if kdb_ptr is not NULL:
                c_list = K_expand_kdb(kdb_ptr, i_iode_type, pattern.encode(), _all)  
        
        if c_list is NULL: 
            raise RuntimeError(f"Could not get names using the pattern {pattern}")
        
        length = strlen(c_list)
        if length == 0:
            SCR_free(c_list)
            return ''

        b_list: bytes = c_list[:length]
        SCR_free(c_list)

        return b_list.decode()

    def property_names(self) -> List[str]:
        cdef string s = string(b'')
        return [name.decode() for name in self.abstract_db_ptr.get_names(s, <bint>True)]

    def rename(self, old_name: str, new_name: str) -> int:
        return self.abstract_db_ptr.rename(old_name.encode(), new_name.encode())

    def remove(self, names: List[str]):
        for name in names:
            self.abstract_db_ptr.remove(name.encode())

    def compare(self, args: str, i_iode_type: int):
        res = B_DataCompare(args.encode(), i_iode_type)
        if res != 0:
            error_manager.display_last_error()

    def merge(self, cython_other: CythonIodeDatabase, overwrite: bool=True):        
        cdef CKDBAbstract* other_db_ptr = cython_other.abstract_db_ptr
        self.abstract_db_ptr.merge(dereference(other_db_ptr), <bint>overwrite)

    def merge_from(self, input_file: str):
        self.abstract_db_ptr.merge_from(input_file.encode())

    def search(self, pattern: str, word: bool=True, case_sensitive: bool=True, in_name: bool=True, 
               in_formula: bool=True, in_text: bool=True, list_result: str='_RES') -> List[str]:
        cdef string s_pattern = pattern.encode('utf-8')
        cdef string s_list_result = list_result.encode('utf-8')
        return [name_other.decode() for name_other in self.abstract_db_ptr.search(s_pattern, 
                <bint>word, <bint>case_sensitive, <bint>in_name, <bint>in_formula, <bint>in_text, s_list_result)]

    def get_names_from_pattern(self, list_name: str, pattern: str, xdim: str, ydim: str) -> bool:
        cdef bytes b_arg
        cdef char* c_arg
        cdef int int_type = self.get_iode_type().value
        cdef int res

        arg: str = f"{list_name} {pattern} {xdim}"
        if ydim:
            arg += " " + ydim
        b_arg = arg.encode('utf-8')
        c_arg = b_arg

        res = B_DataPattern(c_arg, int_type)
        return res == 0

    def print_to_file(self, filepath: str, names: str, format: str=None):
        cdef int res
        cdef char* c_arg = NULL
        cdef bytes b_arg = b''
        cdef int int_type = self.get_iode_type().value

        arg = filepath
        if format is not None:
            arg += ' ' + format
        # NOTE: Cython cannot convert a Python string to a C string directly.
        b_arg = arg.encode('utf-8')
        c_arg = b_arg
        
        res = B_PrintDest(c_arg)
        if res != 0:
            raise RuntimeError(f"The filepath '{filepath}' is not valid")

        res = B_PrintObjDef(names.encode('utf-8'), int_type)
        if res != 0:
            raise RuntimeError(f"Error while printing objects to file '{filepath}'")
        
        # write and close output file
        W_close()

    def save(self, filepath: str, compress: bool):
        self.abstract_db_ptr.save(filepath.encode(), <bint>compress)

    def clear(self):
        self.abstract_db_ptr.clear()

    def size(self) -> int:
        return self.abstract_db_ptr.size()

    def contains(self, item) -> bool:
        return self.abstract_db_ptr.contains(item.encode())

    def remove_objects(self, names: List[str]):
        for name in names:
            self.abstract_db_ptr.remove(name.encode())

    def remove_entries(self, names: List[str]):
        for name in names:
            self.abstract_db_ptr.remove_entry(name.encode())
