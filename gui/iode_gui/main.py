import os
import sys
import inspect
from PySide6.QtWidgets import QApplication, QSplashScreen, QMessageBox
from PySide6.QtGui import QPixmap

from typing import Union, List, Optional
from pathlib import Path

from iode_gui.utils import ORGANIZATION_NAME, Context
from iode_gui.main_window import MainWindow
from iode_gui.super import gui_assign_super


_iode_databases_names = {'comments', 'equations', 'identities', 'lists', 
                         'scalars', 'tables', 'variables'}


def open_application(project_dir: Union[str, Path]=None, files_to_load: List[Union[str, Path]]=None, 
                     called_from_python_script: bool=False, depth: int=0):
    """
    Open the IODE application.

    Parameters
    ----------
    project_dir : Union[str, Path], optional
        Path to the project directory.
        If not given, it is deduced from the list of files to load.
        Default to None.
    files_to_load : List[str], optional
        List of files to load at start. 
        Default to None.
    called_from_python_script : bool, optional
        If False, the IODE application will look into the settings file 
        of the project, check the list of last opened files and reload them.
        Also when False, the IODE application will save the list of the 
        opened files when closing.
        Default to False.
    depth : int, optional
        Stack depth where to look for Python variables. 
        Defaults to 0 (where this function was called).

    Warnings
    --------
    If both project_dir and files_to_load are set, an error is raised.
    """
    if project_dir is not None and files_to_load is not None:
        raise ValueError("project_dir and files_to_load cannot be both set")

    app = QApplication.instance()
    if app is None:
        app = QApplication(sys.argv)
        app.setOrganizationName(ORGANIZATION_NAME)
        app.setApplicationName("IODE")
        app.setStyle('Fusion')

    pixmap = QPixmap(":/images/iode_splash_screen.png")
    splash = QSplashScreen(pixmap)
    splash.show()
    app.processEvents()

    # code adapted from the larray-editor project (file api.py) 
    vars_to_import = None
    if called_from_python_script:
        # Get the current call stack
        stack = inspect.stack()
        # The second frame in the stack is the caller of this function
        caller_frame = stack[depth + 1].frame
        caller_global_vars = caller_frame.f_globals
        caller_global_keys = set(caller_global_vars.keys()) - set(caller_frame.f_builtins)
        caller_global_keys = {name for name in caller_global_keys if not name.startswith("__")}
        vars_to_import = {k: caller_global_vars[k] for k in caller_global_keys}
        
        caller_local_vars = caller_frame.f_locals
        caller_local_keys = set(caller_local_vars.keys()) - caller_global_keys
        for k in caller_local_keys:
            vars_to_import[k] = caller_local_vars[k]

        for key in vars_to_import:
            if key in _iode_databases_names:
                globals()[key] = vars_to_import[key]
    
        vars_to_import = {k: v for k, v in vars_to_import.items() if k not in globals().keys()}
        
        imported_keys = set(vars_to_import.keys())
        caller_local_keys = caller_local_keys.intersection(imported_keys)
        caller_global_keys = caller_global_keys.intersection(imported_keys)

    Context.set_called_from_python_script(called_from_python_script)
    main_window = MainWindow(None, project_dir, files_to_load, vars_to_import)
    gui_assign_super(main_window)
    main_window.show()
    splash.finish(main_window)
    exit_code = app.exec()

    # NOTE: pull modified or reassigned variables from the IPython kernel back to 
    #       the caller frame
    if called_from_python_script and exit_code == 0:
        for key in caller_local_keys:
            try:
                caller_frame.f_locals[key] = main_window.pull_data_from_kernel(key)
            except KeyError:
                pass
        
        for key in caller_global_keys - _iode_databases_names:
            try:
                caller_frame.f_globals[key] = main_window.pull_data_from_kernel(key)
            except KeyError:
                pass


def start():
    try:
        project_dir = None
        files_to_load = sys.argv[1:]
        called_from_python_script = False

        # from official documentation https://docs.python.org/3/library/sys.html#sys.__stdout__ :
        # Under some conditions stdin, stdout and stderr as well as the original values __stdin__, 
        # __stdout__ and __stderr__ can be None. It is usually the case for Windows GUI apps that 
        # aren't connected to a console and Python apps started with pythonw.
        # 
        # code below is from: https://github.com/scipy/scipy/issues/22096#issue-2743279073
        if sys.executable.endswith('pythonw.exe'):
            sys.stdout = open(os.devnull, "w")
            sys.stderr = open(os.path.join(os.getenv('TEMP'), 'stderr-' + os.path.basename(sys.argv[0])), 'w')

        open_application(project_dir, files_to_load, called_from_python_script)
    except Exception as e:
        # Show the error message in a message box
        QMessageBox.critical(None, "Error", str(e))
