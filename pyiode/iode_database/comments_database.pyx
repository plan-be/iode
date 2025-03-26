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


cdef class Comments(IodeDatabase):
    cdef bint ptr_owner
    cdef CKDBComments* database_ptr

    def __cinit__(self, filepath: str=None) -> Comments:
        self.database_ptr = NULL
        self.abstract_db_ptr = NULL
        self.ptr_owner = False

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

    @staticmethod
    def __init_instance(instance: Comments) -> Self:
        instance.ptr_owner = False
        instance.database_ptr = &cpp_global_comments
        instance.abstract_db_ptr = &cpp_global_comments
        return instance

    def _load(self, filepath: str):
        cdef CKDBComments* kdb = new CKDBComments(filepath.encode())
        del kdb

    def _subset(self, pattern: str, copy: bool) -> Comments:
        subset_db: Comments = self._new_instance()
        subset_db.database_ptr = subset_db.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        return subset_db

    def _get_object(self, key: Union[str, int]) -> str:
        if isinstance(key, int):
            return self.database_ptr.get(<int>key).decode()
        else:
            key = key.strip()
            return self.database_ptr.get(<string>(key.encode())).decode()

    def _set_object(self, key: Union[str, int], value: str):
        value = value.strip()
        if isinstance(key, int):
            self.database_ptr.update(<int>key, <string>(value.encode()))
        else:
            if self.database_ptr.contains(key.encode()):
                self.database_ptr.update(<string>(key.encode()), <string>(value.encode()))
            else:
                self.database_ptr.add(key.encode(), value.encode())

    def copy_from(self, input_files: Union[str, List[str]], names: Union[str, List[str]]='*'):
        if not (self.is_global_workspace or self.is_detached):
            raise RuntimeError("Cannot call 'copy_from' method on a subset of a workspace")
        input_files, names = self._copy_from(input_files, names)
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


