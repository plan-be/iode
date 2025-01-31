import sys
from PySide6.QtWidgets import QApplication, QSplashScreen, QMessageBox
from PySide6.QtGui import QPixmap
from PySide6.QtCore import QLocale

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
        of the project the list of last opened files and load them.
        Also when False, the IODE application will save the list of the 
        opened files when closing.
        Default to False.
    depth : int, optional
        Stack depth where to look for Python variables. 
        Defaults to 0 (where this function was called).
    """

    app = QApplication(sys.argv)
    app.setOrganizationName(ORGANIZATION_NAME)
    app.setApplicationName("IODE")

    # force to use '.' as a decimal point
    loc = QLocale()
    loc.setNumberOptions(QLocale.c().numberOptions())
    QLocale.setDefault(loc)

    pixmap = QPixmap(":/images/iode_splash_screen.png")
    splash = QSplashScreen(pixmap)
    splash.show()
    app.processEvents()

    # imported from larray-editor code
    caller_frame = sys._getframe(depth + 1)
    global_vars = caller_frame.f_globals
    local_vars = caller_frame.f_locals
    vars_to_import = {k: global_vars[k] for k in sorted(global_vars.keys())}
    if local_vars is not global_vars:
        vars_to_import.update({k: local_vars[k] for k in sorted(local_vars.keys())})

    for key in vars_to_import:
        if key in _iode_databases_names:
            globals()[key] = vars_to_import[key]
    
    vars_to_import = {k: v for k, v in vars_to_import.items() if k not in globals().keys()}

    Context.set_called_from_python_script(called_from_python_script)
    main_window = MainWindow(None, project_dir, files_to_load, vars_to_import)
    gui_assign_super(main_window)
    main_window.show()
    splash.finish(main_window)
    app.exec()


def main():
    project_dir = Path('.').resolve()
    files_to_load = sys.argv[1:]
    open_application(project_dir, files_to_load)


if __name__ == "__main__":
    main()
