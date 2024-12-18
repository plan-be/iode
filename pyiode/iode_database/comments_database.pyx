# distutils: language = c++

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


@cython.final
cdef class Comments(_AbstractDatabase):
    r"""
    IODE Comments database. 

    Attributes
    ----------
    filename: str
    description: str

    Parameters
    ----------
    filepath: str, optional
        file containing the IODE comments to load.

    Returns
    -------
    Comments

    Examples
    --------
    >>> from iode import comments, SAMPLE_DATA_DIR
    >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Loading .../fun.cmt
    317 objects loaded 
    >>> len(comments)
    317
    >>> comments                                        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Workspace: Comments
    nb comments: 317
    filename: ...\tests\data\fun.cmt
    <BLANKLINE>
     name                                                  comments
    ACAF        Ondernemingen: ontvangen kapitaaloverdrachten.
    ACAG        Totale overheid: netto ontvangen kapitaaloverdrachten.
    AOUC        Kost per eenheid produkt.
    AQC         Kost per eenheid produkt: kapitaal en arbeid.
    BENEF       Ondernemingen: niet-uitgekeerde winsten.
    ...         ...
    ZF          Indexeringscoëfficiënt voor de lonen in de private sector.
    ZJ          Indexeringscoëfficiënt voor de sociale uitkeringen, vertraagd in de periode 1984-1988, voor de
                verrekening van de indexsprongen bij de vervangingsinkomens (1984, 1985, 1987).
    ZKF         Bezettingsgraad van de produktiecapaciteit.
    ZX          Saut d'index (correction en %)
    ZZ_         Marktsector (ondernemingen en zelfstandigen): loonquote (gemiddelde 1954-94).
    <BLANKLINE>
    """
    cdef bint ptr_owner
    cdef CKDBComments* database_ptr

    def __cinit__(self, filepath: str = None) -> Comments:
        self.database_ptr = NULL
        self.abstract_db_ptr = NULL
        self.ptr_owner = False

    def __init__(self, filepath: str = None):
        # Prevent accidental instantiation from normal Python code
        # since we cannot pass a struct pointer into a Python constructor.
        raise TypeError("This class cannot be instantiated directly.")

    def __dealloc__(self):
        # if self.database_ptr points to the C++ global instance Comments 
        # which does not need to be manually deleted 
        if self.ptr_owner and self.database_ptr is not NULL:
            del self.database_ptr
            self.database_ptr = NULL

    # see https://cython.readthedocs.io/en/stable/src/userguide/extension_types.html#instantiation-from-existing-c-c-pointers 
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

    # TODO: implement KDBAbstract::load() method (for global KDB only)
    def _load(self, filepath: str):
        cdef CKDBComments* kdb = new CKDBComments(filepath.encode())
        del kdb

    def _subset(self, pattern: str, copy: bool) -> Comments:
        # call to __new__() that bypasses the __init__() constructor.
        cdef Comments subset_ = Comments.__new__(Comments)
        subset_.database_ptr = subset_.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        return subset_

    @property
    def i(self) -> PositionalIndexer:
        r"""
        Allow to select the ith comment in the database.

        Examples
        --------
        >>> from iode import comments, SAMPLE_DATA_DIR
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> # get the first comment
        >>> comments.i[0]
        'Ondernemingen: ontvangen kapitaaloverdrachten.'
        >>> # get the last comment
        >>> comments.i[-1]
        'Marktsector (ondernemingen en zelfstandigen): loonquote\n(gemiddelde 1954-94).'
        >>> # update first comment
        >>> comments.i[0] = 'New Comment'
        >>> comments.i[0]
        'New Comment'
        >>> # update last comment
        >>> comments.i[-1] = 'New Comment'
        >>> comments.i[-1]
        'New Comment'
        """
        return PositionalIndexer(self)

    def _get_object(self, key: Union[str, int]):
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

    def __getitem__(self, key: Union[str, List[str]]) -> Union[str, Comments]:
        r"""
        Return the (subset of) comment(s) referenced by `key`.

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
            (the list of) name(s) of the comment(s) to get.
            The list of comments to get can be specified by a pattern or by a list of sub-patterns (e.g. "A*;*_").

        Returns
        -------
        Single comment or a subset of the database.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 

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
        """
        return super().__getitem__(key)

    def __setitem__(self, key: Union[str, List[str]], value: Union[str, List[str]]):
        r"""
        Update/add a (subset of) comment(s) referenced by `key` from/to the Comments database.

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
            (the list of) name(s) of the comment(s) to update/add.
            The list of comments to update/add can be specified by a pattern or by a list of sub-patterns 
            (e.g. "A*;*_").
        value: str or list(str)
            (new) comment value(s).

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        
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
        """
        super().__setitem__(key, value)

    def __delitem__(self, key):
        """
        Remove the (subset of) comment(s) referenced by `key` from the Comments database.

        Parameters
        ----------
        key: str or list(str)
            (list of) name(s) of the comment(s) to be removed.
            The list of names can be given as a string pattern (e.g. "A*;*_").

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 

        >>> # a) delete one comment
        >>> comments.get_names("A*")
        ['ACAF', 'ACAG', 'AOUC', 'AQC']
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
        >>> # NOTE: the comment has also been deleted from the global database
        >>> "DPUGO" in comments
        False
        >>> comments.get_names("D*")
        ['DPU', 'DPUF', 'DPUG', 'DPUH', 'DPUU', 'DTF', 'DTFX', 'DTH', 'DTH1', 'DTH1C', 'DTHX']
        """
        super().__delitem__(key)

    def copy_from(self, input_files: Union[str, List[str]], names: Union[str, List[str]]='*'):
        """
        Copy (a subset of) comments from the input file(s) 'input_files' into the current database.

        Parameters
        ----------
        input_file: str or list(str)
            file(s) from which the copied comments are read.
        names: str or list(str)
            list of comments to copy from the input file(s).
            Defaults to load all comments from the input file(s). 
        
        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> len(comments)
        317

        >>> # delete all comments with a name starting with 'A'
        >>> comments.remove("A*")
        >>> comments.get_names("A*")
        []

        >>> # load all comments with a name starting with 'A'
        >>> comments.copy_from(f"{SAMPLE_DATA_DIR}/fun.cmt", "A*")      # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading ...\\fun.cmt
        317 objects loaded
        >>> comments.get_names("A*")
        ['ACAF', 'ACAG', 'AOUC', 'AQC']

        >>> comments.clear()
        >>> # load all comments
        >>> comments.copy_from(f"{SAMPLE_DATA_DIR}/fun.cmt")            # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading ...\\fun.cmt
        317 objects loaded
        >>> len(comments)
        317
        """
        input_files, names = self._copy_from(input_files, names)
        self.database_ptr.copy_from(input_files.encode(), names.encode())

    def from_series(self, s: Series):
        r"""
        Copy the pandas Series `s` into the IODE Comments database.
        The comment names to copy are deduced from the index of the Series.

        Parameters
        ----------
        s: Series
            pandas Series containing the comments to copy into the IODE Comments database.

        Notes
        -----
        The index of the passed Series is sorted in alphabetical order before 
        copying to IODE Comments database.

        See Also
        --------
        Comments.to_series

        Examples
        --------
        >>> from iode import comments
        >>> import pandas as pd
        >>> comments.clear()
        >>> len(comments)
        0

        >>> # create the pandas Series
        >>> names = ["A0", "A1", "B0", "B1", "C0", "C1"]
        >>> data = ["A zero", "A one", "B zero", "B one", "C zero", "C one"]
        >>> s = pd.Series(data=data, index=names, dtype=str, name="Comments")
        >>> # display the pandas series
        >>> s          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        A0     A zero
        A1     A one
        B0    B zero
        B1     B one
        C0    C zero
        C1     C one
        Name: Comments, dtype: object

        >>> # load into the IODE Comments database
        >>> comments.from_series(s)
        >>> len(comments)
        6
        
        >>> comments.names             # doctest: +ELLIPSIS
        ['A0', 'A1', 'B0', 'B1', 'C0', 'C1']
        >>> comments["B0"]
        'B zero'
        >>> comments["C1"]
        'C one'
        """
        if pd is None:
            raise RuntimeError("pandas library not found")

        for index, value in s.items():
            self._set_object(index, value)

    def to_series(self) -> Series:
        r"""
        Create a pandas Series from the current Comments database.
        The index of the returned Series is build from the Comments names.

        See Also
        --------
        Comments.from_series

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, comments
        >>> import pandas as pd
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> len(comments)
        317

        >>> # Export the IODE Comments database as a pandas Series
        >>> s = comments.to_series()
        >>> len(s)
        317

        >>> s.index.to_list()               # doctest: +ELLIPSIS
        ['ACAF', 'ACAG', 'AOUC', 'AQC', ..., 'ZJ', 'ZKF', 'ZX', 'ZZ_']
        >>> comments["ACAF"]                # doctest: +NORMALIZE_WHITESPACE
        'Ondernemingen: ontvangen kapitaaloverdrachten.' 
        >>> s["ACAF"]                       # doctest: +NORMALIZE_WHITESPACE
        'Ondernemingen: ontvangen kapitaaloverdrachten.' 
        >>> comments["ZZ_"]                 # doctest: +NORMALIZE_WHITESPACE
        'Marktsector (ondernemingen en zelfstandigen): loonquote\n(gemiddelde 1954-94).'
        >>> s["ZZ_"]                        # doctest: +NORMALIZE_WHITESPACE
        'Marktsector (ondernemingen en zelfstandigen): loonquote\n(gemiddelde 1954-94).'

        >>> # Export a subset of the IODE Comments database as a pandas Series
        >>> s = comments["A*;*_"].to_series()
        >>> len(s)
        34

        >>> s.index.to_list()               # doctest: +ELLIPSIS
        ['ACAF', 'ACAG', 'AOUC', 'AQC', ..., 'WIND_', 'WNF_', 'YDH_', 'ZZ_']
        >>> comments["ACAF"]                # doctest: +NORMALIZE_WHITESPACE
        'Ondernemingen: ontvangen kapitaaloverdrachten.' 
        >>> s["ACAF"]                       # doctest: +NORMALIZE_WHITESPACE
        'Ondernemingen: ontvangen kapitaaloverdrachten.' 
        >>> comments["ZZ_"]                 # doctest: +NORMALIZE_WHITESPACE
        'Marktsector (ondernemingen en zelfstandigen): loonquote\n(gemiddelde 1954-94).'
        >>> s["ZZ_"]                        # doctest: +NORMALIZE_WHITESPACE
        'Marktsector (ondernemingen en zelfstandigen): loonquote\n(gemiddelde 1954-94).'
        """
        if pd is None:
            raise RuntimeError("pandas library not found")
        
        names = self.names
        data = [self._get_object(name) for name in names]
        return pd.Series(data=data, index=names, dtype=str, name=self.__class__.__name__)

    @property
    def series(self) -> Series:
        r"""
        Create a pandas Series from the current Comments database.
        The index of the returned Series is build from the Comments names.

        See Also
        --------
        Comments.to_series

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, comments
        >>> import pandas as pd
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> len(comments)
        317

        >>> # Export the IODE Comments database as a pandas Series
        >>> s = comments.series
        >>> len(s)
        317

        >>> s.index.to_list()               # doctest: +ELLIPSIS
        ['ACAF', 'ACAG', 'AOUC', 'AQC', ..., 'ZJ', 'ZKF', 'ZX', 'ZZ_']
        >>> comments["ACAF"]                # doctest: +NORMALIZE_WHITESPACE
        'Ondernemingen: ontvangen kapitaaloverdrachten.' 
        >>> s["ACAF"]                       # doctest: +NORMALIZE_WHITESPACE
        'Ondernemingen: ontvangen kapitaaloverdrachten.' 
        >>> comments["ZZ_"]                 # doctest: +NORMALIZE_WHITESPACE
        'Marktsector (ondernemingen en zelfstandigen): loonquote\n(gemiddelde 1954-94).'
        >>> s["ZZ_"]                        # doctest: +NORMALIZE_WHITESPACE
        'Marktsector (ondernemingen en zelfstandigen): loonquote\n(gemiddelde 1954-94).'

        >>> # Export a subset of the IODE Comments database as a pandas Series
        >>> s = comments["A*;*_"].series
        >>> len(s)
        34

        >>> s.index.to_list()               # doctest: +ELLIPSIS
        ['ACAF', 'ACAG', 'AOUC', 'AQC', ..., 'WIND_', 'WNF_', 'YDH_', 'ZZ_']
        >>> comments["ACAF"]                # doctest: +NORMALIZE_WHITESPACE
        'Ondernemingen: ontvangen kapitaaloverdrachten.' 
        >>> s["ACAF"]                       # doctest: +NORMALIZE_WHITESPACE
        'Ondernemingen: ontvangen kapitaaloverdrachten.' 
        >>> comments["ZZ_"]                 # doctest: +NORMALIZE_WHITESPACE
        'Marktsector (ondernemingen en zelfstandigen): loonquote\n(gemiddelde 1954-94).'
        >>> s["ZZ_"]                        # doctest: +NORMALIZE_WHITESPACE
        'Marktsector (ondernemingen en zelfstandigen): loonquote\n(gemiddelde 1954-94).'
        """
        return self.to_series()

    def _str_table(self, names: List[str]) -> str:
        columns = {"name": names, "comments": [join_lines(self._get_object(name)) for name in names]}
        return table2str(columns, max_lines=10, justify_funcs={"name": JUSTIFY.LEFT, "comments": JUSTIFY.LEFT})

    def __hash__(self) -> int:
        """
        Return a hash value for the current Comments database.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, comments
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> len(comments)
        317
        >>> original_hash = hash(comments)
        
        >>> # rename 1 comment
        >>> comments.rename("ACAF", "ACAF_")
        >>> original_hash == hash(comments)
        False
        >>> comments.rename("ACAF_", "ACAF")  # revert the change
        >>> original_hash == hash(comments)
        True

        >>> # modify one comment
        >>> original_comment = comments["ACAF"]
        >>> comments["ACAF"] = "modified comment"
        >>> original_hash == hash(comments)
        False
        >>> comments["ACAF"] = original_comment  # revert the change
        >>> original_hash == hash(comments)
        True

        >>> # delete a comment
        >>> original_comment = comments["ACAF"]
        >>> del comments["ACAF"]
        >>> original_hash == hash(comments)
        False
        >>> comments["ACAF"] = original_comment
        >>> original_hash == hash(comments)
        True

        >>> # add a comment
        >>> comments["NEW"] = "new comment"
        >>> original_hash == hash(comments)
        False
        >>> del comments["NEW"]
        >>> original_hash == hash(comments)
        True
        """
        return hash_value(dereference(self.database_ptr))

comments: Comments = Comments._from_ptr()
