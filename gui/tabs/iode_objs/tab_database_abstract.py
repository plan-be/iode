from PySide6.QtWidgets import (QVBoxLayout, QHBoxLayout, QLineEdit, 
                               QPushButton, QSpacerItem, QSizePolicy)
from PySide6.QtCore import QDir, Qt, Signal, Slot

from tabs.tab_abstract import IodeAbstractWidget

from iode import IodeTypes, IodeFileType, IODE_DATABASE_TYPE_NAMES, IODE_DEFAULT_DATABASE_FILENAME


class AbstractIodeObjectWidget(IodeAbstractWidget):
    """Abstract class for Iode object widgets."""

    tabDatabaseModified = Signal(IodeTypes, bool)

    def __init__(self, iode_type: IodeTypes, parent=None):
        """Initialize the widget with the given IodeTypes and parent."""
        super().__init__(IodeFileType(int(iode_type)), parent)
        self.setObjectName("widget_iode_obj")

        self.iode_type: IodeTypes = iode_type
        self.project_dir: str = QDir.homePath()

        # layout
        self.vLayout = QVBoxLayout(self)
        self.vLayout.setObjectName("vTabLayout")
        self.vLayout.setContentsMargins(0, 0, 0, 0)

        # top horizontal layout
        topHLayout = QHBoxLayout(self)

        # filter
        self.lineEdit_filter = QLineEdit()
        self.lineEdit_filter.setObjectName("lineEdit_filter")
        self.lineEdit_filter.setPlaceholderText("filter pattern here")
        sizePolicyFilter = QSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Fixed)
        sizePolicyFilter.setHorizontalStretch(0)
        sizePolicyFilter.setVerticalStretch(0)
        self.lineEdit_filter.setSizePolicy(sizePolicyFilter)
        self.lineEdit_filter.setMinimumSize(200, 0)
        topHLayout.addWidget(self.lineEdit_filter, alignment=Qt.AlignmentFlag.AlignLeft)

        self.pushButton_filter = QPushButton("Filter")
        self.pushButton_filter.setObjectName("pushButton_filter")
        topHLayout.addWidget(self.pushButton_filter, alignment=Qt.AlignmentFlag.AlignLeft)

        # spacer
        horizontalSpacer = QSpacerItem(828, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)
        topHLayout.addSpacerItem(horizontalSpacer)

        # print button
        self.pushButton_print = QPushButton("Print")
        self.pushButton_print.setObjectName("pushButton_print")
        topHLayout.addWidget(self.pushButton_print, alignment=Qt.AlignmentFlag.AlignLeft)

        # add button
        self.pushButton_add = QPushButton("Add " + IODE_DATABASE_TYPE_NAMES[iode_type])
        self.pushButton_add.setObjectName("pushButton_add")
        sizePolicyAdd = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicyAdd.setHorizontalStretch(0)
        sizePolicyAdd.setVerticalStretch(0)
        self.pushButton_add.setSizePolicy(sizePolicyAdd)
        topHLayout.addWidget(self.pushButton_add, alignment=Qt.AlignmentFlag.AlignRight)

        self.vLayout.addLayout(topHLayout)

    def is_unsaved_database(self) -> bool:
        """
        Return whether or not the tooltip is associated with a tab representing an unsaved KDB.

        :param str tooltip: Tooltip of the tab.
        :return: bool
        """
        return not self.filepath or self.filepath == IODE_DEFAULT_DATABASE_FILENAME

    def update_filepath(self, filepath: str) -> bool:
        """
        Update the filepath of the database.

        :param str filepath: New filepath.
        :return: bool
        """
        if self.check_new_filepath(filepath):
            self.set_database_filename(self.file_type, filepath)
            self.modified = False
            self.tabDatabaseModified.emit(self.iode_type, False)
            return True
        else:
            return False

    def reset(self):
        """
        Reset the database.
        """
        self.clear_database()

    def set_project_dir(self, projectDir: QDir):
        """
        Set the project directory.

        :param QDir projectDir: Project directory.
        """
        self.projectDir = projectDir
        self.load_settings()
        self.reset()

    def clear_database(self):
        """
        Clear the Iode database.
        """
        raise NotImplementedError()

    def filter(self, silent: bool = False):
        """
        Filter the database.

        :param bool silent: Whether to filter silently.
        """
        raise NotImplementedError()

    def reset_filter(self):
        """
        Reset the filter.
        """
        raise NotImplementedError()

    def get_selected_objects_names(self) -> list:
        """
        Get the names of the selected objects.

        :return: list
        """
        raise NotImplementedError()

    def open_add_dialog(self):
        """
        Open the add dialog.
        """
        raise NotImplementedError()

    def open_edit_dialog(self):
        """
        Open the edit dialog.
        """
        raise NotImplementedError()

    def compute_hash(self, before: bool = False):
        """
        Compute the hash of the database.

        :param bool before: Whether to compute the hash before.
        """
        raise NotImplementedError()

    def preferred_height(self) -> int:
        """
        Get the preferred height of the database.

        :return: int
        """
        raise NotImplementedError()

    def reset_model(self):
        """
        Reset the model.
        """
        raise NotImplementedError()

    @Slot(bool)
    def setModified(self, modified: bool):
        """
        Set the modified status of the database.
        """
        self.modified = modified
        if modified:
            # We need to recompute the subset database in the IodeTemplateTableModel class if the filter is active
            # -> executing a command line or an IODE report may have renamed, added or removed IODE objects and
            #    then made the subset database invalid (remember that the subset database in the
            #    IodeTemplateTableModel class is a shallow copy of a subset of a global IODE database)
            self.filter(True)
            self.reset_model()

        self.tabDatabaseModified.emit(self.iode_type, modified)

    @Slot()
    def database_modified(self):
        """
        Slot to handle the database modification.
        """
        self.set_modified(True)