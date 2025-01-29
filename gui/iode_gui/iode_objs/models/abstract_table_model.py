from PySide6.QtCore import (QAbstractTableModel, QModelIndex, Qt, QDir, QFileInfo, QSize, 
                            Signal, Slot)
from PySide6.QtWidgets import (QMessageBox, QDialog, QVBoxLayout, QLabel, QFileDialog, 
                               QDialogButtonBox, QSizePolicy)

from typing import List, Union, Tuple, Any
from pathlib import Path
from iode import (IodeType, IodeFileType, FileType, IODE_FILE_TYPES, Equation, Identity, 
                  comments, equations, identities, lists, scalars, tables, variables) 
from iode.util import IODE_DEFAULT_DATABASE_FILENAME, check_filepath

# NOTE FOR THE DEVELOPERS:
#      Because of how Qt is implemented, it is NOT possible for a template class
#      to define/override signals and slots (using Q_OBJECT)
#      ( see https://doc.qt.io/qt-5/moc.html#limitations )
#      
#      Read-only model: https://doc.qt.io/qt-6/model-view-programming.html#read-only-access
#      Editable model:  https://doc.qt.io/qt-6/model-view-programming.html#editable-items
#      Resizable model: https://doc.qt.io/qt-6/model-view-programming.html#resizable-models


