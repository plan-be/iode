# distutils: language = c++

from collections.abc import Iterable
from typing import Union, Tuple, List, Optional, Any
if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

from iode.util import split_list

cimport cython
from pyiode.iode_database.cpp_api_database cimport KDBLists as CKDBLists
from pyiode.iode_database.cpp_api_database cimport Lists as cpp_global_lists


@cython.final
cdef class Lists(_AbstractDatabase):
    r"""
    IODE Lists database. 

    Attributes
    ----------
    filename: str
    description: str

    Parameters
    ----------
    filepath: str, optional
        file containing the IODE lists to load.

    Returns
    -------
    Lists

    Examples
    --------
    >>> from iode import lists, SAMPLE_DATA_DIR
    >>> lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")
    >>> len(lists)
    17
    >>> lists           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Workspace: Lists
    nb lists: 17
    filename: ...\tests\data\fun.lst
    description: Modèle fun
    <BLANKLINE>
       name                                                    lists
    COPY        $COPY0; $COPY1;
    COPY0       ACAF; ACAG; AOUC; AQC; BENEF; BQY; BVY; CGU; COEFON; COTRES; DPU; DPUF; DPUG; DPUGO; DPUH; DPUU;
                DTF; DTH; DTH1; DTH1C; EXC; EXCC; FLF; FLG; GAP; GOSF; GOSG; GOSH; GOSH_; IDF; IDG; IDH; IFU; IHU;
                IT; ITCEE; ITCR; ITD; ITEP; ITF; ITF5; ITFC; ITFGI; ITFGO; ITFQ; ITGR; ITI5R; ITIFR; ITIGR; ITM;
                ITMQ; ITMQR; ITNQ; ITON; ITONQ; ITPL; ITPR; ITPS; ITT; IUG; KN5; KNF; KNFF; KNFFY; KNFY; KNI; KNIY;
                NFY; NFYH; OCUF; OCUG; OCUH; PAF_; PAG; PAH; PBBP; PBNP; PC; PC_; PDPUG; PFI; PFI_; PFND; PG; PI5;
                PIF; PIG; PM; PMAB; PME; PMS; PMT; PQOG; PW3; PWBG; PX; PXAB; PXB; PXE; PXS; PXT; QAF; QAFF; QAFF_;
                QAF_; QAG; QAH; QAI_; QAT; QAT_; QBBP; QBBPPOT_; QBBP_B; QBBP_P; QBNP; QC; QC_; QFND; QG; QGO; QI;
                QI5; QIF; QIG; QM; QMAB; QME; QMS; QMT; QQMAB_; QS; QX; QXAB; QXB; QXE; QXS; QXT; Q_F; Q_I; RIDG;
                RIDGG; RIPBE; RLBE; RLBERHP; RSBE; SBF; SBF3L; SBG; SBH; SF; SG; SH; SSF; SSF3; SSF3L; SSF3P;
                SSFDOM; SSFF; SSFFIC; SSFG; SSH; SSH3GP; SSH3O; SSH3P; SSH3W; SSH3WA; SSH3WW; SSH3ZA; SSH3ZW; SSHFF;
                SUBCEE; TWG; TWGP; ULCP; UY; VAF; VAFF; VAFF_; VAG; VAH; VAI; VAI_; VAMARE; VAT; VAT_; VBBP; VBBP_B;
                VBBP_P; VBNP; VBNP_B; VBNP_I; VBNP_P; VC; VC_; VI; VI5; VIF; VM
    COPY1       VMAB; VME; VMK; VMN; VMS; VMT; VS; VS_; VX; VXAB; VXB; VXE; VXK; VXN; VXS; VXT; WBF; WBF_; WBG;
                WBGO; WBGP; WBU; WBU_; WCF; WCF_; WDOM; WG; WIND; WIND_; WLCP; WNF; WNF_; YDH; YDH_; YDTG; YIDG; YK;
                YN; YSEFP; YSEFT1; YSEFT2; YSFIC; YSSF; YSSG; ZF; ZJ; ZKF; ZZF_; DPUHO; EFMY; EFXY; EX; EXCCR; HF;
                ITCEEM; ITDR; ITEPR; ITF5R; ITFGR; ITFIGR; ITMR; ITNQHP; KLFHP; KNFFO; NATY; NDOMY; NGY; NIY; PCO;
                PI5O; PIFO; PIGO; PMABO; PMDVD; PMINTP; PMSO; PMTO; POIL; PRDAL; PROF; PROIHP_; PWMAB; PWMS; PWXAB;
                PWXS; PXABO; PXSO; PXTO; QAGHP; QAHHP; QAI; QCO; QI5O; QMABO; QMEO; QMSO; QMTO; QOUG; QSO; QS_;
                QWXSS; QWXS; QXABO; QXEO; QXSO; QXTO; RIPBEO; SSF3PR; SSH3RO; SSH3RW; SSHRZW; SSRDOM; SUB; SUBR;
                TFPFHP_; TIME; TVACEE; TWGR; ULAG; ULCPO; ULIL; ULIO; VAF_; VAIO; VATPC; VKF; VKFF; VSO; VS_O; VXKO;
                VXNO; ZKFO; BRUGP; VK5; SSFFX
    ENDO        $endo0; $endo1;
    ENDO0       ACAF; ACAG; AOUC; AQC; BENEF; BQY; BVY; CGU; COEFON; COTRES; DEBT; DPU; DPUF; DPUG; DPUGO; DPUH;
                DPUU; DTF; DTH; DTH1; DTH1C; EXC; EXCC; FLF; FLG; FLGR; GAP; GOSF; GOSG; GOSH; GOSH_; IDF; IDG; IDH;
                IFU; IHU; IT; ITCEE; ITCR; ITD; ITEP; ITF; ITF5; ITFC; ITFGI; ITFGO; ITFQ; ITGR; ITI5R; ITIFR;
                ITIGR; ITM; ITMQ; ITMQR; ITNQ; ITON; ITONQ; ITPL; ITPR; ITPS; ITT; IUG; KL; KLHP; KN5; KNF; KNFF;
                KNFFY; KNFY; KNI; KNIY; NFY; NFYH; OCUF; OCUG; OCUH; PAF_; PAG; PAH; PBBP; PBNP; PC; PC_; PDPUG;
                PFI; PFI_; PFND; PG; PI5; PIF; PIG; PM; PMAB; PME; PMS; PMT; PQOG; PROD; PW3; PWBG; PX; PXAB; PXB;
                PXE; PXS; PXT; QAF; QAFF; QAFF_; QAF_; QAG; QAH; QAI_; QAT; QAT_; QBBP; QBBPPOT_; QBBP_B; QBBP_P;
                QBNP; QC; QC_; QFND; QG; QGO; QI; QI5; QIF; QIG; QL; QM; QMAB; QME; QMS; QMT; QQMAB_; QS; QX; QXAB;
                QXB; QXE; QXS; QXT; Q_F; Q_I; RDEBT; RENT; RIDG; RIDGG; RIPBE; RLBE; RLBER; RLBERHP; RSBE; SBF;
                SBF3L; SBG; SBGX; SBH; SF; SG; SH; SSF; SSF3; SSF3L; SSF3P; SSFDOM; SSFF; SSFFIC; SSFG; SSH; SSH3GP;
                SSH3O; SSH3P; SSH3W; SSH3WA; SSH3WW; SSH3ZA; SSH3ZW; SSHFF; SUBCEE; TWG; TWGP; ULCP; UY; VAF; VAFF;
                VAFF_; VAG; VAH; VAI; VAI_; VAMARE; VAT; VAT_; VBBP; VBBP_B; VBBP_P; VBNP; VBNP_B
    ...         ...
    TOTAL1      VBNP_I; VBNP_P; VC; VC_; VI; VI5; VIF; VM; VMAB; VME; VMK; VMN; VMS; VMT; VS; VS_; VX; VXAB; VXB;
                VXE; VXK; VXN; VXS; VXT; WBF; WBF_; WBG; WBGO; WBGP; WBU; WBU_; WCF; WCF_; WCRH; WDOM; WG; WIND;
                WIND_; WLCP; WNF; WNF_; YDH; YDH_; YDTG; YIDG; YK; YN; YSEFP; YSEFT1; YSEFT2; YSFIC; YSSF; YSSG; ZF;
                ZJ; ZKF; ZZF_; DPUHO; DTFX; DTHX; EFMY; EFXY; EX; EXCCR; HF; ITCEEM; ITCRX; ITDR; ITEPR; ITF5R;
                ITFGR; ITFIGR; ITMR; ITNQHP; IUGR; KLFHP; KNFFO; NATY; NDOMY; NGY; NIY; OCP; PCO; PI5O; PIFO; PIGO;
                PMABO; PMDVD; PMINTP; PMSO; PMTO; POIL; PRDAL; PROF; PROIHP; PWMAB; PWMS; PWXAB; PWXS; PXABO; PXSO;
                PXTO; QAGHP; QAHHP; QAI; QCO; QI5O; QMABO; QMEO; QMSO; QMTO; QOUG; QSO; QS_; QWXAB; QWXS; QXABO;
                QXEO; QXSO; QXTO; Q_; RIPBEO; SBGXR; SSF3PR; SSH3RO; SSH3RW; SSHKB; SSHRZW; SSRDOM; SUB; SUBR_;
                TFPFHP; TFPFHP_; TIME; TVACEE; TWGR; ULAG; ULCPO; ULIL; ULIO; VAF_; VAIO; VATPC; VKF; VKF5; VKFF;
                VSO; VS_O; VXKO; VXNO; WBFRXX; WBGR; YSEFPR; YSFICR; ZKFO
    XENVI       XEX; XPWMAB; XPWMS; XPWXAB; XPWXS; XQWXAB; XQWXS; XPOIL; XNATY; XTFP
    XSCENARIO   XQWXSS; XPWXAB; XPWMAB; XRLBER; XRSBER; XNATY; XTFP; SBGXR; DTFX; DTHX
    _SCAL       acaf1; acaf2; acaf3; dlnpaf; dpuh_1; dpuh_2; dtf0; dtf1; ifu_1; ifu_2; kl1; kl10; kl11; kl12; kl13;
                kl14; kl2; knf2; knf3; knff0; knff1; nfyh1; nfyh10; nfyh11; nfyh2; nfyh3; nfyh4; pc0; pc1; pc2; pc4;
                pc5; pc6; pif_1; pif_2; pif_3; pmab0; pmab1; pmab4; pmab6; pms0; pms1; pms3; pxab1; pxs1; pxs2;
                pxs3; pxs4; qc0_; qc1_; qc2_; qc3_; qc4_; qc5_; qi5_1; qi5_2; qi5_3; qi5_4; qi5_5; qmab_1; qmab_10;
                qmab_2; qmab_4; qmab_5; qme_1; qme_2; qme_3; qms1; qms2; qms3; qms4; qmt_1; qmt_2; qmt_3; qxab0;
                qxab1; qxab2; qxab3; qxs0; qxs1; qxs2; qxs3; qxs4; qxs5; qxt0; qxt1; qxt2; qxt3; sbf3l_0; sbf3l_1;
                vah_1; vah_2; vah_3; vai1_; vai2_; vai3_; vai4_; vmn_1; vmn_2; vmn_3; vs0_; vs1_; vs2_; vs3_; vxn_1;
                vxn_2; vxn_3; zkf1; zkf2; zkf3
    _SEARCH     ZKF
    <BLANKLINE>
    """
    cdef bint ptr_owner
    cdef CKDBLists* database_ptr

    def __cinit__(self, filepath: str = None) -> Lists:
        self.database_ptr = NULL
        self.abstract_db_ptr = NULL
        self.ptr_owner = False

    def __init__(self, filepath: str = None):
        # Prevent accidental instantiation from normal Python code
        # since we cannot pass a struct pointer into a Python constructor.
        raise TypeError("This class cannot be instantiated directly.")

    def __dealloc__(self):
        # if self.database_ptr points to the C++ global instance Lists 
        # which does not need to be manually deleted 
        if self.ptr_owner and self.database_ptr is not NULL:
            del self.database_ptr
            self.database_ptr = NULL

    # see https://cython.readthedocs.io/en/stable/src/userguide/extension_types.html#instantiation-from-existing-c-c-pointers 
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

    # TODO: implement KDBAbstract::load() method (for global KDB only)
    def _load(self, filepath: str):
        cdef CKDBLists* kdb = new CKDBLists(filepath.encode())
        del kdb

    def _subset(self, pattern: str, copy: bool) -> Lists:
        cdef Lists subset_db = Lists.__new__(Lists)
        subset_db.database_ptr = subset_db.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        return subset_db

    def _get_object(self, key: str) -> List[str]:
        key = key.strip()
        str_list = self.database_ptr.get(key.encode()).decode()
        return split_list(str_list)

    def _set_object(self, key: str, value: Union[str, List[str]]):
        key = key.strip()
        if isinstance(value, str):
            value = value.strip()
            value = split_list(value)
        else:
            value = [item.strip() for item in value]
        # normalize the IODE list
        value = ';'.join(value)
        if self.database_ptr.contains(key.encode()):
            self.database_ptr.update(key.encode(), value.encode())
        else:
            self.database_ptr.add(key.encode(), value.encode())

    def from_series(self, s: Series):
        r"""
        Copy the pandas Series `s` into the IODE Lists database.
        The IODE list names to copy are deduced from the index of the Series.

        Parameters
        ----------
        s: Series
            pandas Series containing the IODE lists to copy into the IODE Lists database.

        Notes
        -----
        The index of the passed Series is sorted in alphabetical order before 
        copying to IODE Lists database.

        See Also
        --------
        Lists.to_series

        Examples
        --------
        >>> from iode import lists
        >>> import pandas as pd
        >>> lists.clear()
        >>> len(lists)
        0

        >>> # create the pandas Series
        >>> names = ["SEMICOLON_LST", "COMMA_LST", "WHITESPACE_LST", "TAB_LST", "MIX_LST"]
        >>> semicolon_lst = "A;B;C;D;E;F"
        >>> comma_lst = "A,B,C,D,E,F"
        >>> whitespace_lst = "A B C D E F" 
        >>> tab_lst = "A    B    C    D    E    F"
        >>> mix_lst = "  A;B,C D    E;F  "
        >>> data = [semicolon_lst, comma_lst, whitespace_lst, tab_lst, mix_lst]
        >>> s = pd.Series(data=data, index=names, dtype=str, name="Lists")
        >>> # display the pandas series
        >>> s          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        SEMICOLON_LST                        A;B;C;D;E;F
        COMMA_LST                            A,B,C,D,E,F
        WHITESPACE_LST                       A B C D E F
        TAB_LST               A    B    C    D    E    F
        MIX_LST                         A;B,C D    E;F
        Name: Lists, dtype: object

        >>> # load into the IODE Lists database
        >>> lists.from_series(s)
        >>> len(lists)
        5
        
        >>> lists.names
        ['COMMA_LST', 'MIX_LST', 'SEMICOLON_LST', 'TAB_LST', 'WHITESPACE_LST']
        >>> lists["SEMICOLON_LST"]
        ['A', 'B', 'C', 'D', 'E', 'F']
        >>> # note: when added or updated, an IODE list is normalized.
        >>> #       - leading and trailing whitespaces are stripped
        >>> #       - items of the IODE list are separated by a semicolon
        >>> lists["COMMA_LST"]
        ['A', 'B', 'C', 'D', 'E', 'F']
        >>> lists["MIX_LST"]
        ['A', 'B', 'C', 'D', 'E', 'F']
        """
        if pd is None:
            raise RuntimeError("pandas library not found")

        for index, value in s.items():
            self._set_object(index, value)

    def to_series(self) -> Series:
        r"""
        Create a pandas Series from the current Lists database.
        The index of the returned Series is build from the Lists names.

        See Also
        --------
        Lists.from_series

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, lists
        >>> import pandas as pd
        >>> lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")
        >>> len(lists)
        17

        >>> # Export the IODE Lists database as a pandas Series
        >>> s = lists.to_series()
        >>> len(s)
        17

        >>> s.index.to_list()               # doctest: +ELLIPSIS
        ['COPY', 'COPY0', 'COPY1', ..., 'XSCENARIO', '_SCAL', '_SEARCH']
        >>> lists["ENVI"]                   # doctest: +NORMALIZE_WHITESPACE
        ['EX', 'PWMAB', 'PWMS', 'PWXAB', 'PWXS', 'QWXAB', 'QWXS', 'POIL', 'NATY', 'TFPFHP_'] 
        >>> s["ENVI"]                       # doctest: +NORMALIZE_WHITESPACE
        "['EX', 'PWMAB', 'PWMS', 'PWXAB', 'PWXS', 'QWXAB', 'QWXS', 'POIL', 'NATY', 'TFPFHP_']"
        >>> lists["MAINEQ"]                 # doctest: +NORMALIZE_WHITESPACE
        ['W', 'NFYH', 'KNFF', 'PC', 'PXAB', 'PMAB', 'QXAB', 'QMAB', 'QC_']
        >>> s["MAINEQ"]                     # doctest: +NORMALIZE_WHITESPACE
        "['W', 'NFYH', 'KNFF', 'PC', 'PXAB', 'PMAB', 'QXAB', 'QMAB', 'QC_']"

        >>> # Export a subset of the IODE Lists database as a pandas Series
        >>> s = lists["E*;MA*"].to_series()
        >>> len(s)
        5

        >>> s.index.to_list()
        ['ENDO', 'ENDO0', 'ENDO1', 'ENVI', 'MAINEQ']
        >>> lists["ENVI"]                   # doctest: +NORMALIZE_WHITESPACE
        ['EX', 'PWMAB', 'PWMS', 'PWXAB', 'PWXS', 'QWXAB', 'QWXS', 'POIL', 'NATY', 'TFPFHP_'] 
        >>> s["ENVI"]                       # doctest: +NORMALIZE_WHITESPACE
        "['EX', 'PWMAB', 'PWMS', 'PWXAB', 'PWXS', 'QWXAB', 'QWXS', 'POIL', 'NATY', 'TFPFHP_']"
        >>> lists["MAINEQ"]                 # doctest: +NORMALIZE_WHITESPACE
        ['W', 'NFYH', 'KNFF', 'PC', 'PXAB', 'PMAB', 'QXAB', 'QMAB', 'QC_']
        >>> s["MAINEQ"]                     # doctest: +NORMALIZE_WHITESPACE
        "['W', 'NFYH', 'KNFF', 'PC', 'PXAB', 'PMAB', 'QXAB', 'QMAB', 'QC_']"
        """
        if pd is None:
            raise RuntimeError("pandas library not found")
        
        names = self.names
        data = [self._get_object(name) for name in names]
        return pd.Series(data=data, index=names, dtype=str, name=self.__class__.__name__)

    @property
    def series(self) -> Series:
        r"""
        Create a pandas Series from the current Lists database.
        The index of the returned Series is build from the Lists names.

        See Also
        --------
        Lists.to_series

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, lists
        >>> import pandas as pd
        >>> lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")
        >>> len(lists)
        17

        >>> # Export the IODE Lists database as a pandas Series
        >>> s = lists.series
        >>> len(s)
        17

        >>> s.index.to_list()               # doctest: +ELLIPSIS
        ['COPY', 'COPY0', 'COPY1', ..., 'XSCENARIO', '_SCAL', '_SEARCH']
        >>> lists["ENVI"]                   # doctest: +NORMALIZE_WHITESPACE
        ['EX', 'PWMAB', 'PWMS', 'PWXAB', 'PWXS', 'QWXAB', 'QWXS', 'POIL', 'NATY', 'TFPFHP_'] 
        >>> s["ENVI"]                       # doctest: +NORMALIZE_WHITESPACE
        "['EX', 'PWMAB', 'PWMS', 'PWXAB', 'PWXS', 'QWXAB', 'QWXS', 'POIL', 'NATY', 'TFPFHP_']" 
        >>> lists["MAINEQ"]                 # doctest: +NORMALIZE_WHITESPACE
        ['W', 'NFYH', 'KNFF', 'PC', 'PXAB', 'PMAB', 'QXAB', 'QMAB', 'QC_']
        >>> s["MAINEQ"]                     # doctest: +NORMALIZE_WHITESPACE
        "['W', 'NFYH', 'KNFF', 'PC', 'PXAB', 'PMAB', 'QXAB', 'QMAB', 'QC_']"

        >>> # Export a subset of the IODE Lists database as a pandas Series
        >>> s = lists["E*;MA*"].series
        >>> len(s)
        5

        >>> s.index.to_list()
        ['ENDO', 'ENDO0', 'ENDO1', 'ENVI', 'MAINEQ']
        >>> lists["ENVI"]                   # doctest: +NORMALIZE_WHITESPACE
        ['EX', 'PWMAB', 'PWMS', 'PWXAB', 'PWXS', 'QWXAB', 'QWXS', 'POIL', 'NATY', 'TFPFHP_'] 
        >>> s["ENVI"]                       # doctest: +NORMALIZE_WHITESPACE
        "['EX', 'PWMAB', 'PWMS', 'PWXAB', 'PWXS', 'QWXAB', 'QWXS', 'POIL', 'NATY', 'TFPFHP_']"
        >>> lists["MAINEQ"]                 # doctest: +NORMALIZE_WHITESPACE
        ['W', 'NFYH', 'KNFF', 'PC', 'PXAB', 'PMAB', 'QXAB', 'QMAB', 'QC_']
        >>> s["MAINEQ"]                     # doctest: +NORMALIZE_WHITESPACE
        "['W', 'NFYH', 'KNFF', 'PC', 'PXAB', 'PMAB', 'QXAB', 'QMAB', 'QC_']"
        """
        return self.to_series()

    def _str_table(self, names: List[str]) -> str:
        columns = {"name": names, "lists": ['; '.join(self._get_object(name)) for name in names]}
        return table2str(columns, max_lines=10, justify_funcs={"name": JUSTIFY.LEFT, "lists": JUSTIFY.LEFT})


lists: Lists = Lists._from_ptr()
