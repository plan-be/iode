# distutils: language = c++

from pathlib import Path
import warnings

from collections.abc import Iterable
from typing import Union, Tuple, List, Optional, Any
import sys
if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

from iode.common import IODE_FILE_TYPES
from iode.util import join_lines, table2str, JUSTIFY, check_filepath, split_list

from cython.operator cimport dereference
from libc.string cimport strlen

from pyiode.common cimport IodeType
from pyiode.iode_cython cimport SCR_free_tbl, SCR_tbl_size, SCR_free
from pyiode.iode_database.cpp_api_database cimport KDBAbstract as CKDBAbstract
from pyiode.iode_database.cpp_api_database cimport KCPTR, KIPTR, KLPTR, KVPTR
from pyiode.iode_database.cpp_api_database cimport KDB, K_expand, K_expand_kdb
from pyiode.iode_database.cpp_api_database cimport B_DataCompare
from pyiode.iode_database.cpp_api_database cimport B_display_last_error


# WARNING: remember:
#          - private Python methods start with __ are NOT inherited in the derived class
#          - protected Pythod methods start with _ and are inherited in the derived class 
#            (and so can be overidde)

class PositionalIndexer:
    def __init__(self, database):
        self.database = database
    
    def _check_index(self, index: int) -> int:
        if index < 0:
            index += len(self.database)
        if not (0 <= index < len(self.database)):
            raise IndexError(f"Index {index} out of range")
        return index

    def __getitem__(self, index: int):
        index = self._check_index(index)
        return self.database._get_object(index)
    
    def __setitem__(self, index: int, value):
        index = self._check_index(index)
        self.database._set_object(index, value)


