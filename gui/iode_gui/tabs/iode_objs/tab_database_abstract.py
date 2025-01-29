from PySide6.QtCore import QDir, Qt, Signal, Slot
from PySide6.QtWidgets import (QVBoxLayout, QHBoxLayout, QLineEdit, QSplitter,
                               QPushButton, QSpacerItem, QSizePolicy, QLabel)
from PySide6.QtGui import QShortcut, QKeySequence

from iode_gui.abstract_main_window import AbstractMainWindow
from iode_gui.tabs.tab_abstract import IodeAbstractWidget
from iode_gui.iode_objs.models.abstract_table_model import IodeAbstractTableModel
from iode_gui.iode_objs.models.table_model import (CommentsModel , EquationsModel , IdentitiesModel , 
                                          ListsModel , ScalarsModel , TablesModel , VariablesModel)
from iode_gui.iode_objs.views.abstract_table_view import IodeAbstractTableView
from iode_gui.iode_objs.views.table_view import (CommentsView , EquationsView , IdentitiesView , 
                                        ListsView , ScalarsView , TablesView , VariablesView)

from typing import List
from iode import (IodeType, IodeFileType, VarsMode, IODE_DATABASE_TYPE_NAMES, IODE_FILE_TYPES, 
                  comments, equations, identities, lists, scalars, tables, variables)
from iode.util import IODE_DEFAULT_DATABASE_FILENAME


