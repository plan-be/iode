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

from iode.util import join_lines, table2str, JUSTIFY

from cython.operator cimport dereference
from pyiode.common cimport IodeType
from pyiode.iode_database.cpp_api_database cimport KDBAbstract as CKDBAbstract


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
            raise RuntimeError("Cannot instanciate an abstract class")

    # Destructor
    def __dealloc__(self):
        pass

    def is_subset(self) -> bool:
        """
        Whether or not the present object represents a subset of a global IODE database.

        Returns
        -------
        bool

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> comments.is_subset()
        False
        >>> cmt_subset = comments["A*"].copy()
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
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> comments.is_subset()
        False
        >>> cmt_subset = comments["A*"].copy()
        >>> cmt_subset.is_subset()
        True
        >>> cmt_subset.is_copy_subset()
        True
        """
        if self.abstract_db_ptr.is_global_database():
            return False
        return self.abstract_db_ptr.is_local_database()

    def _subset(self, pattern: str, copy: bool) -> Self:
        raise NotImplementedError()

    def copy(self) -> Self:
        """
        Create a new database instance in which each object is a *copy* of the original object 
        from the global IODE database. Any change made to the *copied database* (*subset*) will 
        not be applied to the global workspace. This can be useful for example if you want to 
        save previous values of scalars before estimating an equation or a block of equations and 
        then restore the original values if the estimated values are not satisfying.

        Returns
        -------
        Database
        
        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")

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
        """
        return self._subset('*', copy=True)

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
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
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
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
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
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
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
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
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
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> comments.get_name(0)
        'ACAF'
        """
        if not (0 <= pos < len(self)):
            raise IndexError(f"Index {pos} is out of bounds for database of size {len(self)}")
        return self.abstract_db_ptr.get_name(pos).decode()

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
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
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
        """
        if not isinstance(pattern, str) and isinstance(pattern, Iterable) and all(isinstance(item, str) for item in pattern):
            pattern = ';'.join(pattern)
        return [name.decode() for name in self.abstract_db_ptr.get_names(pattern.encode(), <bint>True)]

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
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
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
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
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
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
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
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments
        >>> comments.load(f"{{SAMPLE_DATA_DIR}}/fun.cmt")

        >>> # copy comments with names starting with 'A' into a new database 'cmt_subset'
        >>> cmt_subset = comments.subset("A*", deep_copy=True)
        >>> cmt_subset.names
        ['ACAF', 'ACAG', 'ACOUG', 'AQC']

        >>> # remove 'ACAF' and 'ACAG' from the global Comments database
        >>> comments.remove('ACAF;ACAG')
        >>> comments.get_names("A*")
        ['ACOUG']

        >>> # update the content of 'ACOUG' in 'cmt_subset'
        >>> cmt_subset['ACOUG'] = "Comment modified"
        >>> cmt_subset['ACOUG']
        'Comment modified'
        >>> # content of 'ACOUG' in the global Comments database
        >>> comments['ACOUG']
        ''

        >>> # merge 'cmt_subset' into the global Comments database
        >>> # preserve 'ACOUG' in the global Comments database
        >>> comments.merge(cmt_subset, overwrite=False)
        >>> comments.contains('ACAF')
        True
        >>> comments.contains('ACAG')
        True
        >>> comments['ACOUG']
        ''

        >>> # merge 'cmt_subset' into the global Comments database
        >>> # overwrite the content of 'ACOUG' in the global Comments database 
        >>> comments.merge(cmt_subset)
        >>> comments['ACOUG']
        ''
        """
        if not isinstance(other, type(self)):
            raise TypeError(f"'other': Expected value of type {type(self).__name__}. " + 
                "Got value of type {type(other).__name__}")
        if not isinstance(overwrite, bool):
            raise TypeError(f"'overwrite': Expected value of type boolean. Got value of type {type(overwrite).__name__}")
        
        cdef CKDBAbstract* other_db_ptr = (<_AbstractDatabase>other).abstract_db_ptr
        self.abstract_db_ptr.merge(dereference(other_db_ptr), <bint>overwrite)

    def copy_from(self, input_files: Union[str, List[str]], objects_names: Union[str, List[str]]='*'):
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
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> len(comments)
        317
        >>> # delete all comments with a name starting with 'A'
        >>> comments.remove("A*")
        >>> comments.get_names("A*")
        []

        >>> # load all comments with a name starting with 'A'
        >>> comments.copy_from(f"{SAMPLE_DATA_DIR}/fun.cmt", "A*")
        >>> comments.get_names("A*")
        ['ACAF', 'ACAG', 'AOUC', 'AQC']

        >>> comments.clear()
        >>> # load all comments
        >>> comments.copy_from(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> len(comments)
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

        self.abstract_db_ptr.copy_from(input_files.encode(), objects_names.encode())

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
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> len(comments)
        317
        >>> # delete all comments
        >>> comments.clear()
        >>> len(comments)
        0

        >>> # reload all comments
        >>> comments.merge_from(f"{SAMPLE_DATA_DIR}/fun.cmt")
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
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
        >>> identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")
        >>> lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")

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
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> len(comments)
        317

        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> len(variables)
        394
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
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> len(comments)
        317
        >>> comments.save(f"{SAMPLE_DATA_DIR}/fun2.cmt")
        >>> comments.clear()
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun2.cmt")
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
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
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
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
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
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
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
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
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