# Define the Python wrapper class for KDBAbstract
cdef class _AbstractDatabase:
    cdef CKDBAbstract* abstract_db_ptr 

    # Constructor
    def __cinit__(self):
        # pointer *abstract_db_ptr is set in subclasses
        if isinstance(self.__class__, _AbstractDatabase):
            raise RuntimeError("Cannot instantiate an abstract class")

    # Destructor
    def __dealloc__(self):
        pass

    @property
    def is_global_workspace(self) -> bool:
        """
        Whether or not the present database represents the global IODE workspace.
        
        Returns
        -------
        bool

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> comments.is_global_workspace
        True
        >>> cmt_copy = comments.copy()
        >>> cmt_copy.is_global_workspace
        False
        >>> cmt_subset = comments["A*"]
        >>> cmt_subset.is_global_workspace
        False
        """
        return self.abstract_db_ptr.is_global_database()

    @property
    def is_detached(self) -> bool:
        """
        Whether or not any change made on the present database or subset will modify 
        the global IODE workspace.

        Returns
        -------
        bool

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> # 'comments' represents the global Comments workspace
        >>> comments.is_detached
        False
        >>> comments["ACAF"]
        'Ondernemingen: ontvangen kapitaaloverdrachten.'

        >>> # by default, making a selection will create a new database 
        >>> # object that is a 'view' of the global Comments workspace.
        >>> # Any change made on this 'view' (subset) will also modify 
        >>> # the global workspace.
        >>> cmt_subset = comments["A*"]
        >>> cmt_subset.is_detached
        False
        >>> cmt_subset["ACAF"] = "modified comment"
        >>> cmt_subset["ACAF"]
        'modified comment'
        >>> comments["ACAF"]
        'modified comment'
        >>> # adding a new comment to the subset will also add it 
        >>> # to the global workspace
        >>> cmt_subset["NEW_CMT"] = "new comment"
        >>> "NEW_CMT" in comments
        True
        >>> comments["NEW_CMT"]
        'new comment'
        >>> # removing a comment from the subset will also remove it 
        >>> # from the global workspace
        >>> del cmt_subset["NEW_CMT"]
        >>> "NEW_CMT" in comments
        False

        >>> # explicitly calling the copy method will create a new 
        >>> # detached database. Any change made on the copy will not 
        >>> # modify the global workspace.
        >>> cmt_copy = comments["A*"].copy()
        >>> cmt_copy.is_detached
        True
        >>> cmt_copy["AOUC"] = "modified comment"
        >>> cmt_copy["AOUC"]
        'modified comment'
        >>> comments["AOUC"]
        'Kost per eenheid produkt.'
        """
        return self.abstract_db_ptr.is_local_database()

    def _subset(self, pattern: str, copy: bool) -> Self:
        raise NotImplementedError()

    def copy(self, pattern: str=None) -> Self:
        """
        Create a new database instance in which each object is a *copy* of the original object 
        from the global IODE database. Any change made to the *copied database* (*subset*) will 
        not be applied to the global workspace. This can be useful for example if you want to 
        save previous values of scalars before estimating an equation or a block of equations and 
        then restore the original values if the estimated values are not satisfying.

        Parameters
        ----------
        pattern : str, optional
            If provided, the copied database will only contain the objects whose name matches the 
            provided pattern. By default (None), the copied database will contain all the objects 
            from the global IODE database. The pattern syntax is the same as the one used for the 
            `__getitem__` method. If the pattern is an empty string, the copied database will be 
            empty, creating a new *detached* database.
            Default to None.

        Returns
        -------
        Database
        
        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 

        Database subset

        >>> # without using copy(), any modification made on 
        >>> # the copy subset will also change the corresponding 
        >>> # IODE database
        >>> cmt_subset = comments["A*"]
        >>> cmt_subset.names
        ['ACAF', 'ACAG', 'AOUC', 'AQC']
        >>> # a) add a comment
        >>> cmt_subset["A_NEW"] = "New comment"
        >>> "A_NEW" in cmt_subset
        True
        >>> "A_NEW" in comments
        True
        >>> comments["A_NEW"]
        'New comment'
        >>> # b) modify a comment
        >>> cmt_subset["ACAF"] = "Modified Comment"
        >>> cmt_subset["ACAF"]
        'Modified Comment'
        >>> comments["ACAF"]
        'Modified Comment'
        >>> # c) delete a comment
        >>> del cmt_subset["ACAG"]
        >>> "ACAG" in cmt_subset
        False
        >>> "ACAG" in comments
        False

        Copied database subset

        >>> cmt_subset_copy = comments["B*"].copy()
        >>> cmt_subset_copy.names
        ['BENEF', 'BENEF_', 'BQY', 'BVY']
        >>> # or equivalently
        >>> cmt_subset_copy = comments.copy("B*")
        >>> cmt_subset_copy.names
        ['BENEF', 'BENEF_', 'BQY', 'BVY']
        >>> # by using copy(), any modification made on the copy subset 
        >>> # let the global workspace unchanged
        >>> # a) add a comment -> only added in the copied subset
        >>> cmt_subset_copy["B_NEW"] = "New Comment"
        >>> "B_NEW" in cmt_subset_copy
        True
        >>> "B_NEW" in comments
        False
        >>> # b) modify a comment -> only modified in the copied subset
        >>> cmt_subset_copy["BENEF"] = "Modified Comment"
        >>> cmt_subset_copy["BENEF"]
        'Modified Comment'
        >>> comments["BENEF"]
        'Ondernemingen: niet-uitgekeerde winsten.'
        >>> # c) delete a comment -> only deleted in the copied subset
        >>> del cmt_subset_copy["BENEF_"]
        >>> "BENEF_" in cmt_subset_copy
        False
        >>> "BENEF_" in comments
        True

        New detached database

        >>> # a new empty *detached* database can be created by passing 
        >>> # an empty string to the copy() method 
        >>> cmt_subset_detached = comments.copy("")
        >>> cmt_subset_detached.names
        []
        """
        if pattern is None:
            pattern = '*'
        return self._subset(pattern, copy=True)

    @property
    def iode_type(self) -> IodeType:
        r"""
        Return the object type of the current database.

        Returns
        -------
        IodeType

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> comments.iode_type
        <IodeType.COMMENTS: 0>
        """
        int_iode_type: int = self.abstract_db_ptr.get_iode_type()
        return IodeType(int_iode_type)

    @property
    def filename(self) -> str:
        r"""
        Return the filepath associated with the current database.

        Returns
        -------
        str

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments
        >>> from pathlib import Path
        >>> from os.path import relpath
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> filename = comments.filename
        >>> Path(filename).name
        'fun.cmt'
        >>> comments.filename = "new_filename.cmt"
        >>> filename = comments.filename
        >>> Path(filename).name
        'new_filename.cmt'
        """
        return self.abstract_db_ptr.get_filename().decode()

    @filename.setter
    def filename(self, value: str):
        self.abstract_db_ptr.set_filename(value.encode())

    @property
    def description(self) -> str:
        """
        Description of the current database.

        Parameters
        ----------
        value: str
            New description.
        
        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> comments.description = "test data from file 'fun.cmt'"
        >>> comments.description
        "test data from file 'fun.cmt'"
        """
        return self.abstract_db_ptr.get_description().decode()

    @description.setter
    def description(self, value: str):
        self.abstract_db_ptr.set_description(value.encode())

    def get_position(self, name: str) -> int:
        """
        Return the position of the IODE object with name `name` in the database.

        Parameters
        ----------
        name: str
            Name of the IODE object to search for in the database.
        
        Returns
        -------
        int

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> comments.get_position("ACAF")
        0
        """
        if name not in self:
            raise KeyError(f"'{name}' is not in the database.")
        return self.abstract_db_ptr.get_position(name.encode())

    def get_name(self, pos: int) -> str:
        """
        Return the name of the IODE object at position `pos` in the database.

        Parameters
        ----------
        pos: int
           Position of the object in the database.

        Returns
        -------
        str

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> comments.get_name(0)
        'ACAF'
        """
        if not (0 <= pos < len(self)):
            raise IndexError(f"Index {pos} is out of bounds for database of size {len(self)}")
        return self.abstract_db_ptr.get_name(pos).decode()

    def get_names(self, pattern: Union[str, List[str]]=None, filepath: Union[str, Path]=None) -> List[str]:
        """
        Returns the list of objects names given a pattern.
        If a file is provided, search for names in the file instead of the current database.

        Parameters
        ----------
        pattern: str or list(str), optional
            pattern to select a subset of objects. 
            For example, 'A*;*_' will select all objects for which the name starts 
            with 'A' or ends with '_'.
            Return all names contained in the database by default.
        filepath: str or Path, optional
            Path to the file to search for names. 
            Search in the current database by default.

        Returns
        -------
        list(str)
        
        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> comments.get_names("A*;*_")         # doctest: +NORMALIZE_WHITESPACE
        ['ACAF', 'ACAG', 'AOUC', 'AQC', 'BENEF_', 'GOSH_', 
        'IDH_', 'PAFF_', 'PC_', 'PFI_', 'QAFF_', 'QAF_', 
        'QAI_', 'QAT_', 'QBBPPOT_', 'QC_', 'QQMAB_', 'QS_', 
        'Q_', 'TFPHP_', 'VAFF_', 'VAI_', 'VAT_', 'VC_', 'VS_', 
        'WBF_', 'WBU_', 'WCF_', 'WCR1_', 'WCR2_', 'WIND_', 
        'WNF_', 'YDH_', 'ZZ_']
        >>> # or equivalently
        >>> comments.get_names(["A*", "*_"])    # doctest: +NORMALIZE_WHITESPACE
        ['ACAF', 'ACAG', 'AOUC', 'AQC', 'BENEF_', 'GOSH_', 
        'IDH_', 'PAFF_', 'PC_', 'PFI_', 'QAFF_', 'QAF_', 
        'QAI_', 'QAT_', 'QBBPPOT_', 'QC_', 'QQMAB_', 'QS_', 
        'Q_', 'TFPHP_', 'VAFF_', 'VAI_', 'VAT_', 'VC_', 'VS_', 
        'WBF_', 'WBU_', 'WCF_', 'WCR1_', 'WCR2_', 'WIND_', 
        'WNF_', 'YDH_', 'ZZ_']
        >>> # get the list of all names
        >>> comments.names                # doctest: +ELLIPSIS
        ['ACAF', 'ACAG', 'AOUC', ..., 'ZKF', 'ZX', 'ZZ_']
        >>> # search in file
        >>> comments.clear()
        >>> comments.get_names("A*;*_")
        []
        >>> comments.get_names("A*;*_", f"{SAMPLE_DATA_DIR}/fun.cmt")   # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading ...\\fun.cmt
        317 objects loaded
        ['ACAF', 'ACAG', 'AOUC', 'AQC', 'BENEF_', 'GOSH_', 
        'IDH_', 'PAFF_', 'PC_', 'PFI_', 'QAFF_', 'QAF_', 
        'QAI_', 'QAT_', 'QBBPPOT_', 'QC_', 'QQMAB_', 'QS_', 
        'Q_', 'TFPHP_', 'VAFF_', 'VAI_', 'VAT_', 'VC_', 'VS_', 
        'WBF_', 'WBU_', 'WCF_', 'WCR1_', 'WCR2_', 'WIND_', 
        'WNF_', 'YDH_', 'ZZ_']
        >>> # empty pattern -> return no names
        >>> comments.get_names("")
        []
        """
        cdef int i_iode_type = self.abstract_db_ptr.get_iode_type()
        cdef int _all = ord('*')
        cdef KDB* kdb_ptr = NULL
        cdef size_t length = 0
        cdef char* c_list = NULL
        
        if not isinstance(pattern, str) and isinstance(pattern, Iterable) and all(isinstance(item, str) for item in pattern):
            pattern = ';'.join(pattern)

        # empty pattern (but not None)
        if isinstance(pattern, str) and not len(pattern):
            return []
        
        if filepath is not None:
            if isinstance(filepath, Path):
                filepath = str(filepath.resolve())
            expected_file_type = IodeFileType(i_iode_type)
            filepath = check_filepath(filepath, expected_file_type, file_must_exist=True)
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
            return []

        b_list: bytes = c_list[:length]
        SCR_free(c_list)

        py_list = split_list(b_list.decode())
        py_list = [item for item in py_list if len(item)]
        return py_list

    @property
    def names(self) -> List[str]:
        """
        List of names of all objects in the current database.

        Returns
        -------
        list(str)
        
        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> # get the list of all names
        >>> comments.names                # doctest: +ELLIPSIS
        ['ACAF', 'ACAG', 'AOUC', ..., 'ZKF', 'ZX', 'ZZ_']
        """
        return [name.decode() for name in self.abstract_db_ptr.get_names(b'', <bint>True)]

    def rename(self, old_name: str, new_name: str):
        """
        Rename an object of the database.

        Parameters
        ----------
        old_name: str
            current name in the database
        new_name: str
            new name in the database

        Warning
        -------
        Renaming an Equation is not allowed.
        
        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> comments["ACAF"]
        'Ondernemingen: ontvangen kapitaaloverdrachten.'

        >>> # rename comment 'ACAF' as 'ACCAF'
        >>> comments.rename("ACAF", "ACCAF")
        >>> "ACCAF" in comments
        True
        >>> comments["ACCAF"]
        'Ondernemingen: ontvangen kapitaaloverdrachten.'
        """
        if self.abstract_db_ptr.get_iode_type() == IodeType.EQUATIONS:
            warnings.warn("Renaming an Equation is not allowed")
        else:
            new_pos = self.abstract_db_ptr.rename(old_name.encode(), new_name.encode())
            if new_pos < 0:
                raise RuntimeError(f"Could not rename item '{old_name}' as '{new_name}'")

    def remove(self, names: Union[str, List[str]]):
        """
        Delete the object(s) named 'names' from the current database.
        
        Parameters
        ----------
        names: str
            name(s) of object(s) to be deleted. 
            It can be a pattern (e.g. "A*").

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> comments.get_names("A*")
        ['ACAF', 'ACAG', 'AOUC', 'AQC']

        >>> # remove one object
        >>> comments.remove("ACAF")
        >>> comments.get_names("A*")
        ['ACAG', 'AOUC', 'AQC']

        >>> # remove all objects with a name ending by '_'
        >>> comments.get_names("*_")            # doctest: +ELLIPSIS
        ['BENEF_', 'GOSH_', 'IDH_', ..., 'WNF_', 'YDH_', 'ZZ_']
        >>> comments.remove("*_")
        >>> comments.get_names("*_")
        []
        """
        if isinstance(names, str):
            names = self.get_names(names)
        if isinstance(names, Iterable) and all(isinstance(value, str) for value in names):
            for name in names:
                self.abstract_db_ptr.remove(name.encode())
        else:
            raise TypeError(f"'names': Expected value of type string or list of strings. " + 
                "Got value of type {type(names).__name__}")

    def compare(self, filepath: Union[str, Path], only_in_workspace_list_name: str=None, 
                only_in_file_list_name: str=None, equal_objects_list_name: str=None, 
                different_objects_list_name: str=None) -> Dict[str, List[str]]:
        """
        The objects of the current database are compared with those stored in the file `filepath`. 
        
        The result of this comparison is composed of 4 lists:
        
          - *only_in_workspace_list*: objects only found in the current database
          - *only_in_file_list*: objects only found in the file `filepath`
          - *equal_objects_list*: objects found in both with the same value
          - *different_objects_list*: objects found in both but with a different value
        
        The comparison is made according to current database type.

        For the IODE Variables, the comparison between two variables is made according to 
        the threshold defined by :meth:`iode.Variables.threshold`.

        Parameters
        ----------
        filepath: str or Path
            path to the file to be compared with the current database
        only_in_workspace_list_name: str, optional
            name of the list of objects only found in the current database.
            Defaults to "OLD_<IODE_TYPE>".
        only_in_file_list_name: str, optional
            name of the list of objects only found in the file `filepath`.
            Defaults to "NEW_<IODE_TYPE>".
        equal_objects_list_name: str, optional
            name of the list of objects found in both with the same value.
             Defaults to "SAME_<IODE_TYPE>".
        different_objects_list_name: str, optional
            name of the list of objects found in both but with a different value.
            Defaults to "CHANGED_<IODE_TYPE>".

        Returns
        -------
        dict(str, list(str))
            dictionary containing the 4 lists of objects

        Examples
        --------
        >>> import numpy as np
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> variables.threshold
        1e-07

        >>> # ---- create Variables file to compare with ----
        >>> vars_other = variables.copy()
        >>> # add two variables
        >>> vars_other["NEW_VAR"] = 0.0
        >>> vars_other["NEW_VAR_2"] = 0.0
        >>> # delete two variables
        >>> del vars_other["AOUC"]
        >>> del vars_other["AQC"]
        >>> # change the value of two variables (above threshold)
        >>> vars_other["ACAF"] = np.asarray(variables["ACAF"]) + 1.e-5
        >>> vars_other["ACAG"] = np.asarray(variables["ACAG"]) + 1.e-5
        >>> # change the value of two variables (below threshold)
        >>> vars_other["BENEF"] = np.asarray(variables["BENEF"]) + 1.e-8
        >>> vars_other["BQY"] = np.asarray(variables["BQY"]) + 1.e-8
        >>> # save the Variables file to compare with
        >>> vars_other.save("fun_other.var")                    # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Saving ...\\fun_other.var
        394 objects saved

        >>> # ---- compare the current Variables database ----
        >>> # ---- with the content of the saved file     ----
        >>> lists_compare = variables.compare("fun_other.var")  # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading ...\\fun_other.var
        394 objects loaded
        >>> for name, value in lists_compare.items():           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        ...    print(f"{name}: {value}")
        OLD_VAR: ['AOUC', 'AQC']
        NEW_VAR: ['NEW_VAR', 'NEW_VAR_2']
        SAME_VAR: ['AOUC_', 'BENEF', 'BQY', 'BRUGP', ..., 'ZKFO', 'ZX', 'ZZF_'] 
        CHANGED_VAR: ['ACAF', 'ACAG']
        """
        int_iode_type: int = self.abstract_db_ptr.get_iode_type()
        expected_file_type: IodeFileType = IodeFileType(int_iode_type)

        if filepath is None:
            raise ValueError("Missing value for the argument 'filepath'")
        if isinstance(filepath, Path):
            filepath = str(filepath)
        if not len(filepath):
            raise ValueError("'filepath' is empty")
        filepath = check_filepath(filepath, expected_file_type, file_must_exist=True)

        short_type_name = IODE_FILE_TYPES[int_iode_type].extensions[0].replace('.', '').upper()
        if only_in_file_list_name is None:
            only_in_workspace_list_name = f"OLD_{short_type_name}"
        if only_in_file_list_name is None:
            only_in_file_list_name = f"NEW_{short_type_name}"
        if equal_objects_list_name is None:
            equal_objects_list_name = f"SAME_{short_type_name}"
        if different_objects_list_name is None:
            different_objects_list_name = f"CHANGED_{short_type_name}"

        args = f"{filepath} {only_in_workspace_list_name} {only_in_file_list_name} "
        args += f"{equal_objects_list_name} {different_objects_list_name}"

        res = B_DataCompare(args.encode(), int_iode_type)
        if res != 0:
            B_display_last_error()
        
        return {only_in_workspace_list_name: lists[only_in_workspace_list_name],
                only_in_file_list_name: lists[only_in_file_list_name],
                equal_objects_list_name: lists[equal_objects_list_name],
                different_objects_list_name: lists[different_objects_list_name]}

    def merge(self, other: Self, overwrite: bool=True):
        """
        Merge the content of the 'other' database into the current database.

        Parameters
        ----------
        other:
            other database to be merged in the current one.
        overwrite: bool, optional
            Whether or not to overwrite the objects with the same name in the two database.
            Defaults to True.
        
        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")     # doctest: +ELLIPSIS
        Loading .../fun.cmt
        317 objects loaded

        >>> # copy comments with names starting with 'A' into a 
        >>> # new database 'cmt_detached'
        >>> cmt_detached = comments.copy("A*")
        >>> cmt_detached.names
        ['ACAF', 'ACAG', 'AOUC', 'AQC']

        >>> # remove 'ACAF' and 'ACAG' from the global Comments database
        >>> del comments['ACAF;ACAG']
        >>> comments.get_names("A*")
        ['AOUC', 'AQC']

        >>> # update the content of 'AOUC' in 'cmt_detached'
        >>> cmt_detached['AOUC'] = "Comment modified"
        >>> cmt_detached['AOUC']
        'Comment modified'
        >>> # content of 'AOUC' in the global Comments database
        >>> comments['AOUC']
        'Kost per eenheid produkt.'

        >>> # merge 'cmt_detached' into the global Comments database
        >>> # -> preserve 'AOUC' in the global Comments database
        >>> comments.merge(cmt_detached, overwrite=False)
        >>> 'ACAF' in comments
        True
        >>> 'ACAG' in comments
        True
        >>> comments['AOUC']
        'Kost per eenheid produkt.'

        >>> # merging 'cmt_detached' into the global Comments database
        >>> # -> overwrite the content of 'AOUC' in the global Comments database 
        >>> comments.merge(cmt_detached)
        >>> comments['AOUC']
        'Comment modified'
        """
        if not isinstance(other, type(self)):
            raise TypeError(f"'other': Expected value of type {type(self).__name__}. " + 
                "Got value of type {type(other).__name__}")
        if not isinstance(overwrite, bool):
            raise TypeError(f"'overwrite': Expected value of type boolean. Got value of type {type(overwrite).__name__}")
        
        cdef CKDBAbstract* other_db_ptr = (<_AbstractDatabase>other).abstract_db_ptr
        self.abstract_db_ptr.merge(dereference(other_db_ptr), <bint>overwrite)

    def _copy_from(self, input_files: Union[str, List[str]], names: Union[str, List[str]]) -> Tuple[str, str]:
        if isinstance(input_files, str):
            input_files = input_files.split(';')
        if isinstance(input_files, Iterable): 
            if not all(isinstance(item, str) for item in input_files):
                raise TypeError("'input_files': expected value of type str or list(str). "
                                "At least one of the item of 'input_files' is not of type str")
            # convert all relative path to absolute path
            input_files = [str(Path(filepath).resolve()) for filepath in input_files]
            input_files = ';'.join(input_files)

        if not isinstance(names, str) and isinstance(names, Iterable): 
            if not all(isinstance(name, str) for name in names):
                raise TypeError("'names': expected value of type str or list(str). "
                                "At least one of the item of 'names' is not of type str")
            names = ";".join(names)

        return input_files, names

    def merge_from(self, input_file: str):
        """
        Merge all objects stored in the input file 'input_file' into the current database.

        Parameters
        ----------
        input_file: str
            file from which the objects to merge are read.
        
        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded
        >>> len(comments)
        317
        >>> # delete all comments
        >>> comments.clear()
        >>> len(comments)
        0

        >>> # reload all comments
        >>> comments.merge_from(f"{SAMPLE_DATA_DIR}/fun.cmt")   # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading ...\\fun.cmt
        317 objects loaded
        >>> len(comments)
        317
        """
        if not isinstance(input_file, str):
            raise TypeError(f"'input_file': Expected value of type string. Got value of type {type(input_file).__name__}")
        # convert relative path to absolute path
        input_file = str(Path(input_file).resolve())
        self.abstract_db_ptr.merge_from(input_file.encode())

    def search(self, pattern: str, word: bool=True, case_sensitive: bool=True, in_name: bool=True, in_formula: bool=True, 
               in_text: bool=True, list_result: str="_RES"):
        r"""Return a list of all objects from the current database having a specific pattern in their names or LEC expression, comment...
          
        The following characters in *pattern* have a special meaning:
        
            - `*` : any character sequence, even empty
            - `?` : any character (one and only one)
            - `@` : any alphanumerical char [A-Za-z0-9]
            - `&` : any non alphanumerical char
            - `|` : any alphanumeric character or none at the beginning and end of a string 
            - `!` : any non-alphanumeric character or none at the beginning and end of a string 
            - `\` : escape the next character
        
        The Search method depends on the type of object:

        - Comments: the name and text of the comments are analyzed 
        - Equations: the name and LEC form of the equations are analyzed 
        - Identities: the name and LEC form of the identities are analyzed 
        - Lists: the name and text of the lists are analyzed 
        - Scalars: the name of the scalars is analyzed 
        - Tables: the table name, titles and LEC forms are analyzed 
        - Variables: the name of the variables is analyzed 

        Parameters
        ----------
        pattern: str     
            string to search
        word: bool, optional
            Whether or not the pattern to be searched for must be a whole word and not part of a word. 
            Default to True.
        case_sensitive: bool, optional
            Whether or not the search is case sensitive.
            Default to True.
        in_name: bool, optional
            Whether or not to also search in object names.
            Default to True.
        in_formula: bool, optional
            Whether or not to also search in LEC expressions (for Equations, Identities and Tables (LEC cells)).
            Default to True.
        in_text: bool, optional
            Whether or not to also search in texts (for Comments, lists, Equations (comment) and Tables (text cells)).
            Default to True.
        list_result: str, optional
            Name of the IODE list in which the resulting list of objects is saved.
            Default to *_RES*.

        Returns
        -------
        list(str)

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments, equations, identities, lists, tables
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.eqs
        274 objects loaded 
        >>> identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.idt
        48 objects loaded 
        >>> lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")            # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.lst
        17 objects loaded 
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded 

        >>> # get list of comments containing 'Bruto'
        >>> comments.search("Bruto")            # doctest: +ELLIPSIS 
        ['GOSF', 'GOSG', 'GOSH', 'GOSH_', ..., 'VIF', 'VIG', 'YDH', 'YDH_']

        >>> # get list of equations containing 'AOUC'
        >>> equations.search("AOUC")
        ['AOUC', 'PC', 'PIF', 'PXS', 'QXAB']

        >>> # get list of identities containing 'NDOMY'
        >>> identities.search("NDOMY")
        ['LCLASS', 'LKEYN', 'UCLASS']

        >>> # get list of IODE lists containing 'AOUC'
        >>> lists.search("AOUC")
        ['COPY0', 'ENDO0', 'TOTAL0']

        >>> # get list of IODE tables containing 'AOUC' in cells
        >>> tables.search("AOUC")
        ['ANAPRIX', 'MULT1FR', 'MULT1RESU', 'T1', 'T1NIVEAU']
        """
        return [name_other.decode() for name_other in self.abstract_db_ptr.search(pattern.encode(), 
                <bint>word, <bint>case_sensitive, <bint>in_name, <bint>in_formula, <bint>in_text, 
                list_result.encode())]

    # NOTE: Only available for IODE object types containing a CLEC struct 
    #       -> Equation, Identity and Table
    def _coefficients(self) -> List[str]:
        coeffs_list: List[str] = []
        for i in range(len(self)):
            obj = self._get_object(i)
            coeffs_list += obj.coefficients
        return sorted(list(set(coeffs_list)))

    # NOTE: Only available for IODE object types containing a CLEC struct 
    #       -> Equation, Identity and Table
    def _variables(self) -> List[str]:
        vars_list: List[str] = []
        for i in range(len(self)):
            obj = self._get_object(i)
            vars_list += obj.variables
        return sorted(list(set(vars_list)))

    def _scalars(self) -> List[str]:
        if self.iode_type == IodeType.EQUATIONS or self.iode_type == IodeType.IDENTITIES or \
        self.iode_type == IodeType.TABLES:
            scalars_list: List[str] = []
            for i in range(len(self)):
                obj = self._get_object(i)
                scalars_list += obj.scalars
            return sorted(list(set(scalars_list)))
        else:
            raise TypeError(f"'scalars' is not available for {self.iode_type.name} objects.")



    def _load(self, filepath: str):
        raise NotImplementedError()

    def load(self, filepath: str):
        """
        Load objects stored in file 'filepath' into the current database.
        Erase the database before to load the file.

        Parameters
        ----------
        filepath: str
            path to the file to load
        
        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments, variables
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> len(comments)
        317

        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> len(variables)
        394
        """
        if not self.is_global_workspace:
            raise RuntimeError("Cannot call 'load' method on a subset of a database")
        if not isinstance(filepath, str):
            raise TypeError(f"'filepath': Expected value of type string. Got value of type {type(filepath).__name__}")
        self._load(filepath)

    def save(self, filepath: str):
        """
        Save objects of the current database into the file 'filepath'.

        Parameters
        ----------
        filepath: str
        
        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> len(comments)
        317
        >>> comments.save(f"{SAMPLE_DATA_DIR}/fun2.cmt")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Saving .../fun2.cmt
        317 objects saved
        >>> comments.clear()
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun2.cmt")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun2.cmt
        317 objects loaded
        >>> len(comments)
        317
        """
        if not isinstance(filepath, str):
            raise TypeError(f"'filepath': Expected value of type string. Got value of type {type(filepath).__name__}")

        self.abstract_db_ptr.save(filepath.encode())

    def clear(self):
        """
        Delete all objects from the current database.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> len(comments)
        317
        >>> comments.clear()
        >>> len(comments)
        0
        """
        self.abstract_db_ptr.clear()

    # special methods

    def __len__(self) -> int:
        """
        Return the number of IODE objects in the current database.

        Returns
        -------
        int

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> len(comments)
        317
        """
        return self.abstract_db_ptr.count()

    def __contains__(self, item) -> bool:
        """
        Test if the IODE object named `item` is present in the current database.

        Parameters
        ----------
        item: str
            name of the IODE object. 

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> "ACAF" in comments
        True
        >>> "ZCAF" in comments
        False
        """
        if not isinstance(item, str):
            raise TypeError(f"Expected value of type string. Got value of type {type(item).__name__}")
        return self.abstract_db_ptr.contains(item.encode())

    def __iter__(self):
        """
        Iterate over object names.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> cmt_subset = comments["A*"]
        >>> cmt_subset.names
        ['ACAF', 'ACAG', 'AOUC', 'AQC']
        >>> for name in cmt_subset:
        ...     print(name)
        ACAF
        ACAG
        AOUC
        AQC
        """
        return iter(self.names)

    # needs to be overriden for Variables
    def _unfold_key(self, key) -> List[str]:
        names = []
        if isinstance(key, str):
            key = key.strip()
            # key represents a slice
            if ':' in key:
                key = key.split(key)
                if len(key) != 2:
                    raise ValueError("When selecting a subset of a workspace, "
                                     "the character ':' must only appear once")
                first_name, last_name = key
                key = slice(first_name, last_name)
            # key represents either a single IODE object or a pattern (A* for example)
            else:
                names = self.get_names(key)
                # names is empty -> key represents the name of a new object
                if not len(names):
                    names = [key]
                return names
        # key represents a selection of IODE objects
        elif isinstance(key, Iterable) and all(isinstance(item, str) for item in key):
            # empty key
            if not len(key):
                raise ValueError(f"Empty list of names for {type(self).__name__} objects.")
            return key
        
        # key represents a slice
        if isinstance(key, slice):
            if key.step is not None:
                raise ValueError("When selecting a subset of a workspace using a slice, the step cannot be used")
            names = self.names
            first_name, last_name = key.start, key.stop
            # raise an error if first_name refers to an IODE object that does not exist
            first_index = names.index(first_name)
            # raise an error if last_name refers to an IODE object that does not exist
            last_index = names.index(last_name)
            # last_name must be equal or after first_name
            if last_index < first_index:
                raise ValueError(f"Cannot select {type(self).__name__} objects between '{first_name}' and '{last_name}'. "
                                 f"'{first_name}' is after '{last_name}'.")
            names = names[first_index:last_index+1]
            # invalid key
            if not len(names):
                raise ValueError(f"Invalid name or pattern '{key}' for {type(self).__name__} objects.")
            return names

        # invalid type for key
        raise TypeError(f"Invalid type for the selection key '{key}' when trying to select {type(self).__name__} objects.\n"
                        f"Expected selection key of type str, list of str or slice(str, str).\n"
                        f"Got selection key of type {type(key).__name__} instead.")

    def _get_object(self, key):
        raise NotImplementedError()

    def _subset(self, pattern: str, copy: bool) -> Self:
        raise NotImplementedError()

    # needs to be overriden for Variables
    @property
    def i(self) -> PositionalIndexer:
        raise NotImplementedError()

    # needs to be overriden for Variables
    def __getitem__(self, key):
        names = self._unfold_key(key)
        # names represents a single IODE object
        if len(names) == 1:
            return self._get_object(names[0])
        # names represents a selection of IODE objects
        else:
            names = ';'.join(names)
            return self._subset(names, copy=False)

    def _set_object(self, key, value):
        raise NotImplementedError()

    # needs to be overriden for Variables
    def __setitem__(self, key, value):
        names = self._unfold_key(key)
        # update/add a single IODE object
        if len(names) == 1:
            self._set_object(names[0], value)
        # update/add several IODE objects
        else:
            # if value is a string or a numerical value -> set the same value for all objects 
            values = [value] * len(names) if isinstance(value, str) or not isinstance(value, Iterable) else value
            # check list of values has the same length as list of names
            if len(names) != len(values):
                raise ValueError(f"Cannot add/update values for {type(self).__name__} objects for the selection key '{key}'.\n"
                                f"{len(values)} values has been passed while the selection key '{key}' "
                                f"represents {len(names)} objects.")
            for name, value in zip(names, values):
                self._set_object(name, value) 

    # needs to be overriden for Variables
    def __delitem__(self, key):
        names = self._unfold_key(key)
        for name in names:
            self.abstract_db_ptr.remove(name.encode())

    def _str_header(self) -> str:
        type_name = self.__class__.__name__
        s = f"Workspace: {type_name}\n"
        s += f"nb {type_name.lower()}: {len(self)}\n"
        s += f"filename: {self.filename}\n"
        if self.description:
            s += f"description: {self.description}\n"
        return s
    
    def _str_table(self, names: List[str]) -> str:
        raise NotImplementedError()

    def __str__(self) -> str:
        s = self._str_header() + '\n'
        if len(self):
            names = self.names
            if len(names) >= 12:
                names = names[:6] + names[-6:]
            s += self._str_table(names)
        return s

    def __repr__(self) -> str:
        return str(self)
