from qtpy.QtCore import QDir
from qtpy.QtWidgets import QMessageBox, QFileDialog

from pathlib import Path
from typing import Union
from iode import IodeFileType, FileType, IODE_FILE_TYPES
from iode.util import check_filepath, IODE_DEFAULT_DATABASE_FILENAME


def ask_filepath(iode_file_type: IodeFileType, project_dir: Union[QDir, str], filter: str=None, 
                 overwrite: bool=False) -> str:
    """
    This function opens a dialog to ask the user for a filepath.
    """
    if iode_file_type == IodeFileType.DIRECTORY:
        QMessageBox.warning(None, "WARNING", "Wrong file type: DIRECTORY")
        return None
    
    if not project_dir:
        project_dir = QDir.home()
    
    file_type: FileType = IODE_FILE_TYPES[int(iode_file_type)]

    if isinstance(project_dir, QDir):
        filepath = project_dir.path()
    filepath = Path(filepath)
    if iode_file_type <= IodeFileType.FILE_VARIABLES:
        filepath = filepath / f"{IODE_DEFAULT_DATABASE_FILENAME}{file_type.extensions[0]}"
    filepath = str(filepath.resolve())

    # set the filter for the file dialog
    if not filter:
        filter = f"{file_type.name} "
        if file_type.extensions:
            filter += f"(*{' *'.join(file_type.extensions)})"
        else:
            filter += "(*.*)"

    # open a box dialog
    filepath, _ = QFileDialog.getSaveFileName(None, "Save File", filepath, filter)
    # filepath is empty if the user clicked on the Discard button
    if not filepath:
        return None
    
    p_filepath = Path(filepath)
    if not overwrite and p_filepath.exists():
        answer = QMessageBox.question(None, "WARNING", f"The file {p_filepath.name} already exists. Override it?",
                                      QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No)
        if answer == QMessageBox.StandardButton.No:
            return ask_filepath(iode_file_type, project_dir, filter, overwrite=False)

    try:
        # NOTE: check_filepath() converts to absolute path
        filepath = check_filepath(filepath, iode_file_type, file_must_exist=False)
        return filepath
    except Exception as e:
        QMessageBox.warning(None, "WARNING", str(e))
        return None