class AbstractIodeObjectWidget(IodeAbstractWidget):
    """Abstract class for Iode object widgets."""

    PREFIX_UNSAVED_DATABASE = "Unsaved"
    tab_database_modified = Signal(IodeType, bool)

    def __init__(self, iode_type: IodeType, parent=None):
        """Initialize the widget with the given IodeType and parent."""
        super().__init__(IodeFileType(int(iode_type)), parent)

        iode_type_name = iode_type.name.lower()
        self.setObjectName(f"widget_{iode_type_name}")

        self.iode_type: IodeType = iode_type
        self.project_dir: str = QDir.homePath()

        # layout
        self.vertical_layout = QVBoxLayout(self)
        self.vertical_layout.setObjectName(f"vertical_layout_{iode_type_name}")
        self.vertical_layout.setContentsMargins(0, 0, 0, 0)

        # top horizontal layout
        self.top_horizontal_layout = QHBoxLayout()
        self.top_horizontal_layout.setObjectName(f"top_horizontal_layout_{iode_type_name}")

        # filter
        self.label_filter = QLabel("Names:")
        self.label_filter.setObjectName("label_names_filter")
        self.top_horizontal_layout.addWidget(self.label_filter, alignment=Qt.AlignmentFlag.AlignLeft)

        self.lineEdit_filter = QLineEdit()
        self.lineEdit_filter.setObjectName("lineEdit_filter")
        self.lineEdit_filter.setPlaceholderText("names pattern here")
        sizePolicyFilter = QSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Fixed)
        sizePolicyFilter.setHorizontalStretch(0)
        sizePolicyFilter.setVerticalStretch(0)
        self.lineEdit_filter.setSizePolicy(sizePolicyFilter)
        self.lineEdit_filter.setMinimumSize(200, 0)
        self.top_horizontal_layout.addWidget(self.lineEdit_filter, alignment=Qt.AlignmentFlag.AlignLeft)

        self.pushButton_filter = QPushButton("Filter")
        self.pushButton_filter.setObjectName("pushButton_filter")
        self.top_horizontal_layout.addWidget(self.pushButton_filter, alignment=Qt.AlignmentFlag.AlignLeft)

        # spacer
        horizontalSpacer = QSpacerItem(828, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)
        self.top_horizontal_layout.addSpacerItem(horizontalSpacer)

        # print button
        self.pushButton_print = QPushButton("Print")
        self.pushButton_print.setObjectName("pushButton_print")
        self.top_horizontal_layout.addWidget(self.pushButton_print, alignment=Qt.AlignmentFlag.AlignLeft)

        # add button
        self.pushButton_add = QPushButton("Add " + IODE_DATABASE_TYPE_NAMES[iode_type])
        self.pushButton_add.setObjectName("pushButton_add")
        sizePolicyAdd = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicyAdd.setHorizontalStretch(0)
        sizePolicyAdd.setVerticalStretch(0)
        self.pushButton_add.setSizePolicy(sizePolicyAdd)
        self.top_horizontal_layout.addWidget(self.pushButton_add, alignment=Qt.AlignmentFlag.AlignRight)

        self.vertical_layout.addLayout(self.top_horizontal_layout)

        # Prepare splitter
        self.splitter: QSplitter = QSplitter(parent)
        self.splitter.setObjectName("splitter")
        self.splitter.move(10, 43)
        self.splitter.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Expanding)
        self.splitter.setOrientation(Qt.Orientation.Horizontal)

        # Model + View
        self.database_view: IodeAbstractTableView = None
        self.database_view_2: IodeAbstractTableView = None
        self.database_model: IodeAbstractTableModel = None

        if self.iode_type == IodeType.COMMENTS:
            self.database_view = CommentsView(parent=self.splitter)
            self.database_view_2 = CommentsView(parent=self.splitter)
            self.database_model = CommentsModel(parent=self.database_view)
        elif self.iode_type == IodeType.EQUATIONS:
            self.database_view = EquationsView(parent=self.splitter)
            self.database_view_2 = EquationsView(parent=self.splitter)
            self.database_model = EquationsModel(parent=self.database_view)
        elif self.iode_type == IodeType.IDENTITIES:
            self.database_view = IdentitiesView(parent=self.splitter)
            self.database_view_2 = IdentitiesView(parent=self.splitter)
            self.database_model = IdentitiesModel(parent=self.database_view)
        elif self.iode_type == IodeType.LISTS:
            self.database_view = ListsView(parent=self.splitter)
            self.database_view_2 = ListsView(parent=self.splitter)
            self.database_model = ListsModel(parent=self.database_view)
        elif self.iode_type == IodeType.SCALARS:
            self.database_view = ScalarsView(parent=self.splitter)
            self.database_view_2 = ScalarsView(parent=self.splitter)
            self.database_model = ScalarsModel(precision=2, format='f', parent=self.database_view)
        elif self.iode_type == IodeType.TABLES:
            self.database_view = TablesView(parent=self.splitter)
            self.database_view_2 = TablesView(parent=self.splitter)
            self.database_model = TablesModel(parent=self.database_view)
        elif self.iode_type == IodeType.VARIABLES:
            self.database_view = VariablesView(parent=self.splitter)
            self.database_view_2 = VariablesView(parent=self.splitter)
            self.database_model = VariablesModel(precision=2, format='f', mode=VarsMode.LEVEL, 
                                                 parent=self.database_view)

        self.database_view.setObjectName(f"database_view_{iode_type_name}")
        self.database_view_2.setObjectName(f"database_view_2_{iode_type_name}")
        self.database_view.setModel(self.database_model)
        self.database_view_2.setModel(self.database_model)

        # IODE objects names filter
        self.database_view.set_filter_names(self.lineEdit_filter)
        self.database_view_2.set_filter_names(self.lineEdit_filter)

        # Shortcuts
        self.shortcut_print = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Key.Key_P), self)
        self.shortcut_add = QShortcut(QKeySequence(Qt.Key.Key_Insert), self)

        # NOTE: By default, shortcuts are defined at the main Window level.
        #       Thus, a shortcut of a (combination of) key(s) may override the expected behavior
        #       from another widget dealing with the same (combination of) key(s).
        #       'setContext(Qt::WidgetWithChildrenShortcut)' makes sure that the shortcut does
        #       not propagate to other widgets.
        self.shortcut_print.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.shortcut_add.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)

        # Signals - slots
        # NOTE: no need to connect lineEdit_filter and pushButton_filter to tableview_2 since
        #       both views share the same model
        self.lineEdit_filter.returnPressed.connect(self.database_view.filter_slot)
        self.pushButton_filter.clicked.connect(self.database_view.filter_slot)

        self.pushButton_print.clicked.connect(self.database_view.print)
        self.shortcut_print.activated.connect(self.database_view.print)

        self.pushButton_add.clicked.connect(self.database_view.new_obj)
        self.shortcut_add.activated.connect(self.database_view.new_obj)

        if hasattr(parent, 'show_objects_list'):
            self.database_view.show_objs_request.connect(parent.show_objects_list)
            self.database_view_2.show_objs_request.connect(parent.show_objects_list)

        # Update filter pattern when an object is removed
        self.database_model.object_removed.connect(self.database_view.object_removed)

        self.splitter.addWidget(self.database_view)
        self.splitter.addWidget(self.database_view_2)
        self.database_view_2.hide()

        # Insert table to layout
        # -1 -> span over all rows/columns
        self.vertical_layout.addWidget(self.splitter)

    def setup(self, main_window: AbstractMainWindow):
        self.database_view.setup(main_window)
        self.database_view_2.setup(main_window)

    def update(self):
        self.database_view.update()

    @property
    def is_unsaved_database(self) -> bool:
        """
        Return whether or not the tooltip is associated with a tab representing an unsaved KDB.

        :param str tooltip: Tooltip of the tab.
        :return: bool
        """
        return not self.filepath or self.filepath == IODE_DEFAULT_DATABASE_FILENAME

    # override method from IodeAbstractWidget
    @property
    def filepath(self) -> str:
        return self.database_model.filepath

    @filepath.setter
    def filepath(self, filepath: str):
        if self._check_new_filepath(filepath):
            if self.iode_type == IodeType.COMMENTS:
                comments.filename = filepath
            elif self.iode_type == IodeType.EQUATIONS:
                equations.filename = filepath
            elif self.iode_type == IodeType.IDENTITIES:
                identities.filename = filepath
            elif self.iode_type == IodeType.LISTS:
                lists.filename = filepath
            elif self.iode_type == IodeType.SCALARS:
                scalars.filename = filepath
            elif self.iode_type == IodeType.TABLES:
                tables.filename = filepath
            elif self.iode_type == IodeType.VARIABLES:
                variables.filename = filepath

            self.modified = False
            self.tab_database_modified.emit(self.iode_type, False)
            return True
        else:
            return False

    # override method from IodeAbstractWidget
    @property
    def tab_text(self) -> str:
        """
        Returns the text to be displayed on the tab.

        :return: The text to be displayed on the tab.
        """
        if self.is_unsaved_database:
            index = int(self.iode_type)
            ext = IODE_FILE_TYPES[index].extensions[0]
            # Note: the * is to tell that the content of the KDB has not been saved in file
            return self.tab_prefix[index] + IODE_DATABASE_TYPE_NAMES[index] + ext + " [0]*"
        else:
            return super().tab_text + " [" + str(self.database_model.nb_objects) + "]"

    # override method from IodeAbstractWidget
    @property
    def tooltip(self) -> str:
        if self.is_unsaved_database:
            index = int(self.iode_type)
            return self.PREFIX_UNSAVED_DATABASE + " " + IODE_DATABASE_TYPE_NAMES[index] + " Database [0]"
        else:
            return super().tooltip + " [" + str(self.database_model.nb_objects) + "]"

    def set_project_dir(self, projectDir: QDir):
        """
        Set the project directory.

        :param QDir projectDir: Project directory.
        """
        self.project_dir = projectDir
        self.load_settings()
        self.reset_model()

    def clear_database(self):
        """
        Clear the Iode database.
        """
        self.reset_filter()
        self.database_model.clear_database()
        self.database_model.reset_model()

    def filter(self, key: str=None, silent: bool=False):
        """
        Filter the database.

        :param bool silent: Whether to filter silently.
        """
        self.database_view.filter(key, silent)

    def reset_filter(self):
        """
        Reset the filter.
        """
        self.lineEdit_filter.setText("")
        self.database_view.filter()

    def _load(self, filepath: str, force_overwrite: bool) -> bool:
        return self.database_model.load(filepath, force_overwrite)

    def save(self) -> str:
        if self.is_unsaved_database:
            return self.save_as()
        else:
            filepath = self.database_model.save(self.project_dir)
            if filepath:
                self.set_modified(False)
            return filepath

    def _save_as(self) -> str:
        return self.database_model.save_as(self.project_dir)

    def get_selected_objects_names(self) -> List[str]:
        """
        Get the names of the selected objects.

        :return: list
        """
        object_list: List[str] = self.database_view.get_selected_objects_names()
        if self.database_view_2.isVisible():
            object_list += self.database_view_2.get_selected_objects_names()
        return object_list

    def open_add_dialog(self):
        """
        Open the add dialog.
        """
        self.database_view.new_obj()

    def open_edit_dialog(self):
        """
        Open the edit dialog.
        """
        self.database_view.edit_obj()

    def compute_hash(self, before: bool = False):
        """
        Compute the hash of the database.

        :param bool before: Whether to compute the hash before.
        """
        self.database_model.compute_hash(before)

    def preferred_height(self) -> int:
        """
        Get the preferred height of the database.

        :return: int
        """
        h = (self.vertical_layout.count() - 1) * 30
        return h + self.database_view.preferred_height()

    def reset_model(self):
        """
        Reset the model.
        """
        self.database_model.reset()

    def split(self, orientation):
        """
        Split the view.

        :param orientation: The split orientation.
        """
        if self.splitted:
            return
        if self.splitter.orientation() != orientation:
            self.splitter.setOrientation(orientation)
        self.database_view_2.show()
        self.splitted = True

    def undo_split(self):
        """Undo split the view."""
        if not self.splitted:
            return
        self.database_view_2.hide()
        self.splitted = False

    @Slot(bool)
    def set_modified(self, modified: bool):
        """
        Set the modified status of the database.
        """
        self.modified = modified
        if modified:
            # We need to recompute the subset database in the IodeTemplateTableModel class if the filter is active
            # -> executing a command line or an IODE report may have renamed, added or removed IODE objects and
            #    then made the subset database invalid (remember that the subset database in the
            #    IodeTemplateTableModel class is a shallow copy of a subset of a global IODE database)
            self.filter(silent=True)
            self.reset_model()

        self.tab_database_modified.emit(self.iode_type, modified)

    @Slot()
    def database_modified(self):
        """
        Slot to handle the database modification.
        """
        self.set_modified(True)