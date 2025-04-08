import sys
from pathlib import Path
from typing import Union, Tuple, List, Optional, Any

if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

import pandas as pd
from iode.util import join_lines, table2str, JUSTIFY
from iode.iode_database.abstract_database import IodeDatabase, PositionalIndexer
from iode.iode_cython import  ImportFormats, TableLang
from iode.iode_cython import Comments as CythonComments


class Comments(IodeDatabase):
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
    def __init__(self, filepath: str=None):
        raise TypeError("This class cannot be instantiated directly.")

    @classmethod
    def get_instance(cls) -> Self:
        instance = cls.__new__(cls)
        instance._cython_instance = CythonComments()
        return instance

    def _load(self, filepath: str):
        self._cython_instance._load(filepath)

    def _subset(self, pattern: str, copy: bool) -> Self:
        instance = Comments.get_instance()
        instance._cython_instance = self._cython_instance.initialize_subset(instance._cython_instance, pattern, copy)
        return instance

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

    def _get_object(self, key: Union[str, int]) -> str:
        name = self._single_object_key_to_name(key)
        if not name in self:
            raise KeyError(f"Name '{name}' not found in the {type(self).__name__} workspace")
        comment = self._cython_instance._get_object(name)
        return comment

    def _set_object(self, key: Union[str, int], value: str):
        name = self._single_object_key_to_name(key)
        self._cython_instance._set_object(name, value)

    def __getitem__(self, key: Union[str, List[str]]) -> Union[str, Self]:
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
        value: str or dict(str, str) or pandas.Series(str, str) or Comments
            (new) comment(s) value(s).

        Examples
        --------
        >>> import pandas as pd
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

        >>> # c) add/update several comments at once
        >>> # 1) using a dict of values
        >>> values = {"AOUC": "Updated AOUC from dict", "ACAF": "Updated ACAF from dict", 
        ...           "ACAG": "Updated ACAG from dict"}
        >>> comments["ACAF, ACAG, AOUC"] = values
        >>> comments["ACAF, ACAG, AOUC"]                # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Comments
        nb comments: 3
        filename: ...fun.cmt
        <BLANKLINE>
        name	       comments       
        ACAF	Updated ACAF from dict
        ACAG	Updated ACAG from dict
        AOUC	Updated AOUC from dict

        >>> # 2) using a pandas series
        >>> data = ["Updated AOUC from series", "Updated ACAF from series", "Updated ACAG from series"]
        >>> series = pd.Series(data, index=["AOUC", "ACAF", "ACAG"])
        >>> comments["ACAF, ACAG, AOUC"] = series
        >>> comments["ACAF, ACAG, AOUC"]               # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Comments
        nb comments: 3
        filename: ...fun.cmt
        <BLANKLINE>
        name	        comments        
        ACAF	Updated ACAF from series
        ACAG	Updated ACAG from series
        AOUC	Updated AOUC from series

        >>> # 3) using an iode Comments object (subset)
        >>> comments_subset = comments["ACAF, ACAG, AOUC"].copy()
        >>> comments_subset["ACAF"] = "Updated ACAF from another iode Comments database"
        >>> comments_subset["ACAG"] = "Updated ACAG from another iode Comments database"
        >>> comments_subset["AOUC"] = "Updated AOUC from another iode Comments database"
        >>> comments["ACAF, ACAG, AOUC"] = comments_subset
        >>> comments["ACAF, ACAG, AOUC"]                # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Comments
        nb comments: 3
        filename: ...fun.cmt
        <BLANKLINE>
        name	                    comments                    
        ACAF	Updated ACAF from another iode Comments database
        ACAG	Updated ACAG from another iode Comments database
        AOUC	Updated AOUC from another iode Comments database

        >>> # d) working on a subset
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
        r"""
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
        r"""
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
        Loading ...\fun.cmt
        317 objects loaded
        >>> comments.get_names("A*")
        ['ACAF', 'ACAG', 'AOUC', 'AQC']

        >>> comments.clear()
        >>> # load all comments
        >>> comments.copy_from(f"{SAMPLE_DATA_DIR}/fun.cmt")            # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading ...\fun.cmt
        317 objects loaded
        >>> len(comments)
        317
        """
        input_files, names = self._copy_from(input_files, names)
        self._cython_instance.copy_from(input_files, names)

    def from_series(self, s: pd.Series):
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
        if not (self.is_global_workspace or self.is_detached):
            # check that all names in the pandas object are present in the current subset 
            self._check_same_names(self.names, s.index.tolist())

        for index, value in s.items():
            self._set_object(index, value)

    def to_series(self) -> pd.Series:
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
        names = self.names
        data = [self._get_object(name) for name in names]
        return pd.Series(data=data, index=names, dtype=str, name=self.__class__.__name__)

    @property
    def series(self) -> pd.Series:
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

    @classmethod
    def convert_file(cls, input_file: Union[str, Path], input_format: Union[str, ImportFormats], save_file: Union[str, Path], rule_file: Union[str, Path], lang: Union[str, TableLang]=TableLang.ENGLISH, debug_file: Union[str, Path]=None):
        r"""
        Convert an external file representing IODE comments to an IODE comments file (.cmt). 
        The possible formats for the input file are:
          
          - `Ascii`: IODE-specific Ascii format for objects 
          - `Rotated Ascii`: Ascii format for variables with series in columns 
          - `DIF`: DIF format (Data Interchange Format) 
          - `DIF` Belgostat: (old) exchange format specific to Belgostat 
          - `NIS`: National Institute of Statistics Ascii format (old) 
          - `GEM`: Ascii format of Chronos software 
          - `PRN-Aremos`: Ascii format from Aremos software 
          - `TXT Belgostat`: (old) Belgostat-specific exchange format 

        The rule file is a simple text file contains the rules for: 
        
          - selecting the objects to be imported 
          - determining the objects names.

        Each rule consists of two fields:

          - the selection pattern, containing a description of the names concerned by the rule. 
            This mask is defined in the same way as the :py:meth:`~iode.Comments.search` method.
          - the transcoding algorithm for the names, which can contain : 
            - `+` : indicates that the character must be included in the name 
            - `-` : indicates that the character should be skipped 
            - any other character: included in the name 
        
        Example:

            B* C+-+          -> transforms B1234 into CB2, BCDEF into CBE, etc 
            *X ++++++++++    -> keeps names ending in X unchanged
            * ++++++++++     -> keeps all names unchanged

        Parameters
        ----------
        input_file : str or Path
            The path to the input file to be converted. 
        input_format : str or ImportFormats
            The format of the input file. Possible formats are ASCII, ROT_ASCII (Rotated Ascii), 
            DIF, BISTEL, NIS, GEM, PRN, TXT (TXT Belgostat).
        save_file : str or Path
            The path to the output file where the IODE comments will be saved.
        rule_file : str or Path
            The path to the rule file that defines the selection and transcoding rules. 
        lang : str or TableLang, optional
            The language of the extracted comments. 
            It is only used when a text appears in several languages in the input file. 
            Currently, only the Belgostat DIF format uses this value, allowing you to select 
            the language of the extracted comments.
            Default is ENGLISH.
        debug_file : str or Path, optional
            The path to the debug file where the debug information will be saved. 
            If not provided, the debug information will be printed to the console.

        Examples
        --------
        >>> from pathlib import Path
        >>> from iode import SAMPLE_DATA_DIR, comments, ImportFormats
        >>> output_dir = getfixture('tmp_path')

        >>> input_file = f"{SAMPLE_DATA_DIR}/fun_xode.ac.ref"
        >>> input_format = ImportFormats.ASCII
        >>> save_file = str(output_dir / "imported_cmt.cmt")
        >>> rule_file = f"{SAMPLE_DATA_DIR}/rules.txt"
        >>> debug_file = str(output_dir / "debug.log")

        >>> # print rules
        >>> with open(rule_file, "r") as f:         # doctest: +NORMALIZE_WHITESPACE
        ...     print(f.read())
        ...
        AC*  KK_--+++++++++++++
        *U   UU_++++++++++++++++
        >>> # get list of comments with a name starting with 'AC' 
        >>> # and ending with 'U' from the input file
        >>> with open(input_file, "r") as f:         # doctest: +NORMALIZE_WHITESPACE
        ...     for line in f:
        ...         name = line.split(" ")[0]
        ...         if name.startswith("AC") or name.endswith("U"):
        ...             print(line.strip())
        ...
        ACAF "Ondernemingen: ontvangen kapitaaloverdrachten."
        ACAG "Totale overheid: netto ontvangen kapitaaloverdrachten."
        DPU "Nominale afschrijvingen op de kapitaalvoorraad."
        DPUU "Nominale afschrijvingen op de kapitaalvoorraad (aangepast: inkomensoptiek)."
        IFU "Bruto kapitaalvorming: ondernemingen."
        IHU "Bruto kapitaalvorming: gezinnen."
        WBU "Totale loonmassa (inclusief werkgeversbijdragen)."
        >>> # import comments from input_file to save_file
        >>> # using the rules defined in rule_file
        >>> comments.convert_file(input_file, input_format, save_file, rule_file, 'E', debug_file)
        Reading object 1 : KK_AF
        Reading object 2 : KK_AG
        Reading object 3 : UU_DPU
        Reading object 4 : UU_DPUU
        Reading object 5 : UU_IFU
        Reading object 6 : UU_IHU
        Reading object 7 : UU_WBU
        7 objects saved
        >>> # check content of the saved file
        >>> comments.load(save_file)        # doctest: +ELLIPSIS
        Loading ...\imported_cmt.cmt
        7 objects loaded
        >>> comments                        # doctest: +NORMALIZE_WHITESPACE, +ELLIPSIS
        Workspace: Comments
        nb comments: 7
        filename: ...\imported_cmt.cmt
        <BLANKLINE>
          name                                        comments
        KK_AF       Ondernemingen: ontvangen kapitaaloverdrachten.
        KK_AG       Totale overheid: netto ontvangen kapitaaloverdrachten.
        UU_DPU      Nominale afschrijvingen op de kapitaalvoorraad.
        UU_DPUU     Nominale afschrijvingen op de kapitaalvoorraad (aangepast: inkomensoptiek).
        UU_IFU      Bruto kapitaalvorming: ondernemingen.
        UU_IHU      Bruto kapitaalvorming: gezinnen.
        UU_WBU      Totale loonmassa (inclusief werkgeversbijdragen).
        <BLANKLINE>
        >>> # content of the debug file
        >>> with open(debug_file, "r") as f:         # doctest: +NORMALIZE_WHITESPACE
        ...     for line in f:
        ...         print(line.strip())
        ...
        ACAF -> KK_AF       (Rule KK_--+++++++++++++)
        ACAG -> KK_AG       (Rule KK_--+++++++++++++)
        DPU -> UU_DPU       (Rule UU_++++++++++++++++)
        DPUU -> UU_DPUU     (Rule UU_++++++++++++++++)
        IFU -> UU_IFU       (Rule UU_++++++++++++++++)
        IHU -> UU_IHU       (Rule UU_++++++++++++++++)
        WBU -> UU_WBU       (Rule UU_++++++++++++++++)
        """
        return CythonComments.convert_file(input_file, input_format, save_file, rule_file, lang, debug_file)

    def _str_table(self, names: List[str]) -> str:
        columns = {"name": names, "comments": [join_lines(self._get_object(name)) for name in names]}
        return table2str(columns, max_lines=10, justify_funcs={"name": JUSTIFY.LEFT, "comments": JUSTIFY.LEFT})

    def print_to_file(self, filepath: Union[str, Path], names: Union[str, List[str]]=None, format: str=None):
        r"""
        Print the list comments defined by `names` to the file `filepath` using the format `format`.

        Argument `format` must be in the list:
        - 'H' (HTML file)
        - 'M' (MIF file)
        - 'R' (RTF file)
        - 'C' (CSV file)

        If argument `format` is null (default), the *A2M* format will be used to print the output.

        If the filename does not contain an extension, it is automatically added based on 
        the value of `format`.

        If `names` is a string, it is considered as a *pattern* and the function will print 
        all comments matching the pattern. The following characters in *pattern* have a 
        special meaning:
        
            - `*` : any character sequence, even empty
            - `?` : any character (one and only one)
            - `@` : any alphanumerical char [A-Za-z0-9]
            - `&` : any non alphanumerical char
            - `|` : any alphanumeric character or none at the beginning and end of a string 
            - `!` : any non-alphanumeric character or none at the beginning and end of a string 
            - `\` : escape the next character

        If `names` is None, print all comments of the (subset of the) current database.

        Parameters
        ----------
        filepath: str or Path
            path to the file to print.
            If the filename does not contain an extension, it is automatically 
            added based on the value of the format argument.
        names: str or list of str, optional
            pattern or list of names of the comme,ts to print.
            If None, print all comments of the (subset of the) current database.
            Defaults to None.
        format: str, optional
            format of the output file. Possible values are: 'H' (HTML file), 
            'M' (MIF file), 'R' (RTF file) or 'C' (CSV file).
            Defaults to None meaning that the comments will be dumped in the *A2M* format.

        Examples
        --------
        >>> from iode import comments, SAMPLE_DATA_DIR
        >>> output_dir = getfixture('tmp_path')
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")             # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded
        >>> comments.print_to_file(output_dir / "comments.csv", ["ACAF", "ACAG"])     # doctest: +ELLIPSIS
        Printing IODE objects definition to file '...comments.csv'...
        Printing ACAF ...
        Printing ACAG ...
        Print done
        >>> with open(output_dir / "comments.csv") as f:                     # doctest: +NORMALIZE_WHITESPACE
        ...     print(f.read())
        ...
        " - ACAF : Ondernemingen : ontvangen kapitaaloverdrachten."
        " - ACAG : Totale overheid : netto ontvangen kapitaaloverdrachten."
        <BLANKLINE>
        """
        super().print_to_file(filepath, names, format)

    def __hash__(self) -> int:
        r"""
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
        return super().__hash__()


comments: Comments = Comments.get_instance()
