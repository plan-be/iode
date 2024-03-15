# distutils: language = c++

from pathlib import Path
from collections.abc import Iterable
from typing import Union, Tuple, List, Optional, Any
import sys
if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

from cython.operator cimport dereference
from pyiode.common cimport EnumIodeType
from pyiode.iode_database.database cimport KDBAbstract as CKDBAbstract


# WARNING: remember:
#          - private Python methods start with __ are NOT inherited in the derived class
#          - protected Pythod methods start with _ and are inherited in the derived class 
#            (and so can be overidde)


# Define the Python wrapper class for KDBAbstract
cdef class _AbstractDatabase:
    cdef CKDBAbstract* abstract_db_ptr 

    # Constructor
    def __cinit__(self):
        # pointer *abstract_db_ptr is set in subclasses
        if isinstance(self.__class__, _AbstractDatabase):
            raise RuntimeError("Cannot instanciate an abstract class")

    # Destructor
    def __dealloc__(self):
        pass

    # Public methods
    def get_iode_type(self) -> str:
        """
        Return the IODE type of the current database

        Returns
        -------
        str

        Examples
        --------
        >>> from iode import Comments
        >>> Comments.load("../data/fun.cmt")
        >>> Comments.get_iode_type()
        'Comment'
        """
        return IODE_TYPES_LIST[self.abstract_db_ptr.get_iode_type()]

    def is_subset(self) -> bool:
        """
        Whether or not the present object represents a subset of a global IODE database.

        Returns
        -------
        bool

        Examples
        --------
        >>> from iode import Comments
        >>> Comments.load("../data/fun.cmt")
        >>> Comments.is_subset()
        False
        >>> cmt_subset = Comments.subset("A*")
        >>> cmt_subset.is_subset()
        True
        """
        return not self.abstract_db_ptr.is_global_database()

    def is_copy_subset(self) -> bool:
        """
        Whether or not the present object represents of a subset of a global IODE database 
        and if the IODE objects of the subset represent *deep copies* of the IODE objects 
        from the global IODE database.

        A *deep copy* subset means that any change made on an object of the subset will **NOT** 
        modify the corresponding object in the global IODE database.

        A *shallow copy* subset means that any change made on an object of the subset will also 
        modify the corresponding object in the global IODE database.

        By default, the :py:meth:`iode.CommentsDatabase.subset` method return a *shallow copy* subset.

        Returns
        -------
        bool

        Examples
        --------
        >>> from iode import Comments
        >>> Comments.load("../data/fun.cmt")
        >>> Comments.is_subset()
        False
        >>> # by default a 'shallow copy' subset is returned
        >>> cmt_subset = Comments.subset("A*")
        >>> cmt_subset.is_subset()
        True
        >>> cmt_subset.is_copy_subset()
        False
        >>> # force to return a 'deep copy' subset
        >>> cmt_subset = Comments.subset("A*", copy=True)
        >>> cmt_subset.is_copy_subset()
        True
        """
        if self.abstract_db_ptr.is_global_database():
            return False
        return self.abstract_db_ptr.is_local_database()

    def subset(pattern: str, copy: bool = False) -> Self:
        """
        Create a subset of a global IODE database.

        If 'copy' is True, a *deep copy* subset is returned. 
        This means that any change made on an object of the subset will **NOT** 
        modify the corresponding object in the global IODE database.

        If 'copy' is False (default), a *shallow copy* subset is returned. 
        This means that any change made on an object of the subset will also 
        modify the corresponding object in the global IODE database.

        Parameters
        ----------
        pattern: str
            string pattern used to select the IODE objects to group in the returned subset.
            E.g. "A*" will select all IODE objects with the name starting with 'A', 
            "*_" will select all IODE objects with the name ending with '_' and 
            "*" will select all IODE objects.
        copy: bool, optional
            whether or not to return *deep copy* subset.
            Defaults to False. 

        Returns
        -------
        { CommentsSubset | EquationsSubset | IdentitiesSubset | ListsSubset | ScalarsSubset | TablesSubset | VariablesSubset }
        
        Examples
        --------
        >>> from iode import Comments
        >>> Comments.load("../data/fun.cmt")

        >>> # create a subset with all comments with name starting with 'A'
        >>> cmt_subset = Comments.subset("A*")
        >>> cmt_subset.get_names()
        ['ACAF', 'ACAG', 'ACOUG', 'AQC']
        >>> # any modification made on the subset is visible in the global database
        >>> cmt_subset['ACAF'] = "Modified Comment"
        >>> Comments['ACAF']
        'Modified Comment'

        >>> # force to return a 'deep copy' subset
        >>> cmt_subset = Comments.subset("*_", copy=True)
        >>> # any modification made on the subset let the global database unchanged
        >>> cmt_subset['BENEF_'] = "Modified Comment"
        >>> Comments['BENEF_']
        'Ondernemingen: niet-uitgekeerde winsten (vóór statistische aanpassing).'
        """
        raise NotImplementedError()

    def get_filename(self) -> str:
        r"""
        Return the filepath associated with the current database.

        Returns
        -------
        str

        Examples
        --------
        >>> from iode import Comments
        >>> from pathlib import Path
        >>> from os.path import relpath
        >>> Comments.load("../data/fun.cmt")
        >>> filepath = Comments.get_filename()
        >>> Path(filepath).name
        'fun.cmt'
        """
        return self.abstract_db_ptr.get_filename().decode()

    def get_description(self) -> str:
        """
        Return the description of the current database

        Returns
        -------
        str
        
        Examples
        --------
        >>> from iode import Comments
        >>> Comments.load("../data/fun.cmt")
        >>> Comments.set_description("test data from file 'fun.cmt'")
        >>> Comments.get_description()
        "test data from file 'fun.cmt'"
        """
        return self.abstract_db_ptr.get_description().decode()

    def set_description(self, description: str):
        """
        Set the description of the current database.

        Parameters
        ----------
        description: str

        Examples
        --------
        >>> from iode import Comments
        >>> Comments.load("../data/fun.cmt")
        >>> Comments.set_description("test data from file 'fun.cmt'")
        >>> Comments.get_description()
        "test data from file 'fun.cmt'"
        """
        if not isinstance(description, str):
            raise TypeError(f"'description': Expected value of type string. Got value of type {type(description).__name__}")
        self.abstract_db_ptr.set_description(description.encode())

    def get_names(self, pattern: Union[str, List[str]]="") -> List[str]:
        """
        Returns the list of objects names given a pattern.

        Parameters
        ----------
        pattern: str or list(str), optional
            pattern to select a subset of objects. 
            For example, 'A*;*_' will select all objects for which the name starts 
            with 'A' or ends with '_'.
            Return all names contained in the database by default.

        Returns
        -------
        list(str)
        
        Examples
        --------
        >>> from iode import Comments
        >>> Comments.load("../data/fun.cmt")
        >>> Comments.get_names("A*;*_")         # doctest: +NORMALIZE_WHITESPACE
        ['ACAF', 'ACAG', 'AOUC', 'AQC', 'BENEF_', 'GOSH_', 
        'IDH_', 'PAFF_', 'PC_', 'PFI_', 'QAFF_', 'QAF_', 
        'QAI_', 'QAT_', 'QBBPPOT_', 'QC_', 'QQMAB_', 'QS_', 
        'Q_', 'TFPHP_', 'VAFF_', 'VAI_', 'VAT_', 'VC_', 'VS_', 
        'WBF_', 'WBU_', 'WCF_', 'WCR1_', 'WCR2_', 'WIND_', 
        'WNF_', 'YDH_', 'ZZ_']
        >>> # or equivalently
        >>> Comments.get_names(["A*", "*_"])    # doctest: +NORMALIZE_WHITESPACE
        ['ACAF', 'ACAG', 'AOUC', 'AQC', 'BENEF_', 'GOSH_', 
        'IDH_', 'PAFF_', 'PC_', 'PFI_', 'QAFF_', 'QAF_', 
        'QAI_', 'QAT_', 'QBBPPOT_', 'QC_', 'QQMAB_', 'QS_', 
        'Q_', 'TFPHP_', 'VAFF_', 'VAI_', 'VAT_', 'VC_', 'VS_', 
        'WBF_', 'WBU_', 'WCF_', 'WCR1_', 'WCR2_', 'WIND_', 
        'WNF_', 'YDH_', 'ZZ_']
        >>> # get the list of all names
        >>> Comments.get_names()                # doctest: +ELLIPSIS
        ['ACAF', 'ACAG', 'AOUC', ..., 'ZKF', 'ZX', 'ZZ_']
        """
        if not isinstance(pattern, str) and isinstance(pattern, Iterable) and all(isinstance(item, str) for item in pattern):
            pattern = ';'.join(pattern)
        if not isinstance(pattern, str):
            raise TypeError(f"'pattern': Expected a string value or a list of strings. Got value of type {type(pattern).__name__}")
        return [name.decode() for name in self.abstract_db_ptr.get_names(pattern.encode(), <bint>True)]

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
        >>> from iode import Comments
        >>> Comments.load("../data/fun.cmt")
        >>> Comments["ACAF"]
        'Ondernemingen: ontvangen kapitaaloverdrachten.'

        >>> # rename comment 'ACAF' as 'ACCAF'
        >>> Comments.rename("ACAF", "ACCAF")
        >>> "ACCAF" in Comments
        True
        >>> Comments["ACCAF"]
        'Ondernemingen: ontvangen kapitaaloverdrachten.'
        """
        if not isinstance(old_name, str):
            raise TypeError(f"'old_name': Expected value of type string. Got value of type {type(old_name).__name__}")
        if not isinstance(new_name, str):
            raise TypeError(f"'new_name': Expected value of type string. Got value of type {type(new_name).__name__}")
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
        >>> from iode import Comments
        >>> Comments.load("../data/fun.cmt")
        >>> Comments.get_names("A*")
        ['ACAF', 'ACAG', 'AOUC', 'AQC']

        >>> # remove one object
        >>> Comments.remove("ACAF")
        >>> Comments.get_names("A*")
        ['ACAG', 'AOUC', 'AQC']

        >>> # remove all objects with a name ending by '_'
        >>> Comments.get_names("*_")            # doctest: +ELLIPSIS
        ['BENEF_', 'GOSH_', 'IDH_', ..., 'WNF_', 'YDH_', 'ZZ_']
        >>> Comments.remove("*_")
        >>> Comments.get_names("*_")
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

    def merge(self, other: Self, overwrite: bool=True):
        f"""
        Merge the content of the 'other' database into the current database.

        Parameters
        ----------
        other: {type(self).__name__}
            other database to be merged in the current one.
        overwrite: bool, optional
            Whether or not to overwrite the objects with the same name in the two database.
            Defaults to True.
        
        Examples
        --------
        >>> from iode import Comments
        >>> Comments.load("../data/fun.cmt")

        >>> # copy comments with names starting with 'A' into a new database 'cmt_subset'
        >>> cmt_subset = Comments.subset("A*", deep_copy=True)
        >>> cmt_subset.get_names()
        ['ACAF', 'ACAG', 'ACOUG', 'AQC']

        >>> # remove 'ACAF' and 'ACAG' from the global Comments database
        >>> Comments.remove('ACAF;ACAG')
        >>> Comments.get_names("A*")
        ['ACOUG']

        >>> # update the content of 'ACOUG' in 'cmt_subset'
        >>> cmt_subset['ACOUG'] = "Comment modified"
        >>> cmt_subset['ACOUG']
        'Comment modified'
        >>> # content of 'ACOUG' in the global Comments database
        >>> Comments['ACOUG']
        ''

        >>> # merge 'cmt_subset' into the global Comments database
        >>> # preserve 'ACOUG' in the global Comments database
        >>> Comments.merge(cmt_subset, overwrite=False)
        >>> Comments.contains('ACAF')
        True
        >>> Comments.contains('ACAG')
        True
        >>> Comments['ACOUG']
        ''

        >>> # merge 'cmt_subset' into the global Comments database
        >>> # overwrite the content of 'ACOUG' in the global Comments database 
        >>> Comments.merge(cmt_subset)
        >>> Comments['ACOUG']
        ''
        """
        if not isinstance(other, type(self)):
            raise TypeError(f"'other': Expected value of type {type(self).__name__}. " + 
                "Got value of type {type(other).__name__}")
        if not isinstance(overwrite, bool):
            raise TypeError(f"'overwrite': Expected value of type boolean. Got value of type {type(overwrite).__name__}")
        
        cdef CKDBAbstract* other_db_ptr = (<_AbstractDatabase>other).abstract_db_ptr
        self.abstract_db_ptr.merge(dereference(other_db_ptr), <bint>overwrite)

    def copy_into(self, input_files: Union[str, List[str]], objects_names: Union[str, List[str]]='*'):
        """
        Copy (a subset of) objects from the input file(s) 'input_files' into the current database.

        Parameters
        ----------
        input_file: str or list(str)
            file(s) from which the copied objects are read.
        objects_names: str or list(str)
            list of objects to copy from the input file(s).
            Defaults to load all objects from the input file(s). 
        
        Examples
        --------
        >>> from iode import Comments
        >>> Comments.load("../data/fun.cmt")
        >>> len(Comments)
        317
        >>> # delete all comments with a name starting with 'A'
        >>> Comments.remove("A*")
        >>> Comments.get_names("A*")
        []

        >>> # load all comments with a name starting with 'A'
        >>> Comments.copy_into("../data/fun.cmt", "A*")
        >>> Comments.get_names("A*")
        ['ACAF', 'ACAG', 'AOUC', 'AQC']

        >>> Comments.clear()
        >>> # load all comments
        >>> Comments.copy_into("../data/fun.cmt")
        >>> len(Comments)
        317
        """
        if isinstance(input_files, str):
            input_files = input_files.split(';')
        if isinstance(input_files, Iterable) and all(isinstance(item, str) for item in input_files):
            # convert all relative path to absolute path
            input_files = [str(Path(filepath).resolve()) for filepath in input_files]
            input_files = ';'.join(input_files)
        if not isinstance(input_files, str):
            raise TypeError(f"'input_files': Expected value of type string. Got value of type {type(input_files).__name__}")

        if isinstance(objects_names, str):
            pass
        elif isinstance(objects_names, Iterable) and all(isinstance(name, str) for name in objects_names):
            objects_names = " ".join(objects_names)
        else:
            raise TypeError("'objects_names': Expected value of type string or list of strings. " + 
                "Got value of type {type(objects_names).__name__}")

        self.abstract_db_ptr.copy_into(input_files.encode(), objects_names.encode())

    def merge_into(self, input_file: str):
        """
        Merge all objects stored in the input file 'input_file' into the current database.

        Parameters
        ----------
        input_file: str
            file from which the objects to merge are read.
        
        Examples
        --------
        >>> from iode import Comments
        >>> Comments.load("../data/fun.cmt")
        >>> len(Comments)
        317
        >>> # delete all comments
        >>> Comments.clear()
        >>> len(Comments)
        0

        >>> # reload all comments
        >>> Comments.merge_into("../data/fun.cmt")
        >>> len(Comments)
        317
        """
        if not isinstance(input_file, str):
            raise TypeError(f"'input_file': Expected value of type string. Got value of type {type(input_file).__name__}")
        # convert relative path to absolute path
        input_file = str(Path(input_file).resolve())
        self.abstract_db_ptr.merge_into(input_file.encode())

    def get_associated_objects_list(self, name: str, other_type: int):
        """
        Return the list of all objects of type 'other_type' associated with the object named 'name' 
        from the current database.

        Returns
        -------
        list(str)

        Examples
        --------
        >>> from iode import Equations, Scalars, Variables              # doctest: +SKIP
        >>> Equations.load("../data/fun.eqs")                           # doctest: +SKIP
        >>> Scalars.load("../data/fun.scl")                             # doctest: +SKIP
        >>> Variables.load("../data/fun.var")                           # doctest: +SKIP

        >>> # get list of scalars associated with the equation 'ACAF'
        >>> Equations.get_associated_objects_list("ACAF", I_SCALARS)    # doctest: +SKIP
        ['acaf1', 'acaf2', 'acaf4']

        >>> # get list of variables associated with the equation 'ACAF'
        >>> Equations.get_associated_objects_list("ACAF", I_VARIABLES)  # doctest: +SKIP
        ['ACAF']  
        """
        if not isinstance(name, str):
            raise TypeError(f"'name': Expected value of type string. Got value of type {type(name).__name__}")

        if not isinstance(other_type, int):
            raise TypeError(f"'other_type': Expected value of type int. Got value of type {type(other_type).__name__}")

        return [name_other.encode() for name_other in self.abstract_db_ptr.get_associated_objects_list(name.encode(), <EnumIodeType>other_type)]

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
        >>> from iode import Comments
        >>> Comments.load("../data/fun.cmt")
        >>> len(Comments)
        317
        """
        if self.is_subset():
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
        >>> from iode import Comments
        >>> Comments.load("../data/fun.cmt")
        >>> len(Comments)
        317
        >>> Comments.save("../data/fun2.cmt")
        >>> Comments.clear()
        >>> Comments.load("../data/fun2.cmt")
        >>> len(Comments)
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
        >>> from iode import Comments
        >>> Comments.load("../data/fun.cmt")
        >>> len(Comments)
        317
        >>> Comments.clear()
        >>> len(Comments)
        0
        """
        self.abstract_db_ptr.clear()

    # special methods

    def __len__(self) -> int:
        """
        Return the number of object in the database

        Returns
        -------
        int

        Examples
        --------
        >>> from iode import Comments
        >>> Comments.load("../data/fun.cmt")
        >>> len(Comments)
        317
        """
        return self.abstract_db_ptr.count()

    def __contains__(self, item) -> bool:
        """
        Examples
        --------
        >>> from iode import Comments
        >>> Comments.load("../data/fun.cmt")
        >>> "ACAF" in Comments
        True
        >>> "ZCAF" in Comments
        False
        """
        if not isinstance(item, str):
            raise TypeError(f"Expected value of type string. Got value of type {type(item).__name__}")
        return self.abstract_db_ptr.contains(item.encode())

    def _get_object(self, name: str):
        raise NotImplementedError()

    def __getitem__(self, key):
        """
        Examples
        --------
        >>> from iode import Comments
        >>> Comments.load("../data/fun.cmt")
        >>> Comments["ACAF"]
        'Ondernemingen: ontvangen kapitaaloverdrachten.'
        """
        if not isinstance(key, str):
            raise TypeError(f"Cannot get object {key}.\nExpected a string value for {key} " + 
                "but got value of type {type(filepath).__name__}")
        return self._get_object(key) 

    def _set_object(self, name: str, value):
        raise NotImplementedError()

    def __setitem__(self, key, value):
        """
        Examples
        --------
        >>> from iode import Comments
        >>> Comments.load("../data/fun.cmt")
        >>> Comments["ACAF"]
        'Ondernemingen: ontvangen kapitaaloverdrachten.'
        >>> Comments["ACAF"] = "New Value"
        >>> Comments["ACAF"]
        'New Value'
        """
        if not isinstance(key, str):
            raise TypeError(f"Cannot set object {key}.\nExpected a string value for {key} " + 
                "but got value of type {type(filepath).__name__}")
        self._set_object(key, value) 

    def __delitem__(self, key):
        """
        Examples
        --------
        >>> from iode import Comments
        >>> Comments.load("../data/fun.cmt")
        >>> "ACAF" in Comments
        True
        >>> del Comments["ACAF"]
        >>> "ACAF" in Comments
        False
        """
        if not isinstance(key, str):
            raise TypeError(f"Cannot delete object {key}.\nExpected a string value for {key} " + 
                "but got value of type {type(filepath).__name__}")
        self.abstract_db_ptr.remove(key.encode())
