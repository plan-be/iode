# distutils: language = c++

from collections.abc import Iterable
from typing import Union, Tuple, List, Optional, Any
if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

cimport cython
from pyiode.iode_database.database cimport KDBIdentities as CKDBIdentities
from pyiode.iode_database.database cimport Identities as cpp_global_identities
from pyiode.iode_database.database cimport Variables as cpp_global_variables


@cython.final
cdef class _IdentitiesDatabase(_AbstractDatabase):
    cdef CKDBIdentities* database_ptr

    def __cinit__(self):
        self.database_ptr = self.abstract_db_ptr = &cpp_global_identities

    def __dealloc__(self):
        # self.database_ptr points to the C++ global instance Identities 
        # which does not need to be manually deleted 
        pass

    # TODO: implement KDBAbstract::load() method (for global KDB only)
    def _load(self, filepath: str):
        cdef CKDBIdentities* kdb = new CKDBIdentities(filepath.encode())
        del kdb

    def subset(self, pattern: str, copy: bool = False) -> _IdentitiesDatabase:
        cmt_subset = _IdentitiesDatabase()
        cmt_subset.database_ptr = cmt_subset.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        return cmt_subset

    def _get_object(self, key):
        if not isinstance(key, str):
            raise TypeError(f"Cannot get object {key}.\nExpected a string value for {key} " + 
                "but got value of type {type(filepath).__name__}")
        return self.database_ptr.get_lec(key.encode()).decode()

    def _set_object(self, key, value):
        if not isinstance(key, str):
            raise TypeError(f"Cannot set object {key}.\nExpected a string value for {key} " + 
                "but got value of type {type(filepath).__name__}")
        if self.database_ptr.contains(key.encode()):
            self.database_ptr.update(key.encode(), value.encode())
        else:
            self.database_ptr.add(key.encode(), value.encode())
    
    def add(self, name: str, identity: str):
        if not isinstance(name, str):
            raise TypeError(f"'name': Expected value of type string. Got value of type {type(name).__name__}")
        self.database_ptr.add(name.encode(), identity.encode())

    def update(self, name: str, identity: str):
        if not isinstance(name, str):
            raise TypeError(f"'name': Expected value of type string. Got value of type {type(name).__name__}")
        self.database_ptr.update(name.encode(), identity.encode())

    def execute(self, identities: Union[str, List[str]] = None, from_period: Union[str, Period] = None, 
        to_period: Union[str, Period] = None, var_files: Union[str, List[str]] = None, 
        scalar_files: Union[str, List[str]] = None, trace: bool = False):
        """
        Execute the specified identity(ies).

        Parameters
        ----------
        identities : str or List[str], optional
            The identities to execute. If not provided, all identities are executed.
        from_period : str or Period, optional
            The starting period of the execution range. 
            Defaults to the starting period of the current Variables sample.
        to_period : str or Period, optional
            The ending period of the execution range.
            Defaults to the ending period of the current Variables sample.
        var_files : str or List[str], optional
            The variable files from which the required Variables are temporarily loaded.
            The Variables database is left unchanged after the execution of the identity(ies).
            Defaults to None (current content of the Variables database is used). 
        scalar_files : str or List[str], optional
            The scalar files from which the required Scalars are temporarily loaded.
            The Variables database is left unchanged after the execution of the identity(ies).
            Defaults to None (current content of the Scalars database is used).
        trace : bool, optional
            If True, prints the source of the variables and scalars.

        Returns
        -------
        None

        Examples
        --------
        >>> from iode import Identities, Variables, SAMPLE_DATA_DIR
        >>> Identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")
        >>> Variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> sample = Variables.sample
        >>> sample
        1960Y1:2015Y1
        >>> Identities["GAP_"]
        '100*((QAF_/Q_F)-1)'
        >>> Identities["GAP2"]
        '100*(QAFF_/(Q_F+Q_I))'
        >>> # reset variables GAP_ and GAP2
        >>> Variables["GAP_"] = 0.
        >>> Variables["GAP_"]                   # doctest: +ELLIPSIS 
        [0.0, 0.0, 0.0, ..., 0.0, 0.0, 0.0]
        >>> Variables["GAP2"] = 0.
        >>> Variables["GAP2"]                   # doctest: +ELLIPSIS 
        [0.0, 0.0, 0.0, ..., 0.0, 0.0, 0.0]

        >>> # compute GAP_ and GAP2 (assuming Scalars and Variables are already loaded)
        >>> Identities.execute("GAP_;GAP2")
        >>> Variables["GAP_"]                   # doctest: +ELLIPSIS 
        [-3.20493949860704, -3.981808446333557, ..., 3.7800671441993616, 3.2396415884531793]
        >>> Variables["GAP2"]                   # doctest: +ELLIPSIS 
        [96.92655844699298, 97.39603007168847, ..., 102.14581982070376, 101.58578527761608]

        >>> # compute GAP_ and GAP2 over a subset of the sample
        >>> Variables["GAP_"] = 0.
        >>> Variables["GAP2"] = 0.
        >>> Identities.execute("GAP_;GAP2", "2000Y1", "2005Y1")
        >>> Variables["GAP_", "2000Y1:2005Y1"]
        [4.510095736743436, 3.312304975734315, 2.6151793579969107, 3.464117181974924, 5.478645527985804, 5.578699398837528]
        >>> Variables["GAP2", "2000Y1:2005Y1"]
        [104.60957761618035, 103.05782573291968, 102.17336700422976, 102.82322081548728, 104.4719275849864, 104.3586710898436]

        >>> # compute GAP_ and GAP2 assuming Variables are not already loaded
        >>> Variables.clear()
        >>> Variables.sample = '1960Y1:2015Y1'
        >>> Variables.get_names()
        []
        >>> # setting the var_files argument will fetch the required values of 
        >>> # 'QAF_', 'QAFF_', 'Q_F' and 'Q_I' from the passed Variables file
        >>> Identities.execute("GAP_;GAP2", var_files=f"{SAMPLE_DATA_DIR}/fun.var")
        >>> Variables["GAP_"]                   # doctest: +ELLIPSIS 
        [-3.20493949860704, -3.981808446333557, ..., 3.7800671441993616, 3.2396415884531793]
        >>> Variables["GAP2"]                   # doctest: +ELLIPSIS 
        [96.92655844699298, 97.39603007168847, ..., 102.14581982070376, 101.58578527761608]
        >>> # note that the variables 'QAF_', 'QAFF_', 'Q_F' and 'Q_I' are not 
        >>> # present in the Variables database after running Identities.execute
        >>> Variables.get_names()
        ['GAP2', 'GAP_']
        """
        if identities is None:
            identities = ""
        if isinstance(identities, str):
            pass
        elif isinstance(identities, Iterable) and all(isinstance(item, str) for item in identities):
            identities = ';'.join(identities)
        else:
            raise TypeError("'identities': Expected value of type str or a list of str. "
                            f"Got value of type {type(identities).__name__}")
        
        if from_period is None or to_period is None:
            c_sample = cpp_global_variables.get_sample()
            if from_period is None:
                from_period = c_sample.start_period().to_string().decode()
            if to_period is None:
                to_period = c_sample.end_period().to_string().decode()
        
        if isinstance(from_period, Period):
            from_period = str(from_period)
        if not isinstance(from_period, str):
            raise TypeError("'from_period': Expected value of type str or Period. "
                            f"Got value of type {type(from_period).__name__}")

        if isinstance(to_period, Period):
            to_period = str(to_period)
        if not isinstance(to_period, str):
            raise TypeError("'to_period': Expected value of type str or Period. "
                            f"Got value of type {type(to_period).__name__}")

        if var_files is None:
            var_files = ""
        if isinstance(var_files, str):
            pass
        elif isinstance(var_files, Iterable) and all(isinstance(item, str) for item in var_files):
            var_files = ';'.join(var_files)
        else:
            raise TypeError("'var_files': Expected value of type str or a list of str. "
                            f"Got value of type {type(var_files).__name__}")

        if scalar_files is None:
            scalar_files = ""
        if isinstance(scalar_files, str):
            pass
        elif isinstance(scalar_files, Iterable) and all(isinstance(item, str) for item in scalar_files):
            scalar_files = ';'.join(scalar_files)
        else:
            raise TypeError("'scalar_files': Expected value of type str or a list of str. "
                            f"Got value of type {type(scalar_files).__name__}")

        self.database_ptr.execute_identities(from_period.encode(), to_period.encode(), identities.encode(), 
                                            var_files.encode(), scalar_files.encode(), <bint>trace)



Identities = _IdentitiesDatabase()
