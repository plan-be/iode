from collections.abc import Iterable
from typing import Union, Tuple, List, Optional, Any
if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

cimport cython
from cython.operator cimport dereference
from pyiode.iode_database.cpp_api_database cimport hash_value
from pyiode.iode_database.cpp_api_database cimport KDBLists as CKDBLists
from pyiode.iode_database.cpp_api_database cimport Lists as cpp_global_lists
from pyiode.iode_database.cpp_api_database cimport KCPTR, KIPTR, KLPTR, KVPTR

import pandas as pd
from iode.util import split_list


cdef class Lists(IodeDatabase):
    cdef bint ptr_owner
    cdef CKDBLists* database_ptr

    def __cinit__(self, filepath: str=None) -> Lists:
        self.database_ptr = NULL
        self.abstract_db_ptr = NULL
        self.ptr_owner = False

    def __dealloc__(self):
        if self.ptr_owner and self.database_ptr is not NULL:
            del self.database_ptr
            self.database_ptr = NULL

    @staticmethod
    cdef Lists _from_ptr(CKDBLists* database_ptr = NULL, bint owner=False):
        # call to __new__() that bypasses the __init__() constructor.
        cdef Lists wrapper = Lists.__new__(Lists)
        if database_ptr is not NULL:
            wrapper.ptr_owner = owner
            wrapper.database_ptr = database_ptr
            wrapper.abstract_db_ptr = database_ptr
        else:
            wrapper.ptr_owner = False
            wrapper.database_ptr = &cpp_global_lists
            wrapper.abstract_db_ptr = &cpp_global_lists
        return wrapper

    @staticmethod
    def __init_instance(instance: Lists) -> Self:
        instance.ptr_owner = False
        instance.database_ptr = &cpp_global_lists
        instance.abstract_db_ptr = &cpp_global_lists
        return instance

    def _load(self, filepath: str):
        cdef CKDBLists* kdb = new CKDBLists(filepath.encode())
        del kdb

    def _subset(self, pattern: str, copy: bool) -> Lists:
        subset_db: Lists = self._new_instance()
        subset_db.database_ptr = subset_db.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        return subset_db

    def _get_object(self, key: Union[str, int]) -> List[str]:
        if isinstance(key, int):
            str_list = self.database_ptr.get(<int>key).decode()
        else:
            key = key.strip()
            str_list = self.database_ptr.get(<string>(key.encode())).decode()
        return split_list(str_list)

    def _set_object(self, key: Union[str, int], value: Union[str, List[str]]):
        if isinstance(value, str):
            value = value.strip()
            value = split_list(value)
        else:
            value = [item.strip() for item in value]
        # normalize the IODE list
        value = ';'.join(value)

        if isinstance(key, int):
            self.database_ptr.update(<int>key, <string>(value.encode()))
        else:
            key = key.strip()
            if self.database_ptr.contains(key.encode()):
                self.database_ptr.update(<string>(key.encode()), <string>(value.encode()))
            else:
                self.database_ptr.add(key.encode(), value.encode())

    def copy_from(self, input_files: Union[str, List[str]], names: Union[str, List[str]]='*'):
        if not (self.is_global_workspace or self.is_detached):
            raise RuntimeError("Cannot call 'copy_from' method on a subset of a workspace")
        input_files, names = self._copy_from(input_files, names)
        self.database_ptr.copy_from(input_files.encode(), names.encode())

    def __hash__(self) -> int:
        return hash_value(dereference(self.database_ptr))


