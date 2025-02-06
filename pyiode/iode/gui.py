from pathlib import Path
import inspect
from typing import Tuple


def view_workspace(depth: int=0):
    """
    Open the Graphical User Interface to view and edit the IODE databases.

    Parameters
    ----------
    depth : int, optional
        Stack depth where to look for variables. 
        Defaults to 0 (where this function was called).

    Examples
    --------
    >>> from iode import comments, equations, identities, lists, scalars, tables, variables
    >>> from iode import SAMPLE_DATA_DIR, view_workspace
    >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Loading .../fun.cmt
    317 objects loaded 
    >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")      # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Loading .../fun.eqs
    274 objects loaded
    >>> identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")    # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Loading .../fun.idt
    48 objects loaded
    >>> lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Loading .../fun.lst
    17 objects loaded
    >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")      # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Loading .../fun.scl
    161 objects loaded
    >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Loading .../fun.tbl
    46 objects loaded 
    >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")    # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Loading .../fun.var
    394 objects loaded 
    >>> # will open a viewer showing all IODE databases
    >>> view_workspace()                                # doctest: +SKIP
    """
    try:
        from iode_gui import open_application

        # Get the current call stack
        stack = inspect.stack()
        # The second frame in the stack is the caller of this function
        caller_frame = stack[1]
        # Extract the file path from the caller's frame
        caller_file_path = caller_frame.filename
        # Extract the directory path from the caller's file path
        project_dir = Path(caller_file_path).parent.resolve()

        open_application(project_dir, called_from_python_script=True, depth = depth + 1)
    except ImportError:
        raise Exception('view_workspace() is not available because the iode_gui package is not installed')
    