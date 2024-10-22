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
from pyiode.common cimport IodeDatabaseType
from pyiode.iode_database.cpp_api_database cimport KDBAbstract as CKDBAbstract


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
        Create a *copy* of an IODE database. 
        Any change made on the *copied database* will **NOT** 
        modify the corresponding the global IODE database.

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
    def iode_type(self) -> IodeTypes:
        r"""
        Return the object type of the current database.

        Returns
        -------
        IodeTypes

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> comments.iode_type
        <IodeTypes.COMMENTS: 0>
        """
        int_iode_type: int = self.abstract_db_ptr.get_iode_type()
        return IodeTypes(int_iode_type)

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
        >>> filepath = comments.filename
        >>> Path(filepath).name
        'fun.cmt'
        """
        return self.abstract_db_ptr.get_filename().decode()

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
        if self.abstract_db_ptr.get_iode_type() == IodeTypes.EQUATIONS:
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
    def __getitem__(self, key):
        r"""
        Return the (subset of) IODE object(s) referenced by `key`.

        The `key` can represent a single object name (e.g. "ACAF") or a list of object names ("ACAF;ACAG;AOUC") 
        or a pattern (e.g. "A*") or a list of sub-patterns (e.g. "A*;*_").
        
        If the `key` represents a list of object names or of sub-patterns, each name or sub-pattern is separated 
        by a `separator` character which is either a whitespace ` `, or a comma `,`, or a semi-colon `;`, or a 
        tabulation `\t`, or a newline `\n`.

        A (sub-)`pattern` is a list of characters representing a group of object names. 
        It includes some special characters which have a special meaning:
        
            - `*` : any character sequence, even empty
            - `?` : any character (one and only one)
            - `@` : any alphanumerical char [A-Za-z0-9]
            - `&` : any non alphanumerical char
            - `|` : any alphanumeric character or none at the beginning and end of a string 
            - `!` : any non-alphanumeric character or none at the beginning and end of a string 
            - `\` : escape the next character

        Note that the `key` can contain references to IODE lists which are prefixed with the symbol `$`.

        Parameters
        ----------
        key: str or list(str)
            (the list of) name(s) of the IODE object(s) to get.
            The list of objects to get can be specified by a pattern or by a list of sub-patterns (e.g. "A*;*_").

        Returns
        -------
        Single IODE object or a subset of the database.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR

        Comments

        >>> from iode import comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> # a) get one Comment
        >>> comments["ACAF"]
        'Ondernemingen: ontvangen kapitaaloverdrachten.'
        >>> # b) get a subset of the Comments database using a pattern
        >>> comments_subset = comments["A*"]
        >>> comments_subset.names
        ['ACAF', 'ACAG', 'AOUC', 'AQC']
        >>> # c) get a subset of the Comments database using a list of names
        >>> comments_subset = comments[["ACAF", "AOUC", "BQY", "BVY"]]
        >>> comments_subset.names
        ['ACAF', 'AOUC', 'BQY', 'BVY']

        Equations

        >>> from iode import equations
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
        >>> # a) get one equation
        >>> equations["ACAF"]                  # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'ACAF',
                 lec = '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)',
                 method = 'LSQ',
                 from_period = '1980Y1',
                 to_period = '1996Y1',
                 block = 'ACAF',
                 tests = {corr = 1,
                          dw = 2.32935,
                          fstat = 32.2732,
                          loglik = 83.8075,
                          meany = 0.00818467,
                          r2 = 0.821761,
                          r2adj = 0.796299,
                          ssres = 5.19945e-05,
                          stderr = 0.00192715,
                          stderrp = 23.5458,
                          stdev = 0.0042699},
                 date = '12-06-1998')

        >>> # b) get a subset of the Equations database using a pattern
        >>> equations_subset = equations["A*"]
        >>> equations_subset.names
        ['ACAF', 'ACAG', 'AOUC']
        >>> # c) get a subset of the Equations database using a list of names
        >>> equations_subset = equations[["ACAF", "AOUC", "BQY", "BVY"]]
        >>> equations_subset.names
        ['ACAF', 'AOUC', 'BQY', 'BVY']

        Identities

        >>> from iode import identities
        >>> identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")
        >>> # a) get one Identity
        >>> identities["AOUC"]
        Identity('((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/(VM+VAFF))[-1]')
        >>> # b) get a subset of the Identities database using a pattern
        >>> identities_subset = identities["X*"]
        >>> identities_subset.names
        ['XEX', 'XNATY', 'XPOIL', 'XPWMAB', 'XPWMS', 'XPWXAB', 'XPWXS', 'XQWXAB', 'XQWXS', 'XQWXSS', 'XRLBER', 'XTFP', 'XW']
        >>> # c) get a subset of the Identities database using a list of names
        >>> identities_subset = identities[["XEX", "XPWMAB", "XPWMS", "XQWXS", "XTFP"]]
        >>> identities_subset.names
        ['XEX', 'XPWMAB', 'XPWMS', 'XQWXS', 'XTFP']

        Lists

        >>> from iode import lists
        >>> lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")
        >>> # a) get one list
        >>> lists["ENVI"]
        ['EX', 'PWMAB', 'PWMS', 'PWXAB', 'PWXS', 'QWXAB', 'QWXS', 'POIL', 'NATY', 'TFPFHP_']
        >>> # b) get a subset of the Lists database using a pattern
        >>> lists_subset = lists["E*"]
        >>> lists_subset.names
        ['ENDO', 'ENDO0', 'ENDO1', 'ENVI']
        >>> # c) get a subset of the Lists database using a list of names
        >>> lists_subset = lists[["COPY", "ENDO", "ENVI", "TOTAL"]]
        >>> lists_subset.names
        ['COPY', 'ENDO', 'ENVI', 'TOTAL']

        Scalars

        >>> from iode import scalars
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")
        >>> # a) get one scalar
        >>> acaf1 = scalars["acaf1"]
        >>> acaf1
        Scalar(0.0157684, 1, 0.00136871)
        >>> acaf1.value
        0.01576840691268444
        >>> acaf1.relax
        1.0
        >>> acaf1.std
        0.0013687137980014086
        >>> # b) get a subset of the Scalars database using a pattern
        >>> scalars_subset = scalars["a*"]
        >>> scalars_subset.names
        ['acaf1', 'acaf2', 'acaf3', 'acaf4']
        >>> # c) get a subset of the Scalars database using a list of names
        >>> scalars_subset = scalars[["acaf1", "acaf4", "dpuh_1", "dpuh_2"]]
        >>> scalars_subset.names
        ['acaf1', 'acaf4', 'dpuh_1', 'dpuh_2']

        Tables

        >>> from iode import tables
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
        >>> # a) get one table
        >>> tables["YDH"]           # doctest: +NORMALIZE_WHITESPACE
        DIVIS |                                                          1 |                                    PC_*40.34
        TITLE |                        "Tableau B-3. Revenu disponible des ménages à prix constant"
        ----- | ---------------------------------------------------------------------------------------------------------
        CELL  |                                                            |                     "#S"
        CELL  | "Revenus primaires"                                        |                            WBU_+YN+GOSH_+IDH
        CELL  | "   Masse salariale totale"                                |                                         WBU_
        CELL  | "   Revenu net du travail en provenance du Reste du monde" |                                           YN
        CELL  | "   Surplus brut d'exploitation"                           |                                        GOSH_
        CELL  | "   Revenu net de la propriété"                            |                                          IDH
        CELL  | "Cotisations sociales et impôts"                           |                                  SSF+SSH+DTH
        CELL  | "   Cotisations patronales"                                |                                          SSF
        CELL  | "   Cotisations personnelles"                              |                                          SSH
        CELL  | "IPP"                                                      |                                          DTH
        CELL  | "Prestations sociales "                                    |                                     SBH+OCUH
        CELL  | "   Sécurité sociale"                                      |                                          SBH
        CELL  | "   Diverses prestations"                                  |                                         OCUH
        CELL  | "Total"                                                    | (WBU_+YN+GOSH_+IDH)-(SSF+SSH+DTH)+(SBH+OCUH)
        ----- | ---------------------------------------------------------------------------------------------------------
        FILES |
        DATE  |
        <BLANKLINE>
        nb lines: 19
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>        
        >>> # b) get a subset of the Tables database using a pattern
        >>> tables_subset = tables["C8_*"]
        >>> tables_subset.names
        ['C8_1', 'C8_10', 'C8_11', 'C8_13', 'C8_14', 'C8_2', 'C8_3', 'C8_4', 'C8_5', 'C8_6', 'C8_7', 'C8_8', 'C8_9']
        >>> # c) get a subset of the Tables database using a list of names
        >>> tables_subset = tables[["C8_1", "C8_2", "C8_4", "C8_5", "C8_7"]]
        >>> tables_subset.names
        ['C8_1', 'C8_2', 'C8_4', 'C8_5', 'C8_7']

        Variables

        >>> from iode import variables, NA
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> variables.sample
        '1960Y1:2015Y1'
        >>> # -------- a) get one Variable --------
        >>> # get the variable values for the whole sample
        >>> variables["ACAF"]                       # doctest: +ELLIPSIS 
        [-2e+37, -2e+37, ..., -83.34062511080091, -96.41041982848331]
        >>> # get the variable value for a specific period
        >>> variables["ACAF", "1990Y1"]
        23.771
        >>> # get the variable values for range of periods (using a Python slice)
        >>> variables["ACAF", "1990Y1":"2000Y1"]    # doctest: +ELLIPSIS 
        [23.771, 26.240999, ..., 13.530404919696034, 10.046610792200543]
        >>> # same as above but with the colon ':' inside the periods range string
        >>> variables["ACAF", "1990Y1:2000Y1"]      # doctest: +ELLIPSIS 
        [23.771, 26.240999, ..., 13.530404919696034, 10.046610792200543]
        >>> # b) -------- get a subset of the Variables database using a pattern --------
        >>> variables_subset = variables["A*"]
        >>> variables_subset.names
        ['ACAF', 'ACAG', 'AOUC', 'AOUC_', 'AQC']
        >>> # get the variable values for a specific period
        >>> variables["A*", "1990Y1"]
        [23.771, -28.1721855713507, 1.0, 0.9373591502749314, 1.0]
        >>> # get the variable values for range of periods (using a Python slice)
        >>> variables["A*", "1990Y1":"2000Y1"]      # doctest: +ELLIPSIS 
        [[23.771, 26.240999, 30.159, ..., 1.2031082, 1.3429699656745855, 1.3386028553645442]]
        >>> # c) -------- get a subset of the Variables database using a list of names --------
        >>> variables_subset = variables[["ACAF", "ACAG", "AQC", "BQY", "BVY"]]
        >>> variables_subset.names
        ['ACAF', 'ACAG', 'AQC', 'BQY', 'BVY']
        >>> # get the variable values for a specific period
        >>> variables[["ACAF", "ACAG", "AQC", "BQY", "BVY"], "1990Y1"]
        [23.771, -28.1721855713507, 1.0, -34.099998, -34.099997]
        >>> # get the variable values for range of periods (using a Python slice)
        >>> variables[["ACAF", "ACAG", "AQC", "BQY", "BVY"], "1990Y1":"2000Y1"]      # doctest: +ELLIPSIS 
        [[23.771, 26.240999, 30.159, ..., 140.73978, 144.8587818455608, 150.05335230584103]]
        """
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
        r"""
        Update/add a (subset of) IODE object(s) referenced by `key` from/to the current database.

        The `key` can represent a single object name (e.g. "ACAF") or a list of object names ("ACAF;ACAG;AOUC") 
        or a pattern (e.g. "A*") or a list of sub-patterns (e.g. "A*;*_").
        
        If the `key` represents a list of object names or of sub-patterns, each name or sub-pattern is separated 
        by a `separator` character which is either a whitespace ` `, or a comma `,`, or a semi-colon `;`, or a 
        tabulation `\t`, or a newline `\n`.

        A (sub-)`pattern` is a list of characters representing a group of object names. 
        It includes some special characters which have a special meaning:
        
            - `*` : any character sequence, even empty
            - `?` : any character (one and only one)
            - `@` : any alphanumerical char [A-Za-z0-9]
            - `&` : any non alphanumerical char
            - `|` : any alphanumeric character or none at the beginning and end of a string 
            - `!` : any non-alphanumeric character or none at the beginning and end of a string 
            - `\` : escape the next character

        Note that the `key` can contain references to IODE lists which are prefixed with the symbol `$`.

        Parameters
        ----------
        key: str or list(str)
            (the list of) name(s) of the IODE object(s) to update/add.
            The list of objects to update/add can be specified by a pattern or by a list of sub-patterns 
            (e.g. "A*;*_").
        value: 
            (new) value(s) of the IODE object(s). 

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR

        Comments

        >>> from iode import comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> # a) add one comment
        >>> comments["BDY"] = "Difference net incomes (YN - YK)"
        >>> comments["BDY"]
        'Difference net incomes (YN - YK)'

        >>> # b) update one comment
        >>> comments["ACAF"]
        'Ondernemingen: ontvangen kapitaaloverdrachten.'
        >>> comments["ACAF"] = "New Value"
        >>> comments["ACAF"]
        'New Value'

        >>> # c) working on a subset
        >>> # 1) get subset
        >>> comments_subset = comments["A*"]
        >>> comments_subset.names
        ['ACAF', 'ACAG', 'AOUC', 'AQC']
        >>> # 2) add a comment to the subset 
        >>> comments_subset["A0"] = "New Comment"
        >>> comments_subset["A0"]
        'New Comment'
        >>> # --> new comment also appears in the global workspace
        >>> "A0" in comments
        True
        >>> comments["A0"]
        'New Comment'
        >>> # 3) update a comment in the subset
        >>> comments_subset["A0"] = "Updated Comment"
        >>> comments_subset["A0"]
        'Updated Comment'
        >>> # --> comment is also updated in the global workspace
        >>> comments["A0"]
        'Updated Comment'

        Equations

        >>> from iode import equations, EqMethod
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
        >>> # a) add one equation
        >>> equations["BDY"] = "BDY := YN - YK"
        >>> equations["BDY"]                    # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'BDY',
                 lec = 'BDY := YN - YK',
                 method = 'LSQ',
                 from_period = '1960Y1',
                 to_period = '2015Y1')
        
        >>> # b) update one equation        
        >>> equations["ACAF"]                  # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'ACAF',
                 lec = '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)',
                 method = 'LSQ',
                 from_period = '1980Y1',
                 to_period = '1996Y1',
                 block = 'ACAF',
                 tests = {corr = 1,
                          dw = 2.32935,
                          fstat = 32.2732,
                          loglik = 83.8075,
                          meany = 0.00818467,
                          r2 = 0.821761,
                          r2adj = 0.796299,
                          ssres = 5.19945e-05,
                          stderr = 0.00192715,
                          stderrp = 23.5458,
                          stdev = 0.0042699},
                 date = '12-06-1998')
        >>> # update only the LEC
        >>> equations["ACAF"] = "(ACAF/VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)"
        >>> equations["ACAF"]                  # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'ACAF',
                 lec = '(ACAF/VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)',
                 method = 'LSQ',
                 from_period = '1980Y1',
                 to_period = '1996Y1',
                 block = 'ACAF',
                 tests = {corr = 1,
                          dw = 2.32935,
                          fstat = 32.2732,
                          loglik = 83.8075,
                          meany = 0.00818467,
                          r2 = 0.821761,
                          r2adj = 0.796299,
                          ssres = 5.19945e-05,
                          stderr = 0.00192715,
                          stderrp = 23.5458,
                          stdev = 0.0042699},
                 date = '12-06-1998')
        >>> # upate block and sample of a block of equations to estimation (dictionary)
        >>> estim_sample = "2000Y1:2010Y1"
        >>> block = "ACAF; ACAG; AOUC"
        >>> for eq_name in block.split(';'):
        ...     equations[eq_name] = {"sample": estim_sample, "block": block}
        >>> (equations["ACAF"].sample, equations["ACAG"].sample, equations["AOUC"].sample)
        ('2000Y1:2010Y1', '2000Y1:2010Y1', '2000Y1:2010Y1')
        >>> (equations["ACAF"].block, equations["ACAG"].block, equations["AOUC"].block)
        ('ACAF; ACAG; AOUC', 'ACAF; ACAG; AOUC', 'ACAF; ACAG; AOUC')
        >>> # upate sample and block (Equation objects)
        >>> eq_ACAF = equations["ACAF"]
        >>> eq_ACAF.lec = "(ACAF/VAF[-1]) := acaf2 * GOSF[-1] + acaf4 * (TIME=1995)"
        >>> eq_ACAF.method = EqMethod.MAX_LIKELIHOOD
        >>> # new equation sample is from 1990Y1 to the last year of Variables
        >>> eq_ACAF.sample = "1990Y1:"
        >>> eq_ACAF.block = "ACAF"
        >>> equations["ACAF"] = eq_ACAF
        >>> equations["ACAF"]                  # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'ACAF',
                 lec = '(ACAF/VAF[-1]) := acaf2 * GOSF[-1] + acaf4 * (TIME=1995)',
                 method = 'MAX_LIKELIHOOD',
                 from_period = '1990Y1',
                 to_period = '2015Y1',
                 block = 'ACAF',
                 tests = {corr = 1,
                          dw = 2.32935,
                          fstat = 32.2732,
                          loglik = 83.8075,
                          meany = 0.00818467,
                          r2 = 0.821761,
                          r2adj = 0.796299,
                          ssres = 5.19945e-05,
                          stderr = 0.00192715,
                          stderrp = 23.5458,
                          stdev = 0.0042699},
                 date = '12-06-1998')

        >>> # c) working on a subset
        >>> # 1) get subset
        >>> equations_subset = equations["A*"]
        >>> equations_subset.names
        ['ACAF', 'ACAG', 'AOUC']
        >>> # 2) add a equation to the subset 
        >>> equations_subset["AOUC_"] = "AOUC_ := ((WCRH/QL)/(WCRH/QL)[1990Y1]) * (VAFF/(VM+VAFF))[-1] + PM * (VM/(VAFF+VM))[-1]"
        >>> equations_subset["AOUC_"]               # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'AOUC_',
                 lec = 'AOUC_ := ((WCRH/QL)/(WCRH/QL)[1990Y1]) * (VAFF/(VM+VAFF))[-1] + PM * (VM/(VAFF+VM))[-1]',
                 method = 'LSQ',
                 from_period = '1960Y1',
                 to_period = '2015Y1')
        >>> # --> new equation also appears in the global workspace
        >>> "AOUC_" in equations
        True
        >>> equations["AOUC_"]                      # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'AOUC_',
                 lec = 'AOUC_ := ((WCRH/QL)/(WCRH/QL)[1990Y1]) * (VAFF/(VM+VAFF))[-1] + PM * (VM/(VAFF+VM))[-1]',
                 method = 'LSQ',
                 from_period = '1960Y1',
                 to_period = '2015Y1')
        >>> # 3) update a equation in the subset
        >>> equations_subset["AOUC_"] = "AOUC_ := ((WCRH/QL)/(WCRH/QL)[1990Y1]) * (VAFF/(VM+VAFF))[-1]"
        >>> equations_subset["AOUC_"]           # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'AOUC_',
                 lec = 'AOUC_ := ((WCRH/QL)/(WCRH/QL)[1990Y1]) * (VAFF/(VM+VAFF))[-1]',
                 method = 'LSQ',
                 from_period = '1960Y1',
                 to_period = '2015Y1')
        >>> # --> equation is also updated in the global workspace
        >>> equations["AOUC_"]                  # doctest: +NORMALIZE_WHITESPACE
        Equation(endogenous = 'AOUC_',
                 lec = 'AOUC_ := ((WCRH/QL)/(WCRH/QL)[1990Y1]) * (VAFF/(VM+VAFF))[-1]',
                 method = 'LSQ',
                 from_period = '1960Y1',
                 to_period = '2015Y1')

        Identities

        >>> from iode import identities
        >>> identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")
        >>> # a) add one identity
        >>> identities["BDY"] = "YN - YK"
        >>> identities["BDY"]
        Identity('YN - YK')

        >>> # b) update one identity
        >>> identities["AOUC"]
        Identity('((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/(VM+VAFF))[-1]')
        >>> identities["AOUC"] = '(WCRH / WCRH[1990Y1]) * (VAFF / (VM+VAFF))[-1] + PM * (VM / (VM+VAFF))[-1]'
        >>> identities["AOUC"]
        Identity('(WCRH / WCRH[1990Y1]) * (VAFF / (VM+VAFF))[-1] + PM * (VM / (VM+VAFF))[-1]')

        >>> # c) working on a subset
        >>> # 1) get subset
        >>> identities_subset = identities["X*"]
        >>> identities_subset.names
        ['XEX', 'XNATY', 'XPOIL', 'XPWMAB', 'XPWMS', 'XPWXAB', 'XPWXS', 'XQWXAB', 'XQWXS', 'XQWXSS', 'XRLBER', 'XTFP', 'XW']
        >>> # 2) add an identity to the subset 
        >>> identities_subset["XDPU"] = "grt DPU"
        >>> identities_subset["XDPU"]
        Identity('grt DPU')
        >>> # --> new identity also appears in the global workspace
        >>> "XDPU" in identities
        True
        >>> identities["XDPU"]
        Identity('grt DPU')
        >>> # 3) update an identity in the subset
        >>> identities_subset["XDPU"] = "0"
        >>> identities_subset["XDPU"]
        Identity('0')
        >>> # --> identity is also updated in the global workspace
        >>> identities["XDPU"]
        Identity('0')

        Lists

        >>> from iode import lists, variables
        >>> lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> # a) add one list
        >>> # --- by passing a string 
        >>> lists["A_VAR"] = "ACAF;ACAG;AOUC;AOUC_;AQC"
        >>> lists["A_VAR"]
        ['ACAF', 'ACAG', 'AOUC', 'AOUC_', 'AQC']
        >>> # --- by passing a Python list
        >>> b_vars = variables.get_names("B*")
        >>> b_vars
        ['BENEF', 'BQY', 'BRUGP', 'BVY']
        >>> lists["B_VAR"] = b_vars
        >>> lists["B_VAR"]
        ['BENEF', 'BQY', 'BRUGP', 'BVY']

        >>> # b) update one list
        >>> # --- by passing a string
        >>> lists["A_VAR"] = "ACAF;ACAG;AOUC;AQC"
        >>> lists["A_VAR"]
        ['ACAF', 'ACAG', 'AOUC', 'AQC']
        >>> # --- by passing a Python list
        >>> b_y_vars = variables.get_names("B*Y")
        >>> b_y_vars
        ['BQY', 'BVY']
        >>> lists["B_VAR"] = b_y_vars
        >>> lists["B_VAR"]
        ['BQY', 'BVY']

        >>> # c) working on a subset
        >>> # 1) get subset
        >>> lists_subset = lists["E*"]
        >>> lists_subset.names
        ['ENDO', 'ENDO0', 'ENDO1', 'ENVI']
        >>> # 2) add a list to the subset 
        >>> lists_subset["E_VAR"] = variables.get_names("E*")
        >>> lists_subset["E_VAR"]
        ['EFMY', 'EFXY', 'EX', 'EXC', 'EXCC', 'EXCCR']
        >>> # --> new list also appears in the global workspace
        >>> "E_VAR" in lists
        True
        >>> lists["E_VAR"]
        ['EFMY', 'EFXY', 'EX', 'EXC', 'EXCC', 'EXCCR']
        >>> # 3) update a list in the subset
        >>> lists_subset["E_VAR"] = "EX;EXC;EXCC;EXCCR"
        >>> lists_subset["E_VAR"]
        ['EX', 'EXC', 'EXCC', 'EXCCR']
        >>> # --> list is also updated in the global workspace
        >>> lists["E_VAR"]
        ['EX', 'EXC', 'EXCC', 'EXCCR']

        Scalars

        >>> from iode import scalars
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")
        >>> # a) -------- add one scalar -------- 
        >>> # 1. default relax to 1.0
        >>> scalars["a0"] = 0.1
        >>> scalars["a0"]
        Scalar(0.1, 1, na)
        >>> # 2. value + relax
        >>> scalars["a1"] = 0.1, 0.9
        >>> scalars["a1"]
        Scalar(0.1, 0.9, na)

        >>> # b) -------- update one scalar --------
        >>> scalars["acaf1"]
        Scalar(0.0157684, 1, 0.00136871)
        >>> # only update the value
        >>> scalars["acaf1"] = 0.8
        >>> scalars["acaf1"]
        Scalar(0.8, 1, 0.00136871)
        >>> # upate value and relax (tuple)
        >>> scalars["acaf2"] = 0.8, 0.9
        >>> scalars["acaf2"]
        Scalar(0.8, 0.9, na)
        >>> # update value and relax (list)
        >>> scalars["acaf2"] = (0.7, 0.8)
        >>> scalars["acaf2"]
        Scalar(0.7, 0.8, na)
        >>> # upate value and relax (dictionary)
        >>> scalars["acaf3"] = {"relax": 0.9, "value": 0.8}
        >>> scalars["acaf3"]
        Scalar(0.8, 0.9, 0.87301)
        >>> # upate value and/or relax (Scalar object)
        >>> acaf4 = scalars["acaf4"]
        >>> acaf4
        Scalar(-0.00850518, 1, 0.0020833)
        >>> acaf4.value = 0.8
        >>> acaf4.relax = 0.9
        >>> # WARNING: the standard deviation (std) cannot be changed manually
        >>> scalars["acaf4"] = acaf4
        >>> scalars["acaf4"]
        Scalar(0.8, 0.9, 0.0020833)

        >>> # c) working on a subset
        >>> # 1) get subset
        >>> scalars_subset = scalars["a*"]
        >>> scalars_subset.names
        ['a0', 'a1', 'acaf1', 'acaf2', 'acaf3', 'acaf4']
        >>> # 2) add a scalar to the subset 
        >>> scalars_subset["acaf0"] = 1.0, 1.0
        >>> scalars_subset["acaf0"]
        Scalar(1, 1, na)
        >>> # --> new scalar also appears in the global workspace
        >>> "acaf0" in scalars
        True
        >>> scalars["acaf0"]
        Scalar(1, 1, na)
        >>> # 3) update a scalar in the subset
        >>> scalars_subset["acaf0"] = 0.1
        >>> scalars_subset["acaf0"]
        Scalar(0.1, 1, na)
        >>> # --> scalar is also updated in the global workspace
        >>> scalars["acaf0"]
        Scalar(0.1, 1, na)

        Tables

        >>> from iode import tables, Table, TableGraphAxis
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
        >>> # a) -------- new table --------
        >>> # 1. specify list of line titles and list of LEC expressions
        >>> lines_titles = ["GOSG:", "YDTG:", "DTH:", "DTF:", "IT:", "YSSG+COTRES:", "RIDG:", "OCUG:"]
        >>> lines_lecs = ["GOSG", "YDTG", "DTH", "DTF", "IT", "YSSG+COTRES", "RIDG", "OCUG"]
        >>> tables["TABLE_CELL_LECS"] = {"nb_columns": 2, "table_title": "New Table", "lecs_or_vars": lines_lecs, 
        ...                         "lines_titles": lines_titles, "mode": True, "files": True, "date": True}  
        >>> tables["TABLE_CELL_LECS"]         # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1              |
        TITLE |         "New Table"
        ----- | ----------------------------
        CELL  | ""             |     "#S"
        ----- | ----------------------------
        CELL  | "GOSG:"        |        GOSG
        CELL  | "YDTG:"        |        YDTG
        CELL  | "DTH:"         |         DTH
        CELL  | "DTF:"         |         DTF
        CELL  | "IT:"          |          IT
        CELL  | "YSSG+COTRES:" | YSSG+COTRES
        CELL  | "RIDG:"        |        RIDG
        CELL  | "OCUG:"        |        OCUG
        ----- | ----------------------------
        MODE  |
        FILES |
        DATE  |
        <BLANKLINE>
        nb lines: 16
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>

        >>> # 2. specify list of variables
        >>> vars_list = ["GOSG", "YDTG", "DTH", "DTF", "IT", "YSSG", "COTRES", "RIDG", "OCUG", "$ENVI"]
        >>> tables["TABLE_VARS"] = {"nb_columns": 2, "table_title": "New Table", "lecs_or_vars": vars_list, 
        ...                         "mode": True, "files": True, "date": True}  
        >>> tables["TABLE_VARS"]             # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1                                                                    |
        TITLE |                                  "New Table"
        ----- | ------------------------------------------------------------------------------
        CELL  | ""                                                                   |   "#S"
        ----- | ------------------------------------------------------------------------------
        CELL  | "Bruto exploitatie-overschot: overheid (= afschrijvingen)."          |    GOSG
        CELL  | "Overheid: geïnde indirecte belastingen."                            |    YDTG
        CELL  | "Totale overheid: directe belasting van de gezinnen."                |     DTH
        CELL  | "Totale overheid: directe vennootschapsbelasting."                   |     DTF
        CELL  | "Totale indirecte belastingen."                                      |      IT
        CELL  | "Globale overheid: ontvangen sociale zekerheidsbijdragen."           |    YSSG
        CELL  | "Cotisation de responsabilisation."                                  |  COTRES
        CELL  | "Overheid: inkomen uit vermogen."                                    |    RIDG
        CELL  | "Globale overheid: saldo van de ontvangen lopendeoverdrachten."      |    OCUG
        CELL  | "Wisselkoers van de USD t.o.v. de BEF (jaargemiddelde)."             |      EX
        CELL  | "Index wereldprijs - invoer van niet-energieprodukten, inUSD."       |   PWMAB
        CELL  | "Index wereldprijs - invoer van diensten, in USD."                   |    PWMS
        CELL  | "Index wereldprijs - uitvoer van niet-energieprodukten, inUSD."      |   PWXAB
        CELL  | "Index wereldprijs - uitvoer van diensten, in USD."                  |    PWXS
        CELL  | "Indicator van het volume van de wereldvraag naar goederen,1985=1."  |   QWXAB
        CELL  | "Indicator van het volume van de wereldvraag naar diensten,1985=1."  |    QWXS
        CELL  | "Brent olieprijs (USD per barrel)."                                  |    POIL
        CELL  | "Totale beroepsbevolking (jaargemiddelde)."                          |    NATY
        CELL  | "TFPFHP_"                                                            | TFPFHP_
        ----- | ------------------------------------------------------------------------------
        MODE  |
        FILES |
        DATE  |
        <BLANKLINE>
        nb lines: 27
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>

        >>> # b) -------- update table --------
        >>> table = tables["TABLE_CELL_LECS"]
        >>> table                   # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1              |
        TITLE |         "New Table"
        ----- | ----------------------------
        CELL  | ""             |     "#S"
        ----- | ----------------------------
        CELL  | "GOSG:"        |        GOSG
        CELL  | "YDTG:"        |        YDTG
        CELL  | "DTH:"         |         DTH
        CELL  | "DTF:"         |         DTF
        CELL  | "IT:"          |          IT
        CELL  | "YSSG+COTRES:" | YSSG+COTRES
        CELL  | "RIDG:"        |        RIDG
        CELL  | "OCUG:"        |        OCUG
        ----- | ----------------------------
        MODE  |
        FILES |
        DATE  |
        <BLANKLINE>
        nb lines: 16
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>

        >>> # set graph axis type
        >>> table.graph_axis = TableGraphAxis.SEMILOG
        >>> # print last line
        >>> table[-1]
        <DATE>
        >>> # delete last line
        >>> del table[-1]
        >>> # get index of line containing YSSG+COTRES
        >>> index = table.index("YSSG+COTRES")
        >>> table[index]
        ('"YSSG+COTRES:"', 'YSSG+COTRES')
        >>> # get line type
        >>> table[index].line_type
        'CELL'
        >>> # get line graph type
        >>> table[index].graph_type
        'LINE'
        >>> # know if axis is left
        >>> table[index].axis_left
        True
        >>> # update cells
        >>> # double quotes "    -> STRING cell
        >>> # no double quotes " -> LEC cell
        >>> table[index] = ('"YSSG:"', 'YSSG')
        >>> table[index]
        ('"YSSG:"', 'YSSG')
        >>> # insert a new title line surrounded by two separator lines
        >>> table.insert(index + 1, '-')
        >>> table.insert(index + 2, "New Title")
        >>> table.insert(index + 3, '-')
        >>> # append a new sepatator line
        >>> table += '-'

        >>> # warning: do not forget to actually update the IODE Table database  
        >>> tables["TABLE_CELL_LECS"] = table
        >>> tables["TABLE_CELL_LECS"]                # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1       |
        TITLE |  "New Table"
        ----- | --------------
        CELL  | ""      | "#S"
        ----- | --------------
        CELL  | "GOSG:" | GOSG
        CELL  | "YDTG:" | YDTG
        CELL  | "DTH:"  |  DTH
        CELL  | "DTF:"  |  DTF
        CELL  | "IT:"   |   IT
        CELL  | "YSSG:" | YSSG
        ----- | --------------
        TITLE |  "New Title"
        ----- | --------------
        CELL  | "RIDG:" | RIDG
        CELL  | "OCUG:" | OCUG
        ----- | --------------
        MODE  |
        FILES |
        ----- | --------------
        <BLANKLINE>
        nb lines: 19
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'SEMILOG'
        graph_alignment: 'LEFT'
        <BLANKLINE>

        >>> # c) working on a subset
        >>> # 1) get subset
        >>> tables_subset = tables["C8_*"]
        >>> tables_subset.names
        ['C8_1', 'C8_10', 'C8_11', 'C8_13', 'C8_14', 'C8_2', 'C8_3', 'C8_4', 'C8_5', 'C8_6', 'C8_7', 'C8_8', 'C8_9']
        >>> # 2) add a table to the subset 
        >>> vars_list = ["XNATY", "XPOIL", "XPWMAB", "XPWXAB"]
        >>> tables_subset["X_GRT"] = {"nb_columns": 2, "table_title": "Croissance", "lecs_or_vars": vars_list, 
        ...                           "mode": True, "files": True, "date": True}  
        >>> tables_subset["X_GRT"]                      # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1                                                       |
        TITLE |                           "Croissance"
        ----- | ----------------------------------------------------------------
        CELL  | ""                                                      |  "#S"
        ----- | ----------------------------------------------------------------
        CELL  | "Croissance de la population active"                    |  XNATY
        CELL  | "Croissance du prix du pétrole"                         |  XPOIL
        CELL  | "Croissance des prix des biens importés"                | XPWMAB
        CELL  | "Croissance des prix des marchés pertinents à l'export" | XPWXAB
        ----- | ----------------------------------------------------------------
        MODE  |
        FILES |
        DATE  |
        <BLANKLINE>
        nb lines: 12
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>
        >>> # --> new table also appears in the global workspace
        >>> "X_GRT" in tables
        True
        >>> tables["X_GRT"]                             # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1                                                       |
        TITLE |                           "Croissance"
        ----- | ----------------------------------------------------------------
        CELL  | ""                                                      |  "#S"
        ----- | ----------------------------------------------------------------
        CELL  | "Croissance de la population active"                    |  XNATY
        CELL  | "Croissance du prix du pétrole"                         |  XPOIL
        CELL  | "Croissance des prix des biens importés"                | XPWMAB
        CELL  | "Croissance des prix des marchés pertinents à l'export" | XPWXAB
        ----- | ----------------------------------------------------------------
        MODE  |
        FILES |
        DATE  |
        <BLANKLINE>
        nb lines: 12
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>
        >>> # 3) update a table in the subset
        >>> table_x_grt = tables_subset["X_GRT"]
        >>> index = table_x_grt.index("XPWXAB")
        >>> table_x_grt.insert(index + 1, (f'"{comments["XQWXSS"]}"', "XQWXSS"))
        >>> # warning: do not forget to actually update the IODE Table subset
        >>> tables_subset["X_GRT"] = table_x_grt
        >>> tables_subset["X_GRT"]                      # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1                                                       |
        TITLE |                           "Croissance"
        ----- | ----------------------------------------------------------------
        CELL  | ""                                                      |  "#S"
        ----- | ----------------------------------------------------------------
        CELL  | "Croissance de la population active"                    |  XNATY
        CELL  | "Croissance du prix du pétrole"                         |  XPOIL
        CELL  | "Croissance des prix des biens importés"                | XPWMAB
        CELL  | "Croissance des prix des marchés pertinents à l'export" | XPWXAB
        CELL  | "Croissance des marchés pertinents"                     | XQWXSS
        ----- | ----------------------------------------------------------------
        MODE  |
        FILES |
        DATE  |
        <BLANKLINE>
        nb lines: 13
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>
        >>> # --> table is also updated in the global workspace
        >>> tables["X_GRT"]                             # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1                                                       |
        TITLE |                           "Croissance"
        ----- | ----------------------------------------------------------------
        CELL  | ""                                                      |  "#S"
        ----- | ----------------------------------------------------------------
        CELL  | "Croissance de la population active"                    |  XNATY
        CELL  | "Croissance du prix du pétrole"                         |  XPOIL
        CELL  | "Croissance des prix des biens importés"                | XPWMAB
        CELL  | "Croissance des prix des marchés pertinents à l'export" | XPWXAB
        CELL  | "Croissance des marchés pertinents"                     | XQWXSS
        ----- | ----------------------------------------------------------------
        MODE  |
        FILES |
        DATE  |
        <BLANKLINE>
        nb lines: 13
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>

        Variables

        >>> from iode import variables, NA
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> # a) -------- add one variable --------
        >>> # 1) same value for all periods
        >>> variables["A0"] = NA
        >>> variables["A0"]                     # doctest: +ELLIPSIS 
        [-2e+37, -2e+37, ..., -2e+37, -2e+37]
        >>> # 2) vector (list) containing a specific value for each period
        >>> variables["A1"] = list(range(variables.nb_periods))
        >>> variables["A1"]                     # doctest: +ELLIPSIS 
        [0.0, 1.0, 2.0, ..., 53.0, 54.0, 55.0]
        >>> # 3) LEC expression
        >>> variables["A2"] = "t + 10"
        >>> variables["A2"]                     # doctest: +ELLIPSIS 
        [10.0, 11.0, 12.0, ..., 63.0, 64.0, 65.0]

        >>> # b) -------- update one variable --------
        >>> # 1) update all values of a Variable
        >>> variables["ACAF"]                   # doctest: +ELLIPSIS 
        [-2e+37, -2e+37, ..., -83.34062511080091, -96.41041982848331]
        >>> # 1.I) same value for all periods
        >>> variables["ACAF"] = NA
        >>> variables["ACAF"]                   # doctest: +ELLIPSIS 
        [-2e+37, -2e+37, ..., -2e+37, -2e+37]
        >>> # 1.II) vector (list) containing a specific value for each period
        >>> variables["ACAF"] = list(range(variables.nb_periods))
        >>> variables["ACAF"]                   # doctest: +ELLIPSIS 
        [0.0, 1.0, 2.0, ..., 53.0, 54.0, 55.0]
        >>> # 1.III) LEC expression
        >>> variables["ACAF"] = "t + 10"
        >>> variables["ACAF"]                   # doctest: +ELLIPSIS 
        [10.0, 11.0, 12.0, ..., 63.0, 64.0, 65.0]

        >>> # 2) set one value of a Variable for a specific period
        >>> variables["ACAG", "1990Y1"]
        -28.1721855713507
        >>> variables["ACAG", "1990Y1"] = -28.2
        >>> variables["ACAG", "1990Y1"]
        -28.2

        >>> # 3) set the variable values for range of periods 
        >>> # 3.I) using a Python slice
        >>> # 3.I.a) variable(periods) = same value for all periods
        >>> variables["ACAF", "1991Y1":"1995Y1"] = 0.0
        >>> variables["ACAF", "1991Y1":"1995Y1"]
        [0.0, 0.0, 0.0, 0.0, 0.0]
        >>> # 3.I.b) variable(periods) = vector (list) containing a specific value for each period
        >>> variables["ACAF", "1991Y1":"1995Y1"] = [0., 1., 2., 3., 4.]
        >>> variables["ACAF", "1991Y1":"1995Y1"]
        [0.0, 1.0, 2.0, 3.0, 4.0]
        >>> # 3.I.c) variable(periods) = LEC expression
        >>> variables["ACAF", "1991Y1":"1995Y1"] = "t + 10"
        >>> variables["ACAF", "1991Y1":"1995Y1"]
        [41.0, 42.0, 43.0, 44.0, 45.0]

        >>> # 3.II) same as above but with the colon ':' inside the periods range string
        >>> # 3.II.a) variable(periods) = same value for all periods
        >>> variables["ACAF", "1991Y1:1995Y1"] = 0.0
        >>> variables["ACAF", "1991Y1:1995Y1"]
        [0.0, 0.0, 0.0, 0.0, 0.0]
        >>> # 3.II.b) variable(periods) = vector (list) containing a specific value for each period
        >>> variables["ACAF", "1991Y1:1995Y1"] = [0., -1., -2., -3., -4.]
        >>> variables["ACAF", "1991Y1":"1995Y1"]
        [0.0, -1.0, -2.0, -3.0, -4.0]
        >>> # 3.II.c) variable(periods) = LEC expression
        >>> variables["ACAF", "1991Y1:1995Y1"] = "t - 10"
        >>> variables["ACAF", "1991Y1:1995Y1"]
        [21.0, 22.0, 23.0, 24.0, 25.0]

        >>> # c) -------- working on a subset --------
        >>> # 1) get subset
        >>> variables_subset = variables["A*"]
        >>> variables_subset.names
        ['A0', 'A1', 'A2', 'ACAF', 'ACAG', 'AOUC', 'AOUC_', 'AQC']
        >>> # 2) add a variable to the subset 
        >>> from iode import NA
        >>> variables_subset["A3"] = NA
        >>> variables_subset["A3"]              # doctest: +ELLIPSIS 
        [-2e+37, -2e+37, ..., -2e+37, -2e+37]
        >>> # --> new variable also appears in the global workspace
        >>> "A3" in variables
        True
        >>> variables["A3"]                     # doctest: +ELLIPSIS 
        [-2e+37, -2e+37, ..., -2e+37, -2e+37]
        >>> # 3) update a variable in the subset
        >>> variables_subset["A3"] = 0.0
        >>> variables_subset["A3"]              # doctest: +ELLIPSIS 
        [0.0, 0.0, ..., 0.0, 0.0]
        >>> # --> variable is also updated in the global workspace
        >>> variables["A3"]                     # doctest: +ELLIPSIS 
        [0.0, 0.0, ..., 0.0, 0.0]
        """
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

    def __delitem__(self, key):
        """
        Remove the (subset of) IODE object(s) referenced by `key` from the current database.

        Parameters
        ----------
        key: str or list(str)
            (list of) name(s) of the IODE object(s) to be removed.
            The list of names can be given as a string pattern (e.g. "A*;*_").

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments, variables

        Comments

        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> comments.get_names("A*")
        ['ACAF', 'ACAG', 'AOUC', 'AQC']

        >>> # a) delete one comment
        >>> del comments["ACAF"]
        >>> comments.get_names("A*")
        ['ACAG', 'AOUC', 'AQC']

        >>> # b) delete several comments at once using a pattern
        >>> del comments["A*"]
        >>> comments.get_names("A*")
        []

        >>> # c) delete several comments at once using a list of names
        >>> comments.get_names("B*")
        ['BENEF', 'BENEF_', 'BQY', 'BVY']
        >>> del comments[["BENEF", "BQY"]]
        >>> comments.get_names("B*")
        ['BENEF_', 'BVY']

        >>> # delete one comment from a subset of the global database
        >>> comments_subset = comments["D*"]
        >>> comments_subset.names
        ['DPU', 'DPUF', 'DPUG', 'DPUGO', 'DPUH', 'DPUU', 'DTF', 'DTFX', 'DTH', 'DTH1', 'DTH1C', 'DTHX']
        >>> del comments_subset["DPUGO"]
        >>> comments_subset.names
        ['DPU', 'DPUF', 'DPUG', 'DPUH', 'DPUU', 'DTF', 'DTFX', 'DTH', 'DTH1', 'DTH1C', 'DTHX']
        >>> # comment also deleted in the globale database
        >>> "DPUGO" in comments
        False
        >>> comments.get_names("D*")
        ['DPU', 'DPUF', 'DPUG', 'DPUH', 'DPUU', 'DTF', 'DTFX', 'DTH', 'DTH1', 'DTH1C', 'DTHX']

        Variables

        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> variables.get_names("A*")
        ['ACAF', 'ACAG', 'AOUC', 'AOUC_', 'AQC']

        >>> # a) delete one variable
        >>> del variables["ACAF"]
        >>> variables.get_names("A*")
        ['ACAG', 'AOUC', 'AOUC_', 'AQC']

        >>> # b) delete several variables at once using a pattern
        >>> del variables["A*"]
        >>> variables.get_names("A*")
        []

        >>> # c) delete several variables at once using a list of names
        >>> variables.get_names("B*")
        ['BENEF', 'BQY', 'BRUGP', 'BVY']
        >>> del variables[["BENEF", "BQY"]]
        >>> variables.get_names("B*")
        ['BRUGP', 'BVY']

        >>> # delete one variable from a subset of the global database
        >>> variables_subset = variables["D*"]
        >>> variables_subset.names
        ['DEBT', 'DPU', 'DPUF', 'DPUG', 'DPUGO', 'DPUH', 'DPUHO', 'DPUU', 'DTF', 'DTFX', 'DTH', 'DTH1', 'DTH1C', 'DTHX']
        >>> del variables_subset["DPUGO"]
        >>> variables_subset.names
        ['DEBT', 'DPU', 'DPUF', 'DPUG', 'DPUH', 'DPUHO', 'DPUU', 'DTF', 'DTFX', 'DTH', 'DTH1', 'DTH1C', 'DTHX']
        >>> # variable also deleted in the globale database
        >>> "DPUGO" in variables
        False
        >>> variables.get_names("D*")
        ['DEBT', 'DPU', 'DPUF', 'DPUG', 'DPUH', 'DPUHO', 'DPUU', 'DTF', 'DTFX', 'DTH', 'DTH1', 'DTH1C', 'DTHX']
        """
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
