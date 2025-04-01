from collections.abc import Iterable
from typing import Union, Tuple, List, Optional, Any
if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

cimport cython
from cython.operator cimport dereference
from pyiode.iode_database.cpp_api_database cimport hash_value
from pyiode.iode_database.cpp_api_database cimport KDBComments as CKDBComments
from pyiode.iode_database.cpp_api_database cimport Comments as cpp_global_comments
from pyiode.iode_database.cpp_api_database cimport KCPTR, KIPTR, KLPTR, KVPTR
from pyiode.iode_database.cpp_api_database cimport B_FileImportCmt

import pandas as pd
from iode.util import check_filepath


cdef class Comments(CythonIodeDatabase):
    cdef bint ptr_owner
    cdef CKDBComments* database_ptr

    def __cinit__(self, filepath: str=None) -> Comments:
        self.ptr_owner = False
        self.database_ptr = &cpp_global_comments
        self.abstract_db_ptr = &cpp_global_comments

    def __dealloc__(self):
        if self.ptr_owner and self.database_ptr is not NULL:
            del self.database_ptr
            self.database_ptr = NULL

    @staticmethod
    cdef Comments _from_ptr(CKDBComments* database_ptr = NULL, bint owner=False):
        # call to __new__() that bypasses the __init__() constructor.
        cdef Comments wrapper = Comments.__new__(Comments)
        if database_ptr is not NULL:
            wrapper.ptr_owner = owner
            wrapper.database_ptr = database_ptr
            wrapper.abstract_db_ptr = database_ptr
        else:
            wrapper.ptr_owner = False
            wrapper.database_ptr = &cpp_global_comments
            wrapper.abstract_db_ptr = &cpp_global_comments
        return wrapper

    def _load(self, filepath: str):
        cdef CKDBComments* kdb = new CKDBComments(filepath.encode())
        del kdb

    def initialize_subset(self, cython_instance: Comments, pattern: str, copy: bool) -> Comments:
        cython_instance.database_ptr = cython_instance.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        return cython_instance

    def _get_object(self, name: str) -> str:
        name = name.strip()
        return self.database_ptr.get(name.encode()).decode()

    def _set_object(self, name: str, value: str):
        name = name.strip()
        if self.database_ptr.contains(name.encode()):
            self.database_ptr.update(name.encode(), <string>(value.encode()))
        else:
            self.database_ptr.add(name.encode(), value.encode())

    def copy_from(self, input_files: str, names: str='*'):
        self.database_ptr.copy_from(input_files.encode(), names.encode())

    @classmethod
    def convert_file(cls, input_file: Union[str, Path], input_format: Union[str, ImportFormats], save_file: Union[str, Path], rule_file: Union[str, Path], lang: Union[str, TableLang]=TableLang.ENGLISH, debug_file: Union[str, Path]=None) -> None:
        # $FileImportCmt format rule_file input_file language [debug_file]
        input_file = check_filepath(input_file, IodeFileType.FILE_ANY, file_must_exist=True)

        _c_import_formats: str = ''.join([item.name[0] for item in list(ImportFormats)])
        if isinstance(input_format, ImportFormats):
            input_format = input_format.name[0]
        if input_format not in _c_import_formats:
            raise ValueError(f"Invalid input format '{input_format}'. "
                             f"Possible values are: {_c_import_formats}")
        
        save_file = check_filepath(save_file, IodeFileType.FILE_COMMENTS, file_must_exist=False)
        rule_file = check_filepath(rule_file, IodeFileType.FILE_ANY, file_must_exist=True)

        _c_table_langs: str = ''.join([item.name[0] for item in list(TableLang)])
        if isinstance(lang, TableLang):
            lang = lang.name[0]
        if lang not in _c_table_langs:
            raise ValueError(f"Invalid language '{lang}'. " 
                             f"Possible values are: {_c_table_langs}") 

        # $FileImportCmt format rule infile outfile language [trace]
        args = f"{input_format} {rule_file} {input_file} {save_file} {lang}"
        
        if debug_file:
            debug_file = check_filepath(debug_file, IodeFileType.FILE_LOG, file_must_exist=False)
            args += " " + debug_file

        res = B_FileImportCmt(args.encode('utf-8'))
        if res < 0:
            raise RuntimeError(f"Cannot import comments from file '{input_file}'")

    def __hash__(self) -> int:
        return hash_value(dereference(self.database_ptr))


