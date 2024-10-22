# Import necessary types/classes
from libcpp.string cimport string
from libcpp.vector cimport vector

from pyiode.compute.simulation cimport CSimulation
from pyiode.common cimport SimuSortAlgorithm, VariablesInitialization


# Simulation wrapper class
# see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#create-cython-wrapper-class 
cdef class Simulation:
    r"""
    Class for simulate models.

    Models
    ~~~~~~

    IN IODE, a model is simply a list of equations. No other construction is necessary: in this way, 
    to modify a model, the only thing to do is to modify the list that defines it. 
    For example, if a model is logically divided into 5 blocks, 5 lists of equations will be defined:: 

        BLOC1 : A, B, C, D
        BLOC2 : X, Y
        BLOC3 : C1, C2, C3, C4
        BLOC4 : X_A1, X_A2, X_A3
        BLOC5 : R1, R2, R3, S_12
        MODSIM : $BLOC1, $BLOC2, $BLOC3, $BLOC4, $BLOC5

    To simulate a model, all equations of the model must have been loaded or created. In addition, 
    all the variables and scalars used in the model equations must have been loaded or created. 
    Values of exogenous variables and of scalar cannot be :math:`NA` (*Not Available*) over the 
    simulation periods.

    Exchange Endo-Exo
    ~~~~~~~~~~~~~~~~~

    It is possible to set endogenous-exogenous pairs for ``goal seeking``. 
    For each pair, the status of the variables is exchanged: endogenous becomes exogenous and vice versa. 
    This enables the model to be run through known endogenous values and to deduce the necessary values 
    for the associated exogenous variables. The exogenous variables take on the value calculated over the entire 
    workspace period.
    See the method :meth:`Simulation.model_exchange`.

    Newton-Raphson Algorithm
    ~~~~~~~~~~~~~~~~~~~~~~~~

    When an equation is not explicitly defined in terms of its endogen, or when the endogen appears several times 
    in the equation, the simulation algorithm tries to solve the equation using a Newton method. 
    If this method fails, a secant method is used to find a solution to the equation. 
    However, there is no guarantee that a solution will be found in every case. 
    Non-continuous functions (singularities) such as:

    .. math::`X = a + \frac{b}{X + 1}`

    may be impossible to solve around their singular point. For this type of problem, the only solution is to modify 
    the form of the equation:

    .. math::`(X - a) * (X + 1) = b`

    Warnings
    --------
    - As the endogenous variable of an equation carries the name of the equation, it is not possible to place 
      in a model two equations with the same endogenous variable.
    - The order in which equations are introduced in the lists can influence the behavior of the simulation algorithm. 
      As this is a Gauss-Seidel algorithm, the information calculated is used directly in the rest of the calculation. 
      If `X` depends on `Y`, it's better to place `Y` before `X` in the list of equations.

    Attributes
    ----------
    convergence_threshold: float
        Convergence threshold below which the simulation results are considered enough close 
        to the true solution for the current period. 
        Default to 0.001.
    relax: float
        Relaxation parameter ("damping").
        Its value must be in the range [0.1, 1.0].
        Default to 1.0
    max_nb_iterations: int
        Maximum number of iterations to reach a solution. If, after the specified maximum number 
        of iterations, the model has not converged, the process stops with an error message. 
        This parameter prevents the process from looping indefinitely.  
        Default to 100.
    sort_algorithm: SimulationSort
        Sorting algorithm used to reorganized the list of equations *before* the simulation is performed.
        This reorganization can be usefully to to speed up the simulation.
        Possible values are:
            - NONE: do not use any sorting algorithm
            - CONNEX: use the *Strongly Connected Component (SCC)* method only
            - BOTH: use both the *SCC* and the *Pseudo-triangulation* methods
        Default to BOTH.
    initialization_method: SimulationInitialization
        At the start of each period to be simulated, a starting value must be chosen for each endogenous variable. 
        Possible values are:
            - TM1: :math:`Y := Y[-1], if Y null or NA`
            - TM1_A: :math:`Y := Y[-1], always`
            - EXTRA: :math:`Y := extrapolation, if Y null or NA`
            - EXTRA_A: :math:`Y := extrapolation, always`
            - ASIS: :math:`Y unchanged`
            - TM1_NA: :math:`Y := Y[-1], if Y = NA`
            - EXTRA_NA: :math:`Y := extrapolation, if Y = NA`
        Default to TM1.
    debug: bool
        Option to automatically generates IODE lists containing pre- and post-recursive equations: 
        `_PRE`, `_INTER` and `_POST`.
        Default to False.
    nb_passes: int
        Number of passes to make when the pseudo-triangulation sorting algorithm is used.
        Default to 5.
    debug_newton: bool
        Save a trace of the sub-iterations when the Newton-Raphson alogrithm is used.
        Default to False.

    Examples
    --------
    >>> from iode import SAMPLE_DATA_DIR, equations, scalars, variables 
    >>> from iode import Simulation
    >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
    >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")
    >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")

    >>> simu = Simulation()
    >>> simu.convergence_threshold
    0.001
    >>> simu.relax
    1.0
    >>> simu.max_nb_iterations
    100
    >>> simu.sort_algorithm
    'BOTH (Connex compon. + Triangulation)'
    >>> simu.initialization_method
    'TM1 (Y := Y[-1], if Y null or NA)'
    >>> simu.debug
    False
    >>> simu.nb_passes
    5
    >>> simu.debug_newton
    False
    """
    cdef CSimulation* c_simulation

    # See function B_ModelSimulateParms() from api/report/commands/b_model.c and global variables in api/simulation/k_sim_main.c
    def __cinit__(self, convergence_threshold: float = 0.001, relax: float = 1.0, max_nb_iterations: int = 100, 
                  sort_algorithm: Union[SimulationSort, str] = SimulationSort.BOTH, 
                  initialization_method: Union[SimulationInitialization, str] = SimulationInitialization.TM1, 
                  debug: bool = False, nb_passes: int = 5, debug_newton: bool = False):
        self.c_simulation = new CSimulation()
        self.convergence_threshold = convergence_threshold
        self.relax = relax
        self.max_nb_iterations = max_nb_iterations
        self.sort_algorithm = sort_algorithm
        self.initialization_method = initialization_method
        self.debug = debug
        self.nb_passes = nb_passes
        self.debug_newton = debug_newton

    def __dealloc__(self):
        del self.c_simulation

    @property
    def convergence_threshold(self) -> float:
        r"""
        Indicates the convergence threshold below which the simulation ends positively for the current period.
        In other words, for a given period, when the relative differences between two successive iterations (k and k-1)
        
        .. math:: \frac{\|Y[k] - Y[k-1]\|}{\|Y[k]\|}
        
        are below this threshold for all endogenous variables, the model is considered to have converged.
        Default value is 0.001.

        Parameters
        ----------
        value: float
            New convergence threshold value.

        Examples
        --------
        >>> from iode import Simulation
        >>> simu = Simulation()
        >>> simu.convergence_threshold
        0.001
        >>> simu.convergence_threshold = 1e-6
        >>> simu.convergence_threshold
        1e-06
        """
        return self.c_simulation.get_convergence_threshold()

    @convergence_threshold.setter
    def convergence_threshold(self, value: float):
        self.c_simulation.set_convergence_threshold(value)

    @property
    def relax(self) -> float:
        """
        Relaxation parameter.
        
        It modifies the algorithm's behavior in the following way in the following way: the variable :math:`X`, 
        calculated by the associated model equation, is not for subsequent iterations, but replaced by 

        .. math:: X[k] = X^\prime * \lambda + X[k-1] * (1 - \lambda)

        where: 
        
          - :math:`X^\prime` is the result of calculating the equation,
          - :math:`\lambda` is the relaxation parameter,
          - :math:`X[k-1]` is the result of the calculation at the previous iteration,
          - :math:`X[k]` is the new value
        
        If :math:`\lambda` is 1, :math:`X[k]` is the result of calculating the equation without modification.
        If :math:`\lambda` is 0, :math:`X` will never change. 
        
        It is recommended to set this parameter to a value between 0.5 and 1 when a model doesn't converge
        or converges too slowly.

        Default value is 1.0.

        Parameters
        ----------
        value: float
            New relaxation parameter value. Its value must be in the range [0.1, 1.0]. 

        Examples
        --------
        >>> from iode import Simulation
        >>> simu = Simulation()
        >>> simu.relax
        1.0
        >>> simu.relax = 0.9
        >>> simu.relax
        0.9
        """
        return self.c_simulation.get_relax()

    @relax.setter
    def relax(self, value: float):
        if not 0.1 <= value <= 1.0:
            raise ValueError(f"'relax': Expected value between 0.1 and 1.0. Got value {value} instead.")
        self.c_simulation.set_relax(value)

    @property
    def max_nb_iterations(self) -> int:
        """
        Maximum number of iterations to reach a solution. If, after the specified maximum number 
        of iterations, the model has not converged, the process stops with an error message. 
        This parameter prevents the process from looping indefinitely.
        Default value is 100.

        Parameters
        ----------
        value: int
            New maximum number of iterations allowed.

        Examples
        --------
        >>> from iode import Simulation
        >>> simu = Simulation()
        >>> # default value
        >>> simu.max_nb_iterations
        100
        >>> simu.max_nb_iterations = 1000
        >>> simu.max_nb_iterations
        1000
        """
        return self.c_simulation.get_max_nb_iterations()

    @max_nb_iterations.setter
    def max_nb_iterations(self, value: int):
        self.c_simulation.set_max_nb_iterations(value)

    # see file api\lec\l_newton.c
    @property
    def max_nb_iterations_newton(self) -> int:
        """
        Maximum number of iterations when the Newton-Raphson algorithm is used.
        Default to 50.

        Parameters
        ----------
        value: int

        Examples
        --------
        >>> from iode import Simulation
        >>> simu = Simulation()
        >>> # default value
        >>> simu.max_nb_iterations_newton
        50
        >>> simu.max_nb_iterations_newton = 100
        >>> simu.max_nb_iterations_newton
        100
        """
        return self.c_simulation.get_max_nb_iterations_newton()

    @max_nb_iterations_newton.setter
    def max_nb_iterations_newton(self, value: int):
        if value < 0:
            raise ValueError(f"'max_nb_iterations_newton': Value must be positive. Got value {value} instead.")
        self.c_simulation.set_max_nb_iterations_newton(value)

    @property
    def sort_algorithm(self) -> str:
        """
        Sorting algorithm used to reorganized the list of equations *before* the simulation is performed.
        This reorganization can be usefully to to speed up the simulation. 

        Two methods can be used separately or in combination. Both use algorithms from the *graph theory*.

        Strongly Connected Component (SCC)
        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        this involves dividing the model into non-interdependent blocks. 
        This method limits the number of equations to be calculated, based on the dependencies of 
        the equations on each other. 
        
        In the following example, blocks 1 and 3 are non-interdependent, meaning that only one calculation 
        is required per period. The relaxation parameter is not used in these blocks. 
        Block 2 is the interdependent part of the model. This block is the only one to be iterated on. 
        This reduces the number of equations to be solved to 3, instead of the 6 that actually make up the model. 
        Decomposition can result in more than three blocks::

            Let's consider the following equations:
            A = f1(B,C,D)
            B = f2(C,A)
            C = f3(B,X)
            D = f4(X,Y)
            X = f5(Y)
            Z = f6(A,B,C,X)

            The following blocks are built (in this order):
            1: X = f5(Y)       depends only on exogenous Y
               D = f4(X,Y)     depends on X
            2: A = f1(B,C,D)   loop with B and C
               B = f2(C,A)     loop with A and C
               C = f3(B,X)     loops with B, then with A
            3: Z = f6(A,B,C,X) is not used by any equation

        Pseudo-triangulation
        ~~~~~~~~~~~~~~~~~~~~

        This method reorganizes equations using a specific algorithm. 
        Basically, dependencies are analyzed and equations are organized accordingly: if B depends on A, 
        B is placed after A, and so on. Several passes are usually necessary to obtain a satisfactory result. 
        The optimal number of passes depends on the model and is left to the user's choice.

        Parameters
        ----------
        value: SimulationSort or str
            New value for the used sorting algorithm. 
            Possible values are:
              - NONE: do not use any sorting algorithm
              - CONNEX: use the *Strongly Connected Component (SCC)* method only
              - BOTH: use both the *SCC* and the *Pseudo-triangulation* methods

        Examples
        -------- 
        >>> from iode import Simulation, SimulationSort
        >>> simu = Simulation()
        >>> # default value
        >>> simu.sort_algorithm
        'BOTH (Connex compon. + Triangulation)'

        >>> simu.sort_algorithm = "none"
        >>> simu.sort_algorithm
        'NONE (None)'
        >>> simu.sort_algorithm = SimulationSort.NONE
        >>> simu.sort_algorithm
        'NONE (None)'

        >>> simu.sort_algorithm = "connex"
        >>> simu.sort_algorithm
        'CONNEX (Connex compon. decomposition)'
        >>> simu.sort_algorithm = SimulationSort.CONNEX
        >>> simu.sort_algorithm
        'CONNEX (Connex compon. decomposition)'

        >>> simu.sort_algorithm = "both"
        >>> simu.sort_algorithm
        'BOTH (Connex compon. + Triangulation)'
        >>> simu.sort_algorithm = SimulationSort.BOTH
        >>> simu.sort_algorithm
        'BOTH (Connex compon. + Triangulation)'
        """
        return f"{SimulationSort(<int>(self.c_simulation.get_sort_algorithm())).name} " + \
               f"({self.c_simulation.get_sort_algorithm_as_string().decode()})"

    @sort_algorithm.setter
    def sort_algorithm(self, value: Union[SimulationSort, str]):
        if isinstance(value, str):
            value = value.upper()
            value = SimulationSort[value]
        value = int(value)
        self.c_simulation.set_sort(<SimuSortAlgorithm>value)

    @property
    def initialization_method(self) -> str:
        """
        At the start of each period to be simulated, a starting value must be chosen for each endogenous variable. 
        This value can be :
          - :math:`Y := Y[-1], if Y null or NA` (TM1) : each null or NA endogen at the start takes the value of 
            the previous period,
          - :math:`Y := Y[-1], always` (TM1_A) : each endogen takes the value of the previous period at the start,
          - :math:`Y := extrapolation, if Y null or NA` (EXTRA) : each null or NA endogen takes as value a linear 
            extrapolation of the two previous periods,
          - :math:`Y := extrapolation, always` (EXTRA_A) : each endogen takes as its value a linear extrapolation of 
            the two preceding periods, whether or not it is zero at the start,
          - :math:`Y unchanged` (ASIS): endogenous values are not initialized. They retain their value whether or 
            not they are zero,
          - :math:`Y := Y[-1], if Y = NA` (TM1_NA): each NA value takes the value of the previous period,
          - :math:`Y := extrapolation, if Y = NA` (EXTRA_NA): each NA value takes the value of a linear extrapolation of 
            the two previous periods.

        Parameters
        ----------
        value: SimulationInitialization or str
            initialization method. Possible values are:
              - TM1: :math:`Y := Y[-1], if Y null or NA`
              - TM1_A: :math:`Y := Y[-1], always`
              - EXTRA: :math:`Y := extrapolation, if Y null or NA`
              - EXTRA_A: :math:`Y := extrapolation, always`
              - ASIS: :math:`Y unchanged`
              - TM1_NA: :math:`Y := Y[-1], if Y = NA`
              - EXTRA_NA: :math:`Y := extrapolation, if Y = NA`

        Examples
        -------- 
        >>> from iode import Simulation, SimulationInitialization
        >>> simu = Simulation()
        >>> # default value
        >>> simu.initialization_method
        'TM1 (Y := Y[-1], if Y null or NA)'

        >>> simu.initialization_method = "TM1"
        >>> simu.initialization_method
        'TM1 (Y := Y[-1], if Y null or NA)'
        >>> simu.initialization_method = SimulationInitialization.TM1
        >>> simu.initialization_method
        'TM1 (Y := Y[-1], if Y null or NA)'

        >>> simu.initialization_method = "TM1_A"
        >>> simu.initialization_method
        'TM1_A (Y := Y[-1], always)'
        >>> simu.initialization_method = SimulationInitialization.TM1_A
        >>> simu.initialization_method
        'TM1_A (Y := Y[-1], always)'

        >>> simu.initialization_method = "EXTRA"
        >>> simu.initialization_method
        'EXTRA (Y := extrapolation, if Y null or NA)'
        >>> simu.initialization_method = SimulationInitialization.EXTRA
        >>> simu.initialization_method
        'EXTRA (Y := extrapolation, if Y null or NA)'

        >>> simu.initialization_method = "EXTRA_A"
        >>> simu.initialization_method
        'EXTRA_A (Y := extrapolation, always)'
        >>> simu.initialization_method = SimulationInitialization.EXTRA_A
        >>> simu.initialization_method
        'EXTRA_A (Y := extrapolation, always)'

        >>> simu.initialization_method = "ASIS"
        >>> simu.initialization_method
        'ASIS (Y unchanged)'
        >>> simu.initialization_method = SimulationInitialization.ASIS
        >>> simu.initialization_method
        'ASIS (Y unchanged)'

        >>> simu.initialization_method = "TM1_NA"
        >>> simu.initialization_method
        'TM1_NA (Y := Y[-1], if Y = NA)'
        >>> simu.initialization_method = SimulationInitialization.TM1_NA
        >>> simu.initialization_method
        'TM1_NA (Y := Y[-1], if Y = NA)'

        >>> simu.initialization_method = "EXTRA_NA"
        >>> simu.initialization_method
        'EXTRA_NA (Y := extrapolation, if Y = NA)'
        >>> simu.initialization_method = SimulationInitialization.EXTRA_NA
        >>> simu.initialization_method
        'EXTRA_NA (Y := extrapolation, if Y = NA)'
        """
        return f"{SimulationInitialization(<int>(self.c_simulation.get_initialization_method())).name} " + \
               f"({self.c_simulation.get_initialization_method_as_string().decode()})"

    @initialization_method.setter
    def initialization_method(self, value: Union[SimulationInitialization, str]):
        if isinstance(value, str):
            value = value.upper()
            value = SimulationInitialization[value]
        value = int(value)
        self.c_simulation.set_initialization_method(<VariablesInitialization>value)
    
    @property
    def debug(self) -> bool:
        """
        Option to automatically generates IODE lists containing pre-recursive, interdependent 
        and post-recursive equations: `_PRE`, `_INTER` and `_POST`.
        It is used with the method :meth:`Simulation.model_simulate`.
        Default to False.

        Parameters
        ----------
        value: bool 
        
        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations, lists, scalars, variables 
        >>> from iode import Simulation
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
        >>> lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")

        >>> simu = Simulation()
        >>> # force IODE to create the three list _PRE, _INTER and _POST 
        >>> # when the method model_simulate is called 
        >>> simu.debug = True
        >>> simu.model_simulate("2000Y1", "2015Y1");

        >>> lists["_PRE"]           # doctest: +ELLIPSIS
        ['BRUGP', 'DTH1C', 'EX', 'ITCEE', ..., 'ZZF_', 'DTH1', 'PME', 'PMS', 'PMT']
        >>> len(lists["_PRE"])
        31

        >>> lists["_INTER"]         # doctest: +ELLIPSIS
        ['PMAB', 'PXAB', 'ITFGO', 'ITFGI', ..., 'FLG', 'VBNP', 'VBNP_P', 'VBBP_P']
        >>> len(lists["_INTER"])
        204

        >>> lists["_POST"]          # doctest: +ELLIPSIS
        ['IFU', 'SSHFF', 'PBBP', 'OCUF', ..., 'GAP', 'FLGR', 'FLF', 'DPUU', 'BENEF']
        >>> len(lists["_POST"])
        39
        """
        return self.c_simulation.is_debug_active()

    @debug.setter
    def debug(self, value: bool):
        self.c_simulation.set_debug(value)

    @property
    def debug_newton(self) -> bool:
        """
        Save a trace of the sub-iterations when the Newton-Raphson alogrithm is used.
        Default to False.

        Parameters
        ----------
        value: bool

        Examples
        --------
        >>> from iode import Simulation
        >>> simu = Simulation()
        >>> # default value
        >>> simu.debug_newton
        False
        """
        return self.c_simulation.is_debug_newton_active()

    @debug_newton.setter
    def debug_newton(self, value: bool):
        self.c_simulation.set_debug_newton(value)

    @property
    def nb_passes(self) -> int:
        """
        Number of passes to make when the pseudo-triangulation sorting algorithm is used.
        Default to 5.

        See Also
        --------
        Simulation.sort_algorithm

        Parameters
        ----------
        value: int
        
        Examples
        --------
        >>> from iode import Simulation
        >>> simu = Simulation()
        >>> # default value
        >>> simu.nb_passes 
        5
        >>> simu.nb_passes = 10
        >>> simu.nb_passes
        10
        """
        return self.c_simulation.get_nb_passes()

    @nb_passes.setter
    def nb_passes(self, value: int):
        if value < 0:
            raise ValueError(f"'nb_passes': Value cannot be negative. Got value {value}")
        self.c_simulation.set_nb_passes(value)

    def model_exchange(self, list_exo: Union[str, List[str]]=None):
        """
        Set a list of endogenous-exogenous pairs for ``goal seeking``. 
        For each pair, the status of the variables is exchanged: endogenous becomes exogenous and vice versa. 
        This enables the model to be run through known endogenous values and to deduce the necessary values 
        for the associated exogenous variables. The exogenous variables take on the value calculated over the entire 
        workspace period.

        Parameters
        ----------
        list_exo : str or list(str)
            List of endogenous-exogenous pairs. 
            Each pair must be written as `endogenous_name-exogenous_name`.
            If the passed list is empty, the internal list of endogenous-exogenous pairs is reset.
            Default to None (empty).

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations, identities, scalars, variables 
        >>> from iode import Simulation
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
        >>> identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")

        >>> equations["UY"].lec
        'UY := NATY-NDOMY-NIY-NGY-(EFXY-EFMY)-NFY'
        >>> identities["XNATY"]
        Identity('grt NATY')
        >>> # endogenous variable
        >>> variables["XNATY", "2000Y1:2015Y1"]     # doctest: +ELLIPSIS
        [0.22, 0.699999988079071, 0.4, 0.7, ..., -0.20000000298023224, -0.20000000298023224]
        >>> # reset exogeneous variable
        >>> variables["UY", "2000Y1:2015Y1"] = 0.0
        >>> variables["UY", "2000Y1:2015Y1"]    # doctest: +ELLIPSIS
        [0.0, 0.0, ..., 0.0, 0.0]

        >>> simu = Simulation()
        >>> simu.model_simulate("2000Y1", "2015Y1")
        >>> # endogenous variable (unchanged)
        >>> variables["XNATY", "2000Y1:2015Y1"]     # doctest: +ELLIPSIS
        [0.22, 0.699999988079071, 0.4, 0.7, ..., -0.20000000298023224, -0.20000000298023224]
        >>> # exogeneous variable
        >>> variables["UY", "2000Y1:2015Y1"]         # doctest: +ELLIPSIS
        [624.1781881798956, 645.0542977503601, 661.6074127102179, ..., 525.1258272992568]

        >>> # exchange UY and XNATY
        >>> simu.model_exchange("UY-XNATY")
        >>> # update endogenous variable (now UY)
        >>> variables["UY", "2000Y1:2002Y1"] = [630.0, 650.0, 670.0]
        >>> # rerun simulation
        >>> simu.model_simulate("2000Y1", "2015Y1")
        >>> # exogeneous variable (now XNATY)
        >>> variables["XNATY", "2000Y1:2015Y1"]     # doctest: +ELLIPSIS
        [0.3508274950028535, 0.6702440874066322, 0.5092369210188463, ..., -0.19926923081414763]
        """
        if list_exo is None:
            list_exo = ''
        if not isinstance(list_exo, str) and isinstance(list_exo, Iterable) and \
            all(isinstance(item, str) for item in list_exo):
            list_exo = ';'.join(list_exo)

        self.c_simulation.model_exchange(list_exo.encode())

    def model_compile(self, list_eqs: Union[str, List[str]]=None):
        """
        Recompiles a list of equations, or all equations if no list is specified. 
        It is only useful if (the list of) equations contain macros in their LEC form (like $list).

        The set of equations stored in memory are made up of several elements: the LEC form, 
        the estimation inputs and the **compiled form** of the equation. If the LEC form 
        contains macros, the **compiled form** may no longer correspond to the LEC form.
        This can happen when a new list file is loaded into memory, when a list is manually 
        modified or copied from disk, or when an equation file is loaded into memory.

        Parameters
        ----------
        list_eqs : str or list(str), optional
            List of equations to recompile. 
            Default to empty (recompile all equations).
        """
        if list_eqs is None:
            list_eqs = ''
        if not isinstance(list_eqs, str) and isinstance(list_eqs, Iterable) and \
            all(isinstance(item, str) for item in list_eqs):
            list_eqs = ';'.join(list_eqs)
        
        self.c_simulation.model_compile(list_eqs.encode())

    def model_simulate(self, from_period: Union[str, Period], to_period: Union[str, Period], 
                       list_eqs: Union[str, List[str]]=None):
        """
        Run the simulation of a model for a given sample.

        To simulate a model, all equations of the model must have been loaded or created. In addition, 
        all the variables and scalars used in the model equations must have been loaded or created. 
        Values of exogenous variables and of scalar cannot be :math:`NA` (*Not Available*) over the 
        simulation sample.

        Parameters
        ----------
        from_period : str or Period
            The starting period for the simulation.
        to_period : str or Period
            The ending period for the simulation.
        list_eqs : str or list(str), optional
            List of equations representing the model.
            Default to empty (all equations).

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations, identities, lists, scalars, variables 
        >>> from iode import Simulation
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
        >>> identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")
        >>> lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")

        >>> # exogeneous variable 
        >>> equations["UY"].lec
        'UY := NATY-NDOMY-NIY-NGY-(EFXY-EFMY)-NFY'
        >>> # reset values
        >>> variables["UY", "2000Y1:2015Y1"] = 0.0
        >>> variables["UY", "2000Y1:2015Y1"]    # doctest: +ELLIPSIS
        [0.0, 0.0, ..., 0.0, 0.0]

        >>> # endogenous variable
        >>> identities["XNATY"]
        Identity('grt NATY')
        >>> variables["XNATY", "2000Y1:2015Y1"]     # doctest: +ELLIPSIS
        [0.22, 0.699999988079071, 0.4, 0.7, ..., -0.20000000298023224, -0.20000000298023224]

        >>> # simulate the model (no reordering)
        >>> simu = Simulation()
        >>> simu.model_simulate("2000Y1", "2015Y1")
        >>> variables["UY", "2000Y1:2015Y1"]    # doctest: +ELLIPSIS
        [624.1781881798956, 645.0542977503601, 661.6074127102179, ..., 525.1258272992568]
        """
        if isinstance(from_period, Period):
            from_period = str(from_period)

        if isinstance(to_period, Period):
            to_period = str(to_period)

        if list_eqs is None:
            list_eqs = ''
        if not isinstance(list_eqs, str) and isinstance(list_eqs, Iterable) and \
            all(isinstance(item, str) for item in list_eqs):
            list_eqs = ';'.join(list_eqs)

        self.c_simulation.model_simulate(from_period.encode(), to_period.encode(), list_eqs.encode())

    def model_calculate_SCC(self, nb_iterations: int, pre_name: str="_PRE", inter_name: str="_INTER", 
                            post_name: str="_POST", list_eqs: Union[str, List[str]]=None):
        """
        Decompose the model into Strongly Connex Components (SCC) and reorder it. 
        
        Three lists are created: pre-recursive, interdependent and post-recursive equations.
        When reordering the model, the number of triangulation iterations for the interdependent 
        block must be specified. This value only affects the list of interdependent equations.
        These 3 lists only need to be constructed once for a given model.

        Parameters
        ----------
        nb_iterations : int
            Number of triangulation iterations to be performed. 
        pre_name : str, optional
            Name of the list representing the pre-recursive equations.
            Default to "_PRE".
        inter_name : str, optional
            Name of the list representing the interdependent equations. 
            Default to "_INTER".
        post_name : str, optional
            Name of the list representing the post-recursive equations.
            Default to "_POST".
        list_eqs : str or list(str), optional
            List of equations representing the model.
            Default to empty (all equations).

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations, lists, scalars, variables 
        >>> from iode import Simulation
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
        >>> lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")

        >>> simu = Simulation()
        >>> simu.model_calculate_SCC(10);

        >>> lists["_PRE"]           # doctest: +ELLIPSIS
        ['BRUGP', 'DTH1C', 'EX', 'ITCEE', ..., 'DTH1', 'PME', 'PMS', 'PMT']
        >>> len(lists["_PRE"])
        31

        >>> lists["_INTER"]         # doctest: +ELLIPSIS
        ['PMAB', 'PXAB', 'ULCP', 'SSH3P', ..., 'WCF_', 'ITEP', 'EXC', 'ITT']
        >>> len(lists["_INTER"])
        204

        >>> lists["_POST"]          # doctest: +ELLIPSIS
        ['IFU', 'SSHFF', 'PBBP', 'OCUF', ..., 'FLGR', 'FLF', 'DPUU', 'BENEF']
        >>> len(lists["_POST"])
        39
        """
        if nb_iterations <= 0:
            raise ValueError(f"'nb_iterations': Value must be positive. Got value {nb_iterations} instead.")
        
        if list_eqs is None:
            list_eqs = ''
        if not isinstance(list_eqs, str) and isinstance(list_eqs, Iterable) and \
            all(isinstance(item, str) for item in list_eqs):
            list_eqs = ';'.join(list_eqs)

        self.c_simulation.model_calculate_SCC(nb_iterations, pre_name.encode(), inter_name.encode(), 
                                              post_name.encode(), list_eqs.encode())

    def model_simulate_SCC(self, from_period: Union[str, Period], to_period: Union[str, Period], 
                           pre_name: str="_PRE", inter_name: str="_INTER", post_name: str="_POST"):
        """
        Simulates a model previously decomposed into Strongly Connex Components (SCC) and reordered.
        It is intended to be called after the method :meth:`Simulation.model_calculate_SCC`.

        To improve performance at simulation start-up, the simulation process has been divided into 2 stages. 
        The first only deals with model reordering (see method :meth:`Simulation.model_calculate_SCC`), 
        the second with simulation (see method :meth:`Simulation.model_simulate`).

        Parameters
        ----------
        from_period : str or Period
            The starting period for the simulation.
        to_period : str or Period
            The ending period for the simulation.
        pre_name : str, optional
            Name of the list representing the pre-recursive equations.
            Default to "_PRE".
        inter_name : str, optional
            Name of the list representing the interdependent equations. 
            Default to "_INTER".
        post_name : str, optional
            Name of the list representing the post-recursive equations.
            Default to "_POST".

        See Also
        --------
        Simulation.model_calculate_SCC
        Simulation.model_simulate

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations, identities, lists, scalars, variables 
        >>> from iode import Simulation
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
        >>> identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")
        >>> lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> simu = Simulation()

        Step 1 - Compute the Strongly Connex Components (SCC) decomposition

        >>> simu.model_calculate_SCC(10);
        >>> lists["_PRE"]           # doctest: +ELLIPSIS
        ['BRUGP', 'DTH1C', 'EX', 'ITCEE', ..., 'ZZF_', 'DTH1', 'PME', 'PMS', 'PMT']
        >>> len(lists["_PRE"])
        31
        >>> lists["_INTER"]         # doctest: +ELLIPSIS
        ['PMAB', 'PXAB', 'ULCP', 'SSH3P', ..., 'YSSG', 'WCF_', 'ITEP', 'EXC', 'ITT']
        >>> len(lists["_INTER"])
        204
        >>> lists["_POST"]          # doctest: +ELLIPSIS
        ['IFU', 'SSHFF', 'PBBP', 'OCUF', ..., 'GAP', 'FLGR', 'FLF', 'DPUU', 'BENEF']
        >>> len(lists["_POST"])
        39

        Step 2 - Simulate the model based on the 3 lists _PRE, _INTER and _POST

        >>> # exogeneous variable 
        >>> equations["UY"].lec
        'UY := NATY-NDOMY-NIY-NGY-(EFXY-EFMY)-NFY'
        >>> # reset values
        >>> variables["UY", "2000Y1:2015Y1"] = 0.0
        >>> variables["UY", "2000Y1:2015Y1"]    # doctest: +ELLIPSIS
        [0.0, 0.0, ..., 0.0, 0.0]
        >>> # endogenous variable
        >>> identities["XNATY"]
        Identity('grt NATY')
        >>> variables["XNATY", "2000Y1:2015Y1"]     # doctest: +ELLIPSIS
        [0.22, 0.699999988079071, 0.4, 0.7, ..., -0.20000000298023224, -0.20000000298023224]
        
        >>> simu.model_simulate_SCC("2000Y1", "2015Y1")

        >>> variables["UY", "2000Y1:2015Y1"]    # doctest: +ELLIPSIS
        [624.1822680369564, 645.0533712662168, 661.5778936891761, ..., 525.2940777398553]
        """
        if isinstance(from_period, Period):
            from_period = str(from_period)

        if isinstance(to_period, Period):
            to_period = str(to_period)

        self.c_simulation.model_simulate_SCC(from_period.encode(), to_period.encode(), 
                                             pre_name.encode(), inter_name.encode(), post_name.encode())
