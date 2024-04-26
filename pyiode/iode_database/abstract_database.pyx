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
    @property
    def iode_type(self) -> str:
        """
        Return the IODE type of the current database

        Returns
        -------
        str

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Comments
        >>> cmt_db = Comments(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> cmt_db.iode_type
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
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Comments
        >>> cmt_db = Comments(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> cmt_db.is_subset()
        False
        >>> cmt_subset = cmt_db.subset("A*")
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
        >>> from iode import Comments
        >>> cmt_db = Comments(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> cmt_db.is_subset()
        False
        >>> # by default a 'shallow copy' subset is returned
        >>> cmt_subset = cmt_db.subset("A*")
        >>> cmt_subset.is_subset()
        True
        >>> cmt_subset.is_copy_subset()
        False
        >>> # force to return a 'deep copy' subset
        >>> cmt_subset = cmt_db.subset("A*", copy=True)
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
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Comments
        >>> cmt_db = Comments(f"{SAMPLE_DATA_DIR}/fun.cmt")

        >>> # create a subset with all comments with name starting with 'A'
        >>> cmt_subset = cmt_db.subset("A*")
        >>> cmt_subset.get_names()
        ['ACAF', 'ACAG', 'AOUC', 'AQC']
        >>> # any modification made on the subset is visible in the global database
        >>> cmt_subset['ACAF'] = "Modified Comment"
        >>> cmt_db['ACAF']
        'Modified Comment'

        >>> # force to return a 'deep copy' subset
        >>> cmt_subset = cmt_db.subset("*_", copy=True)
        >>> # any modification made on the subset let the global database unchanged
        >>> cmt_subset['BENEF_'] = "Modified Comment"
        >>> cmt_db['BENEF_']
        'Ondernemingen: niet-uitgekeerde winsten (vóór statistische\\naanpassing).'
        """
        raise NotImplementedError()

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
        >>> from iode import Comments
        >>> from pathlib import Path
        >>> from os.path import relpath
        >>> cmt_db = Comments(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> filepath = cmt_db.filename
        >>> Path(filepath).name
        'fun.cmt'
        """
        return self.abstract_db_ptr.get_filename().decode()

    @property
    def description(self) -> str:
        """
        Return the description of the current database

        Returns
        -------
        str
        
        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Comments
        >>> cmt_db = Comments(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> cmt_db.description = "test data from file 'fun.cmt'"
        >>> cmt_db.description
        "test data from file 'fun.cmt'"
        """
        return self.abstract_db_ptr.get_description().decode()

    @description.setter
    def description(self, value: str):
        """
        Set the description of the current database.

        Parameters
        ----------
        value: str
            New description.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Comments
        >>> cmt_db = Comments(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> cmt_db.description = "test data from file 'fun.cmt'"
        >>> cmt_db.description
        "test data from file 'fun.cmt'"
        """
        if not isinstance(value, str):
            raise TypeError(f"'description': Expected value of type string. Got value of type {type(value).__name__}")
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
        >>> from iode import Comments
        >>> cmt_db = Comments(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> cmt_db.get_names("A*;*_")         # doctest: +NORMALIZE_WHITESPACE
        ['ACAF', 'ACAG', 'AOUC', 'AQC', 'BENEF_', 'GOSH_', 
        'IDH_', 'PAFF_', 'PC_', 'PFI_', 'QAFF_', 'QAF_', 
        'QAI_', 'QAT_', 'QBBPPOT_', 'QC_', 'QQMAB_', 'QS_', 
        'Q_', 'TFPHP_', 'VAFF_', 'VAI_', 'VAT_', 'VC_', 'VS_', 
        'WBF_', 'WBU_', 'WCF_', 'WCR1_', 'WCR2_', 'WIND_', 
        'WNF_', 'YDH_', 'ZZ_']
        >>> # or equivalently
        >>> cmt_db.get_names(["A*", "*_"])    # doctest: +NORMALIZE_WHITESPACE
        ['ACAF', 'ACAG', 'AOUC', 'AQC', 'BENEF_', 'GOSH_', 
        'IDH_', 'PAFF_', 'PC_', 'PFI_', 'QAFF_', 'QAF_', 
        'QAI_', 'QAT_', 'QBBPPOT_', 'QC_', 'QQMAB_', 'QS_', 
        'Q_', 'TFPHP_', 'VAFF_', 'VAI_', 'VAT_', 'VC_', 'VS_', 
        'WBF_', 'WBU_', 'WCF_', 'WCR1_', 'WCR2_', 'WIND_', 
        'WNF_', 'YDH_', 'ZZ_']
        >>> # get the list of all names
        >>> cmt_db.get_names()                # doctest: +ELLIPSIS
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
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Comments
        >>> cmt_db = Comments(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> cmt_db["ACAF"]
        'Ondernemingen: ontvangen kapitaaloverdrachten.'

        >>> # rename comment 'ACAF' as 'ACCAF'
        >>> cmt_db.rename("ACAF", "ACCAF")
        >>> "ACCAF" in cmt_db
        True
        >>> cmt_db["ACCAF"]
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
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Comments
        >>> cmt_db = Comments(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> cmt_db.get_names("A*")
        ['ACAF', 'ACAG', 'AOUC', 'AQC']

        >>> # remove one object
        >>> cmt_db.remove("ACAF")
        >>> cmt_db.get_names("A*")
        ['ACAG', 'AOUC', 'AQC']

        >>> # remove all objects with a name ending by '_'
        >>> cmt_db.get_names("*_")            # doctest: +ELLIPSIS
        ['BENEF_', 'GOSH_', 'IDH_', ..., 'WNF_', 'YDH_', 'ZZ_']
        >>> cmt_db.remove("*_")
        >>> cmt_db.get_names("*_")
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
        >>> from iode import Comments
        >>> cmt_db = Comments(f"{{SAMPLE_DATA_DIR}}/fun.cmt")

        >>> # copy comments with names starting with 'A' into a new database 'cmt_subset'
        >>> cmt_subset = cmt_db.subset("A*", deep_copy=True)
        >>> cmt_subset.get_names()
        ['ACAF', 'ACAG', 'ACOUG', 'AQC']

        >>> # remove 'ACAF' and 'ACAG' from the global Comments database
        >>> cmt_db.remove('ACAF;ACAG')
        >>> cmt_db.get_names("A*")
        ['ACOUG']

        >>> # update the content of 'ACOUG' in 'cmt_subset'
        >>> cmt_subset['ACOUG'] = "Comment modified"
        >>> cmt_subset['ACOUG']
        'Comment modified'
        >>> # content of 'ACOUG' in the global Comments database
        >>> cmt_db['ACOUG']
        ''

        >>> # merge 'cmt_subset' into the global Comments database
        >>> # preserve 'ACOUG' in the global Comments database
        >>> cmt_db.merge(cmt_subset, overwrite=False)
        >>> cmt_db.contains('ACAF')
        True
        >>> cmt_db.contains('ACAG')
        True
        >>> cmt_db['ACOUG']
        ''

        >>> # merge 'cmt_subset' into the global Comments database
        >>> # overwrite the content of 'ACOUG' in the global Comments database 
        >>> cmt_db.merge(cmt_subset)
        >>> cmt_db['ACOUG']
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
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Comments
        >>> cmt_db = Comments(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> len(cmt_db)
        317
        >>> # delete all comments with a name starting with 'A'
        >>> cmt_db.remove("A*")
        >>> cmt_db.get_names("A*")
        []

        >>> # load all comments with a name starting with 'A'
        >>> cmt_db.copy_into(f"{SAMPLE_DATA_DIR}/fun.cmt", "A*")
        >>> cmt_db.get_names("A*")
        ['ACAF', 'ACAG', 'AOUC', 'AQC']

        >>> cmt_db.clear()
        >>> # load all comments
        >>> cmt_db.copy_into(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> len(cmt_db)
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
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Comments
        >>> cmt_db = Comments(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> len(cmt_db)
        317
        >>> # delete all comments
        >>> cmt_db.clear()
        >>> len(cmt_db)
        0

        >>> # reload all comments
        >>> cmt_db.merge_into(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> len(cmt_db)
        317
        """
        if not isinstance(input_file, str):
            raise TypeError(f"'input_file': Expected value of type string. Got value of type {type(input_file).__name__}")
        # convert relative path to absolute path
        input_file = str(Path(input_file).resolve())
        self.abstract_db_ptr.merge_into(input_file.encode())

    def get_associated_objects_list(self, name: str, other_type: int):
        r"""
        Return the list of all objects of type 'other_type' associated with the object named 'name' 
        from the current database.

        Returns
        -------
        list(str)

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, COMMENTS, EQUATIONS, LISTS, SCALARS, TABLES, VARIABLES
        >>> from iode import Equations, Lists, Scalars, Tables, Variables
        >>> eqs_db = Equations(f"{SAMPLE_DATA_DIR}/fun.eqs")
        >>> lst_db = Lists(f"{SAMPLE_DATA_DIR}/fun.lst")
        >>> scl_db = Scalars(f"{SAMPLE_DATA_DIR}/fun.scl")
        >>> tbl_db = Tables(f"{SAMPLE_DATA_DIR}/fun.tbl")
        >>> var_db = Variables(f"{SAMPLE_DATA_DIR}/fun.var")

        >>> # get list of comments associated with the variable 'AOUC'
        >>> var_db.get_associated_objects_list("AOUC", COMMENTS)
        ['AOUC']

        >>> # get list of equations associated with the variable 'AOUC'
        >>> var_db.get_associated_objects_list("AOUC", EQUATIONS)
        ['AOUC', 'PC', 'PIF', 'PXS', 'QXAB']

        >>> # get list of lists associated with the variable 'AOUC'
        >>> var_db.get_associated_objects_list("AOUC", LISTS)
        ['COPY0', 'ENDO0', 'TOTAL0']

        >>> # get list of tables associated with the variable 'AOUC'
        >>> var_db.get_associated_objects_list("AOUC", TABLES)
        ['ANAPRIX', 'MULT1FR', 'MULT1RESU', 'T1', 'T1NIVEAU']

        >>> # WARNING: In the case of Equations, the get_associated_objects_list method must not
        >>> #          be confused with the get_coefficients_list and get_variables_list methods
        >>> eqs_db["ACAF"].lec
        '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)'
        >>> eqs_db["ACAF"].get_coefficients_list()
        ['acaf1', 'acaf2', 'acaf4']
        >>> eqs_db.get_associated_objects_list("ACAF", SCALARS)
        []
        >>> eqs_db["ACAF"].get_variables_list()
        ['ACAF', 'VAF', 'GOSF', 'TIME']
        >>> eqs_db.get_associated_objects_list("ACAF", VARIABLES)
        ['ACAF']
        """
        if not isinstance(name, str):
            raise TypeError(f"'name': Expected value of type string. Got value of type {type(name).__name__}")

        if not isinstance(other_type, int):
            raise TypeError(f"'other_type': Expected value of type int. Got value of type {type(other_type).__name__}")

        return [name_other.decode() for name_other in self.abstract_db_ptr.get_associated_objects_list(name.encode(), <EnumIodeType>other_type)]

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
        >>> from iode import Comments, Variables
        >>> cmt_db = Comments(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> len(cmt_db)
        317

        >>> var_db = Variables(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> len(var_db)
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
        >>> from iode import Comments
        >>> cmt_db = Comments(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> len(cmt_db)
        317
        >>> cmt_db.save(f"{SAMPLE_DATA_DIR}/fun2.cmt")
        >>> cmt_db.clear()
        >>> cmt_db = Comments(f"{SAMPLE_DATA_DIR}/fun2.cmt")
        >>> len(cmt_db)
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
        >>> from iode import Comments
        >>> cmt_db = Comments(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> len(cmt_db)
        317
        >>> cmt_db.clear()
        >>> len(cmt_db)
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
        >>> from iode import Comments
        >>> cmt_db = Comments(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> len(cmt_db)
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
        >>> from iode import Comments
        >>> cmt_db = Comments(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> "ACAF" in cmt_db
        True
        >>> "ZCAF" in cmt_db
        False
        """
        if not isinstance(item, str):
            raise TypeError(f"Expected value of type string. Got value of type {type(item).__name__}")
        return self.abstract_db_ptr.contains(item.encode())

    def _get_object(self, key):
        raise NotImplementedError()

    def __getitem__(self, key):
        """
        Return the IODE object named `key` from the current database.

        Parameters
        ----------
        key: str
            name of the IODE object to get.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR

        Comments

        >>> from iode import Comments
        >>> cmt_db = Comments(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> cmt_db["ACAF"]
        'Ondernemingen: ontvangen kapitaaloverdrachten.'

        Equations

        >>> from iode import Equations
        >>> eqs_db = Equations(f"{SAMPLE_DATA_DIR}/fun.eqs")
        >>> eqs_db["ACAF"]                  # doctest: +NORMALIZE_WHITESPACE
        Equation(lec: (ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+
        acaf4*(TIME=1995),
                method: LSQ,
                sample: 1980Y1:1996Y1,
                comment: ,
                block: ACAF,
                instruments: ,
                tests:
                    corr: 1
                    dw: 2.32935
                    fstat: 32.2732
                    loglik: 83.8075
                    meany: 0.00818467
                    r2: 0.821761
                    r2adj: 0.796299
                    ssres: 5.19945e-05
                    stderr: 0.00192715
                    stderrp: 23.5458
                    stdev: 0.0042699
                date: 12-06-1998)

        Identities

        >>> from iode import Identities
        >>> idt_db = Identities(f"{SAMPLE_DATA_DIR}/fun.idt")
        >>> idt_db["AOUC"]
        '((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/(VM+VAFF))[-1]'

        Lists

        >>> from iode import Lists
        >>> lst_db = Lists(f"{SAMPLE_DATA_DIR}/fun.lst")
        >>> lst_db["ENVI"]
        'EX;PWMAB;PWMS;PWXAB;PWXS;QWXAB;QWXS;POIL;NATY;TFPFHP_'

        Scalars

        >>> from iode import Scalars
        >>> scl_db = Scalars(f"{SAMPLE_DATA_DIR}/fun.scl")
        >>> acaf1 = scl_db["acaf1"]
        >>> acaf1
        Scalar(0.0157684, 1, 0.00136871)
        >>> acaf1.value
        0.01576840691268444
        >>> acaf1.relax
        1.0
        >>> acaf1.std
        0.0013687137980014086

        Tables

        >>> from iode import Tables
        >>> tbl_db = Tables(f"{SAMPLE_DATA_DIR}/fun.tbl")
        >>> tbl_db["YDH"]           # doctest: +NORMALIZE_WHITESPACE
        DIVIS |                                                          1 |                                    PC_*40.34
        TITLE |                        "Tableau B-3. Revenu disponible des ménages à prix constant"
        ----- | ---------------------------------------------------------------------------------------------------------
        CELL  | ""                                                         |                     "#S"
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
        language: 'English'
        gridx: 'major'
        gridy: 'major'
        graph_axis: 'values'
        graph_alignment: 'left'
        <BLANKLINE>        

        Variables

        >>> from iode import Variables, nan
        >>> var_db = Variables(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> var_db.sample
        1960Y1:2015Y1
        >>> # get the variable values for the whole sample
        >>> var_db["ACAF"]                       # doctest: +ELLIPSIS 
        [-2e+37, -2e+37, ..., -83.34062511080091, -96.41041982848331]
        >>> # get the variable value for a specific period
        >>> var_db["ACAF", "1990Y1"]
        23.771
        >>> # get the variable values for range of periods (using a Python slice)
        >>> var_db["ACAF", "1990Y1":"2000Y1"]    # doctest: +ELLIPSIS 
        [23.771, 26.240999, ..., 13.530404919696034, 10.046610792200543]
        >>> # same as above but with the colon ':' inside the periods range string
        >>> var_db["ACAF", "1990Y1:2000Y1"]      # doctest: +ELLIPSIS 
        [23.771, 26.240999, ..., 13.530404919696034, 10.046610792200543]
        """
        return self._get_object(key) 

    def _set_object(self, key, value):
        raise NotImplementedError()

    def __setitem__(self, key, value):
        """
        Update/add an IODE object named `key` from/to the current database.

        Parameters
        ----------
        key: str
            name of the IODE object to update/add.
        value: 
            (new) value of the IODE object. 

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR

        Comments

        >>> from iode import Comments
        >>> cmt_db = Comments(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> cmt_db["ACAF"]
        'Ondernemingen: ontvangen kapitaaloverdrachten.'
        >>> cmt_db["ACAF"] = "New Value"
        >>> cmt_db["ACAF"]
        'New Value'

        Equations

        >>> from iode import Equations
        >>> eqs_db = Equations(f"{SAMPLE_DATA_DIR}/fun.eqs")
        >>> eqs_db["ACAF"]                  # doctest: +NORMALIZE_WHITESPACE
        Equation(lec: (ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+
        acaf4*(TIME=1995),
                method: LSQ,
                sample: 1980Y1:1996Y1,
                comment: ,
                block: ACAF,
                instruments: ,
                tests:
                    corr: 1
                    dw: 2.32935
                    fstat: 32.2732
                    loglik: 83.8075
                    meany: 0.00818467
                    r2: 0.821761
                    r2adj: 0.796299
                    ssres: 5.19945e-05
                    stderr: 0.00192715
                    stderrp: 23.5458
                    stdev: 0.0042699
                date: 12-06-1998)
        >>> # update only the LEC
        >>> eqs_db["ACAF"] = "(ACAF/VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)"
        >>> eqs_db["ACAF"]                  # doctest: +NORMALIZE_WHITESPACE
        Equation(lec: (ACAF/VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995),
                method: LSQ,
                sample: 1980Y1:1996Y1,
                comment: ,
                block: ACAF,
                instruments: ,
                tests:
                    corr: 1
                    dw: 2.32935
                    fstat: 32.2732
                    loglik: 83.8075
                    meany: 0.00818467
                    r2: 0.821761
                    r2adj: 0.796299
                    ssres: 5.19945e-05
                    stderr: 0.00192715
                    stderrp: 23.5458
                    stdev: 0.0042699
                date: 12-06-1998)
        >>> # upate block and sample of a block of equations to estimation (dictionary)
        >>> estim_sample = "2000Y1:2010Y1"
        >>> block = "ACAF; ACAG; AOUC"
        >>> for eq_name in block.split(';'):
        ...     eqs_db[eq_name] = {"sample": estim_sample, "block": block}
        >>> (eqs_db["ACAF"].sample, eqs_db["ACAG"].sample, eqs_db["AOUC"].sample)
        (2000Y1:2010Y1, 2000Y1:2010Y1, 2000Y1:2010Y1)
        >>> (eqs_db["ACAF"].block, eqs_db["ACAG"].block, eqs_db["AOUC"].block)
        ('ACAF; ACAG; AOUC', 'ACAF; ACAG; AOUC', 'ACAF; ACAG; AOUC')
        >>> # upate sample and block (Equation objects)
        >>> eq_ACAF = eqs_db["ACAF"]
        >>> eq_ACAF.set_lec("(ACAF/VAF[-1]) := acaf2 * GOSF[-1] + acaf4 * (TIME=1995)", "ACAF")
        >>> eq_ACAF.method = "MAX_LIKELIHOOD"
        >>> # new equation sample is from 1990Y1 to the last year of Variables
        >>> eq_ACAF.sample = "1990Y1:"
        >>> eq_ACAF.block = "ACAF"
        >>> eqs_db["ACAF"] = eq_ACAF
        >>> eqs_db["ACAF"]                  # doctest: +NORMALIZE_WHITESPACE
        Equation(lec: (ACAF/VAF[-1]) := acaf2 * GOSF[-1] + acaf4 * (TIME=1995),
                method: MAX_LIKELIHOOD,
                sample: 1990Y1:2015Y1,
                comment: ,
                block: ACAF,
                instruments: ,
                tests:
                    corr: 1
                    dw: 2.32935
                    fstat: 32.2732
                    loglik: 83.8075
                    meany: 0.00818467
                    r2: 0.821761
                    r2adj: 0.796299
                    ssres: 5.19945e-05
                    stderr: 0.00192715
                    stderrp: 23.5458
                    stdev: 0.0042699
                date: 12-06-1998)

        Identities

        >>> from iode import Identities
        >>> idt_db = Identities(f"{SAMPLE_DATA_DIR}/fun.idt")
        >>> idt_db["AOUC"]
        '((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/(VM+VAFF))[-1]'
        >>> idt_db["AOUC"] = '(WCRH / WCRH[1990Y1]) * (VAFF / (VM+VAFF))[-1] + PM * (VM / (VM+VAFF))[-1]'
        >>> idt_db["AOUC"]
        '(WCRH / WCRH[1990Y1]) * (VAFF / (VM+VAFF))[-1] + PM * (VM / (VM+VAFF))[-1]'

        Lists

        >>> from iode import Lists
        >>> lst_db = Lists(f"{SAMPLE_DATA_DIR}/fun.lst")
        >>> lst_db["ENVI"]
        'EX;PWMAB;PWMS;PWXAB;PWXS;QWXAB;QWXS;POIL;NATY;TFPFHP_'
        >>> lst_db["ENVI"] = 'PWMAB;PWMS;PWXAB;PWXS;QWXAB;QWXS;POIL;NATY'
        >>> lst_db["ENVI"]
        'PWMAB;PWMS;PWXAB;PWXS;QWXAB;QWXS;POIL;NATY'

        Scalars

        >>> from iode import Scalars
        >>> scl_db = Scalars(f"{SAMPLE_DATA_DIR}/fun.scl")
        >>> scl_db["acaf1"]
        Scalar(0.0157684, 1, 0.00136871)
        >>> # only update the value
        >>> scl_db["acaf1"] = 0.8
        >>> scl_db["acaf1"]
        Scalar(0.8, 1, 0.00136871)
        >>> # upate value and relax (tuple)
        >>> scl_db["acaf2"] = 0.8, 0.9
        >>> scl_db["acaf2"]
        Scalar(0.8, 0.9, nan)
        >>> # upate value and relax (list)
        >>> scl_db["acaf2"] = (0.7, 0.8)
        >>> scl_db["acaf2"]
        Scalar(0.7, 0.8, nan)
        >>> # upate value and relax (dictionary)
        >>> scl_db["acaf3"] = {"relax": 0.9, "value": 0.8}
        >>> scl_db["acaf3"]
        Scalar(0.8, 0.9, 0.87301)
        >>> # upate value and/or relax (Scalar object)
        >>> acaf4 = scl_db["acaf4"]
        >>> acaf4
        Scalar(-0.00850518, 1, 0.0020833)
        >>> acaf4.value = 0.8
        >>> acaf4.relax = 0.9
        >>> # WARNING: the standard deviation (std) cannot be changed manually
        >>> scl_db["acaf4"] = acaf4
        >>> scl_db["acaf4"]
        Scalar(0.8, 0.9, 0.0020833)

        Tables

        >>> from iode import Tables, Table
        >>> tbl_db = Tables(f"{SAMPLE_DATA_DIR}/fun.tbl")

        >>> # -- new table --
        >>> # 1. specify list of line titles and list of LEC expressions
        >>> lines_titles = ["GOSG:", "YDTG:", "DTH:", "DTF:", "IT:", "YSSG+COTRES:", "RIDG:", "OCUG:"]
        >>> lines_lecs = ["GOSG", "YDTG", "DTH", "DTF", "IT", "YSSG+COTRES", "RIDG", "OCUG"]
        >>> tbl_db["TABLE_LECS"] = {"nb_columns": 2, "table_title": "New Table", "lecs_or_vars": lines_lecs, 
        ...                         "lines_titles": lines_titles, "mode": True, "files": True, "date": True}  
        >>> tbl_db["TABLE_LECS"]         # doctest: +NORMALIZE_WHITESPACE
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
        language: 'English'
        gridx: 'major'
        gridy: 'major'
        graph_axis: 'values'
        graph_alignment: 'left'
        <BLANKLINE>

        >>> # 2. specify list of variables
        >>> vars_list = ["GOSG", "YDTG", "DTH", "DTF", "IT", "YSSG", "COTRES", "RIDG", "OCUG", "$ENVI"]
        >>> tbl_db["TABLE_VARS"] = {"nb_columns": 2, "table_title": "New Table", "lecs_or_vars": vars_list, 
        ...                         "mode": True, "files": True, "date": True}  
        >>> tbl_db["TABLE_VARS"]             # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1                                                                    |
        TITLE |                                  "New Table"
        ----- | -----------------------------------------------------------------------------
        CELL  | ""                                                                   |  "#S"
        ----- | -----------------------------------------------------------------------------
        CELL  | "Bruto exploitatie-overschot: overheid (= afschrijvingen)."          |   GOSG
        CELL  | "Overheid: geïnde indirecte belastingen."                            |   YDTG
        CELL  | "Totale overheid: directe belasting van de gezinnen."                |    DTH
        CELL  | "Totale overheid: directe vennootschapsbelasting."                   |    DTF
        CELL  | "Totale indirecte belastingen."                                      |     IT
        CELL  | "Globale overheid: ontvangen sociale zekerheidsbijdragen."           |   YSSG
        CELL  | "Cotisation de responsabilisation."                                  | COTRES
        CELL  | "Overheid: inkomen uit vermogen."                                    |   RIDG
        CELL  | "Globale overheid: saldo van de ontvangen lopendeoverdrachten."      |   OCUG
        CELL  | "Index wereldprijs - invoer van niet-energieprodukten, inUSD."       |  PWMAB
        CELL  | "Index wereldprijs - invoer van diensten, in USD."                   |   PWMS
        CELL  | "Index wereldprijs - uitvoer van niet-energieprodukten, inUSD."      |  PWXAB
        CELL  | "Index wereldprijs - uitvoer van diensten, in USD."                  |   PWXS
        CELL  | "Indicator van het volume van de wereldvraag naar goederen,1985=1."  |  QWXAB
        CELL  | "Indicator van het volume van de wereldvraag naar diensten,1985=1."  |   QWXS
        CELL  | "Brent olieprijs (USD per barrel)."                                  |   POIL
        CELL  | "Totale beroepsbevolking (jaargemiddelde)."                          |   NATY
        ----- | -----------------------------------------------------------------------------
        MODE  |
        FILES |
        DATE  |
        <BLANKLINE>
        nb lines: 25
        nb columns: 2
        language: 'English'
        gridx: 'major'
        gridy: 'major'
        graph_axis: 'values'
        graph_alignment: 'left'
        <BLANKLINE>

        >>> # -- update table --
        >>> table = tbl_db["TABLE_LECS"]
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
        language: 'English'
        gridx: 'major'
        gridy: 'major'
        graph_axis: 'values'
        graph_alignment: 'left'
        <BLANKLINE>

        >>> # set graph axis type
        >>> table.graph_axis = 'semilog'
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
        'line'
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
        >>> table.insert(index, '-')
        >>> table.insert(index + 1, "New Title")
        >>> table.insert(index + 2, '-')
        >>> # append a new sepatator line
        >>> table += '-'

        >>> # warning: do not forget to actually update the IODE Table database  
        >>> tbl_db["TABLE_LECS"] = table
        >>> tbl_db["TABLE_LECS"]                # doctest: +NORMALIZE_WHITESPACE
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
        language: 'English'
        gridx: 'major'
        gridy: 'major'
        graph_axis: 'semilog'
        graph_alignment: 'left'
        <BLANKLINE>

        Variables

        >>> from iode import Variables
        >>> var_db = Variables(f"{SAMPLE_DATA_DIR}/fun.var")

        >>> # set all values of a Variable
        >>> var_db["ACAF"]                   # doctest: +ELLIPSIS 
        [-2e+37, -2e+37, ..., -83.34062511080091, -96.41041982848331]
        >>> # a. variable = same value for all periods
        >>> var_db["ACAF"] = 0.
        >>> var_db["ACAF"]                   # doctest: +ELLIPSIS 
        [0.0, 0.0, 0.0, ..., 0.0, 0.0, 0.0]
        >>> # b. variable = vector (list) containing a specific value for each period
        >>> var_db["ACAF"] = list(range(var_db.nb_periods))
        >>> var_db["ACAF"]                   # doctest: +ELLIPSIS 
        [0.0, 1.0, 2.0, ..., 53.0, 54.0, 55.0]
        >>> # c. variable = LEC expression
        >>> var_db["ACAF"] = "t + 10"
        >>> var_db["ACAF"]                   # doctest: +ELLIPSIS 
        [10.0, 11.0, 12.0, ..., 63.0, 64.0, 65.0]

        >>> # set one value of a Variable for a specific period
        >>> var_db["ACAG", "1990Y1"]
        -28.1721855713507
        >>> var_db["ACAG", "1990Y1"] = -28.2
        >>> var_db["ACAG", "1990Y1"]
        -28.2

        >>> # set the variable values for range of periods 
        >>> # 1. using a Python slice
        >>> # 1a. variable(periods) = same value for all periods
        >>> var_db["ACAF", "1991Y1":"1995Y1"] = 0.0
        >>> var_db["ACAF", "1991Y1":"1995Y1"]
        [0.0, 0.0, 0.0, 0.0, 0.0]
        >>> # 1b. variable(periods) = vector (list) containing a specific value for each period
        >>> var_db["ACAF", "1991Y1":"1995Y1"] = [0., 1., 2., 3., 4.]
        >>> var_db["ACAF", "1991Y1":"1995Y1"]
        [0.0, 1.0, 2.0, 3.0, 4.0]
        >>> # 1c. variable(periods) = LEC expression
        >>> var_db["ACAF", "1991Y1":"1995Y1"] = "t + 10"
        >>> var_db["ACAF", "1991Y1":"1995Y1"]
        [41.0, 42.0, 43.0, 44.0, 45.0]

        >>> # 2. same as above but with the colon ':' inside the periods range string
        >>> # 2a. variable(periods) = same value for all periods
        >>> var_db["ACAF", "1991Y1:1995Y1"] = 0.0
        >>> var_db["ACAF", "1991Y1:1995Y1"]
        [0.0, 0.0, 0.0, 0.0, 0.0]
        >>> # 2b. variable(periods) = vector (list) containing a specific value for each period
        >>> var_db["ACAF", "1991Y1:1995Y1"] = [0., -1., -2., -3., -4.]
        >>> var_db["ACAF", "1991Y1":"1995Y1"]
        [0.0, -1.0, -2.0, -3.0, -4.0]
        >>> # 2c. variable(periods) = LEC expression
        >>> var_db["ACAF", "1991Y1:1995Y1"] = "t - 10"
        >>> var_db["ACAF", "1991Y1:1995Y1"]
        [21.0, 22.0, 23.0, 24.0, 25.0]
        """
        self._set_object(key, value) 

    def __delitem__(self, key):
        """
        Remove the IODE object named `key` from the current database.

        Parameters
        ----------
        key: str
            name of the IODE object to remove.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Comments
        >>> cmt_db = Comments(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> "ACAF" in cmt_db
        True
        >>> del cmt_db["ACAF"]
        >>> "ACAF" in cmt_db
        False
        """
        if not isinstance(key, str):
            raise TypeError(f"Cannot delete object {key}.\nExpected a string value for {key} " + 
                f"but got value of type {type(key).__name__}")
        self.abstract_db_ptr.remove(key.encode())