class IodeAbstractTableModel(QAbstractTableModel):
    """
    This is the base class for all "models" used to represents IODE objects.
    """

    database_modified = Signal()
    object_removed = Signal(str)

    def __init__(self, column_names: List[str], iode_type: IodeType, database=None, parent=None):
        """
        Initializes the IodeAbstractTableModel with the given column names.

        :param column_names: A list of column names.
        :param parent: The parent object.
        """
        super().__init__(parent)
        self.iode_type: IodeType = iode_type
        self.column_names: List[str] = column_names
        if database is not None:
            self._database = database
            if self._database.iode_type != self.iode_type:
                QMessageBox.warning(None, "Error", "The database type does not match the model type.")
                self._database = None
        elif self.iode_type == IodeType.COMMENTS:
            self._database = comments
        elif self.iode_type == IodeType.EQUATIONS:
            self._database = equations
        elif self.iode_type == IodeType.IDENTITIES:
            self._database = identities
        elif self.iode_type == IodeType.LISTS:
            self._database = lists
        elif self.iode_type == IodeType.SCALARS:
            self._database = scalars
        elif self.iode_type == IodeType.TABLES:
            self._database = tables
        elif self.iode_type == IodeType.VARIABLES:
            self._database = variables
        else:
            QMessageBox.warning(None, "Error", f"Unknown IODE types {self.iode_type}")
            self._database = None
        self._displayed_database = self._database

        self.alignment = Qt.AlignmentFlag.AlignLeft
        self.hash_before: int = 0
        self.hash_after: int = 0

    @property
    def displayed_database(self) -> Any:
        """
        Returns the currently displayed database or subset.
        """
        return self._displayed_database

    # override QAbstractTableModel method
    def rowCount(self, parent: QModelIndex=QModelIndex()):
        """
        Returns the number of rows in the displayed database.

        :param parent: The parent model index.
        :return: The number of rows in the displayed database.
        """
        return len(self._displayed_database)

    # override QAbstractTableModel method
    def columnCount(self, parent: QModelIndex=QModelIndex()) -> int:
        """
        Returns the number of columns in the model.

        :param parent: The parent model index.
        :return: The number of columns.
        """
        return len(self.column_names)

    @property
    def nb_objects(self) -> int:
        """
        Returns the number of IODE objects in the database.
        """
        return len(self._database)

    @property
    def filepath(self) -> str:
        """
        Returns the file path associated with the database.
        """
        return self._database.filename

    @property
    def filter_active(self) -> bool:
        """
        Returns whether a filter is currently active.
        """
        return self._displayed_database.is_global_workspace

    def reset_model(self):
        """
        Resets the model.
        """
        self.beginResetModel()
        self.endResetModel()

    # override QAbstractTableModel method
    def flags(self, index: QModelIndex):
        """
        Returns the item flags for the given index.

        :param index: The model index.
        :return: The item flags for the given index.
        """
        if not index.isValid():
            return Qt.ItemFlag.ItemIsEnabled
        return super().flags(index) | Qt.ItemFlag.ItemIsEditable

    # override QAbstractTableModel method
    def headerData(self, section: int, orientation: Qt.Orientation, role: int=Qt.ItemDataRole.DisplayRole) -> Any:
        """
        Returns the data for the given role and section in the header with the specified orientation.

        :param section: The section number.
        :param orientation: The header orientation.
        :param role: The data role.
        :return: The header data for the given role and section.
        """
        if role != Qt.ItemDataRole.DisplayRole:
            return None

        if orientation == Qt.Orientation.Horizontal:
            return self.column_names[section]
        else:
            try:
                return self._displayed_database.get_name(section)
            except Exception as e:
                return " "

    # override QAbstractTableModel method
    def setHeaderData(self, section: int, orientation: Qt.Orientation, value: Any, role: int=Qt.ItemDataRole.EditRole) -> bool:
        """
        Sets the header data for the given role and section in the header with the specified orientation.

        :param section: The section number.
        :param orientation: The header orientation.
        :param value: The new header data.
        :param role: The data role.
        :return: True if the header data was set successfully, False otherwise.
        """
        if orientation == Qt.Orientation.Horizontal:
            return False

        if role != Qt.ItemDataRole.EditRole:
            return False

        if section >= self.rowCount():
            return False

        try:
            old_name: str = self.headerData(section, orientation)
            new_name: str = value
            self._displayed_database.rename(old_name, new_name)
            self.headerDataChanged.emit(orientation, section, section)
            return True
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))
            return False

    # override QAbstractTableModel method
    def data(self, index: QModelIndex, role: int=Qt.ItemDataRole.DisplayRole) -> Any:
        """
        Returns the data for the given role and model index.

        :param index: The model index.
        :param role: The data role.
        :return: The data for the given role and model index.
        """
        if not index.isValid():
            return None

        if role == Qt.ItemDataRole.TextAlignmentRole:
            return int(self.alignment)

        if role == Qt.ItemDataRole.DisplayRole or role == Qt.ItemDataRole.EditRole:
            return self.get_value(index.row(), index.column())

        return None

    # override QAbstractTableModel method
    def setData(self, index: QModelIndex, value: Any, role: int) -> bool:
        """
        Sets the data for the given role and model index.

        :param index: The model index.
        :param value: The new data.
        :param role: The data role.
        :return: True if the data was set successfully, False otherwise.
        """
        if role != Qt.ItemDataRole.EditRole:
            return False

        if not index.isValid():
            return False 

        success = self.set_value(index.row(), index.column(), value)
        if success:
            self.dataChanged.emit(index, index, [role])
        return success
        
    # override QAbstractTableModel method
    def removeRows(self, position: int, rows: int, index: QModelIndex=QModelIndex()) -> bool:
        """
        Removes the given number of rows starting at the given position.

        :param position: The starting position of the rows to remove.
        :param rows: The number of rows to remove.
        :param index: The parent model index.
        :return: True if the rows were removed successfully, False otherwise.
        """
        self.beginRemoveRows(QModelIndex(), position, position + rows - 1)

        try:
            for row in range(position, position + rows):
                name: str = self.headerData(row, Qt.Orientation.Vertical)
                del self._displayed_database[name]
                self.object_removed.emit(name)
                self.endRemoveRows()
                return True
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))
            self.endRemoveRows()
            return False

    def clear_database(self):
        """
        Clears the database and resets the displayed database to the full database.
        """
        if self._database is not None:
            self._database.clear()
        self._displayed_database = self._database

    def load(self, filepath: str, force_overwrite: bool):
        """
        Loads the database from the given file path.

        :param filepath: The file path to load the database from.
        :param force_overwrite: If True, the database is overwritten if it already exists.
        :return: True if the database was loaded successfully, False otherwise.
        """
        iode_type: IodeType = self._database.iode_type
        iode_file_type: IodeFileType = IodeFileType(int(iode_type))

        try:
            # NOTE: check_filepath() converts to absolute path
            filepath = check_filepath(filepath, iode_file_type, file_must_exist=True)

            if not force_overwrite and len(self._database) > 0:
                answer = QMessageBox.warning(None, "WARNING", f"There are {iode_type.name} already loaded. "
                                             f"Would like to override {iode_type.name} ?",
                                             QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No, 
                                             QMessageBox.StandardButton.Yes)
                if answer == QMessageBox.StandardButton.No:
                    return False

            # load Iode file in the global database
            self._database.load(filepath)

            # 'displayed_database' now points to the newly loaded 'database'
            self._displayed_database = self._database

            # update the view
            self.reset_model()

            return True
        except Exception as e:
            QMessageBox.critical(None, "ERROR", str(e))
            return False

    def save(self, project_dir: QDir, filepath: str=None) -> str:
        """
        Saves the database to the given file path.

        :param project_dir: The project directory.
        :param filepath: The file path to save the database to. If empty, the current file path is used.
        :return: The file path of the saved database.
        """
        # NOTE: we don't simply use 'database' since it may point to 'database_subset'
        if not len(self._database):
            return ""

        iode_type: IodeType = self._database.iode_type
        iode_file_type: IodeFileType = IodeFileType(int(iode_type))

        # if not provided as argument, get path to the file associated with KDB of objects of type iode_type
        if filepath is None:
            filepath = self._database.filename

        # if KDB not linked to any file, ask the user to give/create a file to save in.
        # Otherwise, check the filepath
        if not filepath or filepath == IODE_DEFAULT_DATABASE_FILENAME:
            file_type: FileType = IODE_FILE_TYPES[int(iode_type)]
            # open a box dialog
            filepath, _ = QFileDialog.getSaveFileName(None, "Save File", project_dir.path(), 
                                                      f"{file_type.name} Files (*.{file_type.extensions})")
            # filepath is empty if the user clicked on the Discard button
            if not filepath:
                return None

        # NOTE: check_filepath() converts to absolute path
        filepath = check_filepath(filepath, iode_file_type, file_must_exist=False)

        try:
            self._database.save(filepath)
            self._database.filename = filepath
            return filepath
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))
            return None

    def save_as(self, project_dir: QDir) -> str:
        """
        Saves the database to a new file path.

        :param project_dir: The project directory.
        :return: The file path of the saved database.
        """
        # If the database is empty, return an empty string
        if not len(self._database):
            return None

        # Ask the user for a new filepath
        filepath = self._ask_filepath(project_dir)

        # Call the save method
        filepath = self.save(project_dir, filepath)

        # Update the IODE database filename
        if filepath:
            self._database.filename = filepath

        return filepath

    @Slot()
    @Slot(bool)
    def compute_hash(self, before=False):
        """
        Computes the hash value of the database.

        :param before: If True, the hash value is computed before any modifications are made.
        """
        # NOTE: computing the hash value of 'displayed_database' makes no sense
        #       if it represents a subset.
        #       So we can't simply pass the 'displayed_database' class member
        #       to kdb_hasher because 'displayed_database' may point to
        #       'database_subset' if filtering is activated.
        if self._database is None:
            return
        
        if before:
            self.hash_before = hash(self._database)
        else:
            self.hash_after = hash(self._database)
            if self.hash_after != self.hash_before:
                self.database_modified.emit()

    def get_value(self, row: int, col: int) -> str:
        """
        Returns the data for the given row and column.

        :param row: The row number.
        :param col: The column number.
        :return: The data for the given row and column.
        """
        raise NotImplementedError()

    def set_value(self, row: int, column: int, value: Any) -> bool:
        """
        Sets the value for the given row and column.

        :param row: The row number.
        :param column: The column number.
        :param value: The new value.
        :return: True if the value was set successfully, False otherwise.
        """
        raise NotImplementedError()

    def _ask_filepath(self, project_dir: QDir) -> str:
        """
        This function opens a dialog to ask the user for a filepath.

        :param project_dir: The directory of the project.
        :return: The filepath provided by the user.
        """
        filepath = self._ask_filepath_dialog(project_dir)

        if not filepath:
            return None

        file_info = QFileInfo(filepath)

        if file_info.exists():
            answer = QMessageBox.warning(None, "WARNING",
                                         f"The file {file_info.fileName()} already exists. Override it?",
                                         QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No | QMessageBox.StandardButton.Discard,
                                         QMessageBox.StandardButton.Yes)

            if answer == QMessageBox.StandardButton.No:
                return self._ask_filepath(project_dir)

            if answer == QMessageBox.StandardButton.Discard:
                filepath = None

        return filepath

    def _ask_filepath_dialog(self, project_dir: QDir) -> str:
        """
        This function creates and executes a dialog to ask the user for a filepath.

        :param project_dir: The directory of the project.
        :return: The filepath provided by the user.
        """
        iode_type: IodeType = self._database.iode_type
        str_iode_type = str[iode_type]
        file_type: FileType = IODE_FILE_TYPES[iode_type]

        default_filename = IODE_DEFAULT_DATABASE_FILENAME + file_type.extensions[0]
        default_filepath = project_dir.absoluteFilePath(default_filename)

        dialog = QDialog()
        dialog.setObjectName("ask_save_to_dialog")
        dialog.setMinimumWidth(380)
        dialog.setMinimumHeight(100)
        dialog.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Expanding)
        dialog.setWindowTitle(f"Where to save {str_iode_type} ?")

        layout = QVBoxLayout(dialog)
        layout.setObjectName("layout")

        label = QLabel(f"Please provide the path to the file where {str_iode_type} will be stored")
        layout.addWidget(label, 0, Qt.AlignmentFlag.AlignLeft)

        file_dialog = QFileDialog(dialog, "Select File")
        file_dialog.setFileMode(QFileDialog.FileMode.AnyFile)
        file_dialog.setDirectory(project_dir.absolutePath())
        file_dialog.selectFile(default_filepath)
        layout.addWidget(file_dialog, 0, Qt.AlignmentFlag.AlignLeft)

        button_box = QDialogButtonBox(QDialogButtonBox.StandardButton.Ok | QDialogButtonBox.StandardButton.Cancel)
        button_box.accepted.connect(dialog.accept)
        button_box.rejected.connect(dialog.reject)
        layout.addWidget(button_box, 0, Qt.AlignmentFlag.AlignCenter)

        if dialog.exec() == QDialog.DialogCode.Accepted:
            return file_dialog.selectedFiles()[0]

        return None

    @Slot()
    def reset(self):
        """
        Resets the model.
        """
        self.reset_model()

    @Slot(str)
    @Slot(str, bool)
    @Slot(tuple, bool)
    def filter(self, key: Union[str, Tuple[str, str]], silent: bool=False):
        """
        Filters the database based on the given pattern.

        :param key: The filter pattern ( tuple(names, periods) for Variables ).
        :param silent: If the key is invalid and if True, the displayed database 
                       is set to the global IODE database without displaying a warning message.
        """
        if not key:
            self._displayed_database = self._database
        else:
            try:
                self._displayed_database = self._database[key]
            except Exception as e:
                self._displayed_database = self._database
                if not silent:
                    QMessageBox.warning(None, "WARNING", str(e))

    @Slot(str, IodeType)
    def get_same_name_obj_or_objs_from_CLEC(self, name: str, other_type: IodeType) -> List[str]:
        """
        Returns the list of scalars or variables listed in the CLEC structure or 
        the object of the same name.

        :param name: The name to search for.
        :param other_type: The type of IODE objects to search for.
        """
        if not len(self._database):
            return []

        this_type: IodeType = self._database.iode_type

        if this_type == other_type:
            return [name]

        if other_type == IodeType.COMMENTS and name in comments:
            return [name]
        elif other_type == IodeType.EQUATIONS and name in equations:
            return [name]
        elif other_type == IodeType.IDENTITIES and name in identities:
            return [name]
        elif other_type == IodeType.LISTS and name in lists:
            return [name]
        elif other_type == IodeType.SCALARS:
            obj_names = [name] if name in scalars else []
            if this_type == IodeType.EQUATIONS:
                eq: Equation = equations[name]
                obj_names += eq.coefficients
            elif this_type == IodeType.IDENTITIES:
                idt: Identity = identities[name]
                obj_names += idt.coefficients
            return obj_names
        elif other_type == IodeType.TABLES and name in tables:
            return [name]
        elif other_type == IodeType.VARIABLES:
            obj_names = [name] if name in variables else []
            if this_type == IodeType.EQUATIONS:
                eq: Equation = equations[name]
                obj_names += eq.variables
            elif this_type == IodeType.IDENTITIES:
                idt: Identity = identities[name]
                obj_names += idt.variables
            return obj_names
        else:
            return []

    @Slot(str, IodeType)
    def get_related_objs(self, name: str, other_type: IodeType) -> List[str]:
        """
        Returns a list of all related objects of type other_type.

        :param name: The name of the object to search for related objects.
        :param other_type: The type of IODE objects to search for.
        """
        if not len(self._database):
            return []

        if other_type == IodeType.COMMENTS:
            return comments.search(name)
        elif other_type == IodeType.EQUATIONS:
            return equations.search(name)
        elif other_type == IodeType.IDENTITIES:
            return identities.search(name)
        elif other_type == IodeType.LISTS:
            return lists.search(name)
        elif other_type == IodeType.SCALARS:
            return scalars.search(name)
        elif other_type == IodeType.TABLES:
            return tables.search(name)
        elif other_type == IodeType.VARIABLES:
            return variables.search(name)
