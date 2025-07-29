from PySide6.QtCore import (Qt, QDir, QFileInfo, QPoint, QCoreApplication, QFileSystemWatcher, 
                          QUrl, QProcess, Slot, Signal)
from PySide6.QtWidgets import (QWidget, QTabWidget, QDialog, QMessageBox, QProgressDialog, 
                             QTabBar, QGridLayout, QMenu, QApplication)
from PySide6.QtGui import QAction, QKeySequence, QShortcut, QDesktopServices

import sys
import shutil
from pathlib import Path
from typing import List, Union, Optional

from iode_gui.settings import get_settings
from iode_gui.abstract_main_window import AbstractMainWindow
from iode_gui.tabs.tab_abstract import IodeAbstractWidget
from iode_gui.tabs.iode_objs.tab_database_abstract import AbstractIodeObjectWidget
from iode_gui.tabs.iode_objs.tab_database import (CommentsWidget, EquationsWidget, IdentitiesWidget, 
                                         ListsWidget, ScalarsWidget, TablesWidget, VariablesWidget)
from iode_gui.tabs.tab_text import TextWidget
from iode_gui.tabs.tab_report import ReportWidget
from iode_gui.utils import Context, SHOW_IN_TEXT_TAB_EXTENSIONS_LIST

from iode import IodeType, IodeFileType, IODE_FILE_TYPES, IODE_DATABASE_TYPE_NAMES
from iode.util import get_iode_file_type


class IodeTabWidget(QTabWidget):
    """
    The present class represents the widget displaying tabs.
    Tabs content can represent:

    - an IODE database,
    - an IODE report,
    - a text file.

    Implemented features:

    - Tab text = prefix (CMT, EQS, ..., REP, TXT) + filename of the loaded file (if any).
      If the tab represents an IODE database not (yet) linked to any file, 'filename' is empty.
    - The tab text includes an * if modifications have been made but not saved yet.
    - Tab tooltip = absolute path to the loaded file (if any) or "Unsaved <IODE type> Database".
    - Only one tab can be open per IODE objects type (Comment, Equation, ..., Variable).
    - When switching from another project or quit the GUI, the absolute paths to all loaded files
      (= tooltips) are saved into a project settings file (with the .ini extension).
    - When switching to a previous project or starting the GUI, all previously open tabs are restored
      (by reading list of files in the project settings file .ini).
    - Users are allowed to close tabs if they represent a report or a text files (i.e. not an IODE objects type).
    - Users are allowed to move tabs.
    - Open files with corresponding application (.xlsx with Excel, ...)
    - Reload file content if modified by another program.
    -> CTRL + S on a tab saves its content (*)
    -> CTRL + W on a tab closes it if the tab does not represent an IODE database
    -> SHIFT + ALT + C copies the absolute filepath
    -> SHIFT + ALT + R reveals file in OS file explorer
    -> CTRL + SHIFT + S saves all tabs content (*)
    -> CTRL + D on a IODE database tab clears the corresponding database
    -> CTRL + SHIFT + D clears the whole workspace (*)
    -> ALT + [C|E|I|L|S|T|V] open the [Comments | Equations | Identities | Lists | Scalars | Tables | Variables] tab
    -> CTRL + [SHIFT] + TAB shift to the next [previous] tab

    (*) defined in main_window.ui
    """

    file_content_modified = Signal(str, bool)
    subset_objects_dialog_requested = Signal(QDialog)

    def __init__(self, parent=None):
        super().__init__(parent)

        self.overwrite_all = False 
        self.discard_all = False 
        self.index_context_menu = -1
        self.files_list: List[str] = []

        # Set name
        self.setObjectName("tab_widget_IODE_objs")

        # Set policy
        sizePolicy = self.sizePolicy()
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.sizePolicy().hasHeightForWidth())
        self.setSizePolicy(sizePolicy)

        # Initialize the tabs associated with the Iode databases
        self.tab_comments: CommentsWidget = CommentsWidget(self)
        self.tab_equations: EquationsWidget = EquationsWidget(self)
        self.tab_identities: IdentitiesWidget = IdentitiesWidget(self)
        self.tab_lists: ListsWidget = ListsWidget(self)
        self.tab_scalars: ScalarsWidget = ScalarsWidget(self)
        self.tab_tables: TablesWidget = TablesWidget(self)
        self.tab_variables: VariablesWidget = VariablesWidget(self)

        # Prepare shortcuts
        # NOTE: By default, shortcuts are defined at the main Window level.
        #       Thus, a shortcut of a (combination of) key(s) may override the expected behavior
        #       from another widget dealing with the same (combination of) key(s).
        #       'setContext(Qt.WidgetWithChildrenShortcut)' makes sure that the shortcut does
        #       not propagate to other widgets.
        self.filepath_shortcut = QShortcut(QKeySequence(Qt.Modifier.SHIFT | Qt.Modifier.ALT | Qt.Key.Key_C), self)
        self.filepath_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.filepath_shortcut.activated.connect(self.absolute_path)

        self.reveal_explorer_shortcut = QShortcut(QKeySequence(Qt.Modifier.SHIFT | Qt.Modifier.ALT | Qt.Key.Key_R), self)
        self.reveal_explorer_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.reveal_explorer_shortcut.activated.connect(self.reveal_in_folder)

        self.next_tab_shortcut = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Key.Key_Tab), self)
        self.next_tab_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.next_tab_shortcut.activated.connect(self.show_next_tab)
        
        self.previous_tab_shortcut = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Modifier.SHIFT | Qt.Key.Key_Tab), self)
        self.previous_tab_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.previous_tab_shortcut.activated.connect(self.show_previous_tab)
        
        self.clear_shortcut = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Key.Key_D), self)
        self.clear_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.clear_shortcut.activated.connect(self.clear_tab)
        
        self.close_shortcut = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Key.Key_W), self)
        self.close_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.close_shortcut.activated.connect(self.close_tab)
        
        # NOTE: set in the setup() method
        self.save_key_sequence: QKeySequence = None

        # ---- file system watcher ----
        self.file_system_watcher = QFileSystemWatcher(self)

        # ---- QAction to be (de)activated ----
        self.action_close: QAction = None
        self.action_clear: QAction = None
        self.action_split_h: QAction = None
        self.action_split_v: QAction = None
        self.action_undo_split: QAction = None

        # ---- signals to slots  ----
        self.currentChanged.connect(self.show_tab)
        self.tabCloseRequested.connect(self.remove_tab)

        self.file_system_watcher.fileChanged.connect(self.reload_file)

        self.tab_comments.tab_database_modified.connect(self.tab_database_modified)
        self.tab_equations.tab_database_modified.connect(self.tab_database_modified)
        self.tab_identities.tab_database_modified.connect(self.tab_database_modified)
        self.tab_lists.tab_database_modified.connect(self.tab_database_modified)
        self.tab_scalars.tab_database_modified.connect(self.tab_database_modified)
        self.tab_tables.tab_database_modified.connect(self.tab_database_modified)
        self.tab_variables.tab_database_modified.connect(self.tab_database_modified)

        self.tabs_database: List[AbstractIodeObjectWidget] = [
            self.tab_comments, self.tab_equations, self.tab_identities, self.tab_lists, 
            self.tab_scalars, self.tab_tables, self.tab_variables]

        # rebuild the list of files (tabs) every time a tab is moved
        self.tabBar().tabMoved.connect(self._build_files_list)

        # popups a context menu when the user right clicks on a tab
        self.tabBar().setContextMenuPolicy(Qt.ContextMenuPolicy.CustomContextMenu)
        self.tabBar().customContextMenuRequested.connect(self.onCustomContextMenu)

        # setup context menu
        self._setup_context_menu()

        self.load_settings()

    @Slot()
    def load_settings(self):
        """
        Load settings:
        - Reopen all tabs (files) that were open.
        - Display the tab that was displayed the last time the user quit the IODE gui.
        """
        if Context.called_from_python_script:
            self.files_list.clear()
            self._build_files_list()
            for tab in self.tabs_database:
                tab.update()
            self.show_tab(0)
            return
        
        # reset IODE objects tabs
        for tab in self.tabs_database:
            tab.clear_database()

        # reset list of open files (tabs)
        self.files_list.clear()

        # extract settings
        project_settings = get_settings()
        if not project_settings:
            return
        
        # end all groups to be sure we are at the top level
        while project_settings.group():
            project_settings.endGroup()

        # get project name (which is the name of the root directory of the project)
        project_name = QFileInfo(self.project_dir_path).fileName()

        # ---- LOAD SETTINGS ----
        project_settings.beginGroup("PROJECT")

        try:
            index = -1
            size = project_settings.beginReadArray("tabs")
            if size == 0:
                self.show_tab(0)
            else:
                progress = QProgressDialog("", "", 0, size)
                progress.setWindowModality(Qt.WindowModality.WindowModal)
                progress.setWindowTitle("Loading project " + project_name)
                progress.setCancelButton(None)

                filename = ""
                # reopen all tabs (files) that were open the last time the user quit the IODE gui
                for i in range(size):
                    project_settings.setArrayIndex(i)

                    filepath: str = project_settings.value("filepath", type=str)
                    file_type = IodeFileType(project_settings.value("file_type", type=int))
                    forced_as_text = project_settings.value("forced_as_text", type=bool)
                    splitted = project_settings.value("splitted", type=bool)
                    split_orientation = Qt.Orientation(project_settings.value("split_orientation", type=int))

                    try:
                        # unsaved database
                        if not filepath:
                            index = self.update_object_tab(IodeType(int(file_type)))
                            if index != i:
                                self.tabBar().moveTab(index, i)
                        else:
                            filename = QFileInfo(filepath).fileName()

                            progress.setLabelText("Loading file " + filename + "...")
                            progress.setValue(i)
                            QCoreApplication.processEvents()

                            index = self.load_file(filepath, False, True, i, forced_as_text)
                    except Exception as e:
                        QMessageBox.warning(None, "WARNING", f"Cannot load file {filename}.\n{str(e)}")
                        index = -1

                    # split tab if necessary
                    if splitted and index >= 0:
                        self._split_tab(index, split_orientation)

                    if index < 0:
                        continue

                    # split tab if necessary
                    if splitted:
                        self._split_tab(index, split_orientation)

                    tab_widget: IodeAbstractWidget = self.widget(index)
                    if tab_widget:
                        tab_widget.load_settings()

                progress.setValue(size)

            project_settings.endArray()

            self._build_files_list()

            # get the position of the tab displayed the last time the user quit the IODE GUI
            index = project_settings.value("index_last_open_tab", -1, type=int)

        except Exception as e:
            QMessageBox.warning(None, "WARNING", f"Could not reload last open tabs.\n{str(e)}")
            index = -1

        project_settings.endGroup()

        # display the tab that was displayed the last time the user quit the IODE GUI
        self.show_tab(index)

    @Slot()
    def save_settings(self):
        """
        Dump in settings:
        - The list of open tabs.
        - The index of the currently displayed tab.

        Note: The list of open tabs is actually the list of the tooltip 
        associated with each tab. The tooltip is either the path to the 
        corresponding file or a default string in case of unsaved IODE database.
        """
        project_settings = get_settings()
        if not project_settings:
            return

        # end all groups to be sure we are at the top level
        while project_settings.group():
            project_settings.endGroup()

        # ---- SAVE SETTINGS ----

        project_settings.beginGroup("PROJECT")

        try:
            # clear the "tabs" entry
            project_settings.remove("tabs")

            # save the list of open tabs
            project_settings.beginWriteArray("tabs")
            for i in range(self.count()):
                project_settings.setArrayIndex(i)
                tab_widget: IodeAbstractWidget = self.widget(i)

                file_type: IodeFileType = tab_widget.file_type
                if file_type <= IodeFileType.FILE_VARIABLES:
                    tab_database_widget: AbstractIodeObjectWidget = tab_widget 
                    filepath = "" if tab_database_widget.is_unsaved_database else tab_database_widget.filepath
                else:
                    filepath = tab_widget.filepath

                project_settings.setValue("filepath", filepath)
                project_settings.setValue("file_type", int(file_type))
                project_settings.setValue("forced_as_text", tab_widget.forced_as_text)
                project_settings.setValue("splitted", tab_widget.splitted)
                project_settings.setValue("split_orientation", tab_widget.split_orientation.value)

                if tab_widget:
                    tab_widget.save_settings()

            project_settings.endArray()

            # save index of the currently displayed tab
            index = self.currentIndex()
            project_settings.setValue("index_last_open_tab", index)
        
        except Exception as e:
            QMessageBox.warning(None, "WARNING", f"Could not save the list of open tabs.\n{str(e)}")

        project_settings.endGroup()

    # TODO: merge setup() method with update_project_dir()
    def setup(self, main_window: AbstractMainWindow):
        """
        - initialize some attributes.
        - close all tabs.
        - Reload previously opened files if called_from_python_script is False.
        """
        self.main_window = main_window

        # set output
        self.output = main_window.output

        # set save_key_sequence
        self.save_key_sequence = main_window.ui.actionSave.shortcut

        # connects to append_dialog() slot
        self.subset_objects_dialog_requested.connect(main_window.append_dialog)

        # reset all IODE databases

        # close all tabs
        self.clear()

        # update settings filepath in each Iode object tab
        for tab_database in self.tabs_database:
            tab_database.setup(main_window)

        # add a default tab for each IODE type of objects
        for tab_database in self.tabs_database:
            self.addTab(tab_database, "")

        # remove close button for IODE objects tabs
        for i in range(self.count()):
            self.tabBar().tabButton(i, QTabBar.ButtonPosition.RightSide).deleteLater()
            self.tabBar().setTabButton(i, QTabBar.ButtonPosition.RightSide, None)

        # make sure that all IODE database tabs has a default text and tooltip
        # (usually when starting a new project)
        for index in range(self.count()):
            tab_widget: IodeAbstractWidget = self.widget(index)
            self.setTabText(index, tab_widget.tab_text)
            self.setTabToolTip(index, tab_widget.tooltip)

        # restore previously opened files (if called_from_python_script is False)
        self.load_settings()
        
        self.show()

    def update_colors(self):
        """
        Update the colors of all tabs according to the current color theme.
        """
        for index in range(self.count()):
            tab_widget: IodeAbstractWidget = self.widget(index)
            tab_widget.update_colors()

    def _reset_file_system_watcher(self):
        """
        Reset all the paths (to directories and files) registered in the file system watcher.
        """
        # remove all previously registered directories and files
        dirs = self.file_system_watcher.directories()
        if dirs:
            self.file_system_watcher.removePaths(dirs)
        files = self.file_system_watcher.files()
        if files:
            self.file_system_watcher.removePaths(files)

    def _add_action(self, name: str, tooltip: str, slot_func, 
                    shortcut: Union[QShortcut, QKeySequence] = None) -> QAction:
        """
        Add an action to the context menu.

        Args:
            name (str): Name of the action displayed in the context menu.
            tooltip (str): Tooltip for the action.
            shortcut (QKeySequence, optional): Shortcut for the action. 
            Defaults to QKeySequence().

        Returns:
            QAction: The created action.
        """
        action = QAction(name, self)
        action.setToolTip(tooltip)
        action.setVisible(True)
        if shortcut is not None: 
            key_sequence = shortcut.key() if isinstance(shortcut, QShortcut) else shortcut
            if not key_sequence.isEmpty():
                action.setShortcut(key_sequence)
        action.triggered.connect(slot_func)
        self.context_menu.addAction(action)
        return action

    def _setup_context_menu(self):
        """Create a context menu for the tab bar."""
        # --- directory context menu
        self.context_menu = QMenu(self)

        # NOTE: close tab action MUST BE DISABLE FOR TABS REPRESENTING AN IODE DATABASE
        self.action_close = self._add_action("Close", "Close the current tab", self.close_tab, 
                                             self.close_shortcut)
        self._add_action("Save", "Save the content of the current tab", self.save_tab, 
                         self.save_key_sequence)
        # NOTE: clear tab (ONLY FOR TABS REPRESENTING AN IODE DATABASE)
        self.action_clear = self._add_action("Clear", "Clear the content of the current tab", 
                                             self.clear_tab, self.clear_shortcut)
        
        self.context_menu.addSeparator()

        self._add_action("Copy Absolute Path", "Copy the absolute path of the current file", 
                         self.absolute_path, self.filepath_shortcut)
        self._add_action("Copy Relative Path", "Copy the relative path of the current file", 
                         self.relative_path)
        self._add_action("Reveal in File Manager", "Reveal the current file in the file manager",
                         self.reveal_in_folder, self.reveal_explorer_shortcut)

        self.context_menu.addSeparator()

        self.action_split_v = self._add_action("Split Right", "Split the current tab vertically",
                                               self.split_tab_horizontally)
        self.action_split_h = self._add_action("Split Down", "Split the current tab horizontally",
                                               self.split_tab_vertically)
        self.action_undo_split = self._add_action("Undo split", "Undo split the current tab", 
                                                  self.undo_split)

    def _get_iode_obj_tab_index(self, iode_type: IodeType) -> int:
        """
        Get the index of the tab corresponding to the given IodeType.

        :param iode_type: The IodeType to get the index for.
        :return: The index of the tab, or -1 if not found.
        """
        if iode_type is not None:
            tab_database: QWidget = self.tabs_database[int(iode_type)]
            return self.indexOf(tab_database)
        else:
            return -1

    def _get_iode_obj_tab(self, iode_type: IodeType) -> Optional[AbstractIodeObjectWidget]:
        """
        Get the tab corresponding to the given IodeType.

        :param iode_type: The IodeType to get the tab for.
        :return: The tab, or None if not found.
        """
        index = self._get_iode_obj_tab_index(iode_type)
        return self.widget(index) if index >= 0 else None

    def _add_report_tab(self, filepath: str) -> int:
        """
        Add a tab for editing reports.

        Args:
            filepath (str): Path to the report file.

        Returns:
            int: Index of the new tab.
        """
        report_widget = ReportWidget(filepath, self.main_window, self)
        index = self.addTab(report_widget, report_widget.tab_text)
        self.setTabToolTip(index, report_widget.tooltip)

        report_widget.tab_text_modified.connect(self.tab_text_modified_slot)
        report_widget.ask_compute_hash.connect(self.compute_hash)

        return index

    def _add_text_tab(self, filepath: str, iode_file: IodeFileType, forced: bool = False) -> int:
        """
        Add a tab for editing a text file (.log, .ini, .txt).

        Args:
            filepath (str): Path to the text file.
            iode_file: The type of the Iode file.
            forced (bool, optional): Whether the file has been forced to be opened 
            as text tab. Defaults to False.

        Returns:
            int: Index of the new tab.
        """
        text_widget = TextWidget(iode_file, filepath, self.main_window, self)
        index = self.addTab(text_widget, text_widget.tab_text)
        self.setTabToolTip(index, text_widget.tooltip)

        if forced:
            text_widget.forced_as_text = True

        text_widget.tab_text_modified.connect(self.tab_text_modified_slot)

        return index

    def _save_tab_content(self, index: int) -> bool:
        """
        Save the content of the tab at a given index.
        Ask for a filepath if the tab represents an IODE database and 
        the database is not linked to any file.

        Args:
            index (int): Index of the tab to be saved.

        Returns:
            bool: Whether or not the method has succeeded.
        """
        try:
            tab_widget: IodeAbstractWidget = self.widget(index)
            filepath = tab_widget.save()
            if not filepath:
                return False
            self.setTabText(index, tab_widget.tab_text)
            self.setTabToolTip(index, tab_widget.tooltip)
            return True
        except Exception as e:
            QMessageBox.warning(None, "WARNING", "Couldn't save the content "
                                f"of '{self.tabText(index)}'")
            return False

    def _split_tab(self, index: int, orientation: Qt.Orientation):
        """
        Split the content of the tab at position index into two views.

        :param index: The index of the tab to split.
        :param orientation: The orientation of the split.
        """
        tab_widget = self.widget(index)
        if isinstance(tab_widget, IodeAbstractWidget):
            tab_widget.split(orientation)

    def _undo_split_tab(self, index: int):
        """
        Undo split the tab at position index.

        :param index: The index of the tab to undo_split.
        """
        tab_widget = self.widget(index)
        if isinstance(tab_widget, IodeAbstractWidget):
            tab_widget.undo_split()

    def load_file(self, filepath: str, display_tab: bool = True, force_overwrite: bool = False,
                  move_to_position: int = -1, force_as_text: bool = False) -> int:
        """
        Load a file and set corresponding tab text (filename) and tooltip (full absolute path).

        :param filepath: QString path to the file to load.
        :param display_tab: bool whether or not displaying the corresponding tab when 
                            finished to load the file (used in load_settings()).
        :param force_overwrite: bool whether or not forcing overwrite of the corresponding IODE database 
                                (use in load_setting()).
        :param move_to_position: int new position of the corresponding tab.
                                 Only used when reloading tabs from a previous session.
        :param force_as_text: bool whether or not to force to open the corresponding file 
                              as text (if not an IODE database file or report file)

        :return: int index of the corresponding tab. -1 if failed to load file.
        """
        if not filepath:
            QMessageBox.warning(None, "WARNING", "Cannot load file because given filepath is empty")
            return -1

        try:
            filepath = Path(filepath).resolve()

            if not filepath.exists():
                QMessageBox.warning(None, "WARNING", f"File {filepath} has not been found!")
                return -1

            if filepath.is_dir():
                QMessageBox.warning(None, "WARNING", f"Cannot load {filepath} as it is not "
                                    "a file but a directory")
                return -1
            
            str_filepath = str(filepath)
            file_type: IodeFileType = get_iode_file_type(str_filepath)

            # checks if the file has been already loaded
            # if file is already open in a tab, shows the tab
            self._build_files_list()
            if str_filepath in self.files_list:
                if not force_overwrite:
                    reload_file = QMessageBox.question(None, "WARNING", 
                                                    f"The file {filepath.name} is already open.\n"
                                                    "Would you like to reload it?")
                    force_overwrite = reload_file == QMessageBox.StandardButton.Yes
                if force_overwrite:
                    self.reload_file(str_filepath)
                index = self.files_list.index(str_filepath)
                self.setCurrentIndex(index)
                return index

            # ---- load file ----
            # load IODE database file
            if file_type <= IodeFileType.FILE_VARIABLES:
                tab_widget = self.tabs_database[int(file_type)]
                success = tab_widget.load(str_filepath, force_overwrite)
                if success:
                    tab_widget.reset_filter()
                    index = self.indexOf(tab_widget)
                    self.setTabText(index, tab_widget.tab_text)
                    self.setTabToolTip(index, tab_widget.tooltip)
                    self.files_list[index] = str_filepath
                else:
                    return -1
            # load non-IODE database file and create a new tab
            else:
                index = self.add_new_tab(file_type, str_filepath, force_as_text)

            if move_to_position >= 0:
                self.tabBar().moveTab(index, move_to_position)
                index = move_to_position

            if index >= 0 and display_tab:
                self.show_tab(index)

            return index
        except Exception as e:
            QMessageBox.warning(None, "WARNING", f"Could not load file {filepath}:\n{str(e)}")
            return -1

    def reload_file(self, filepath: str):
        """
        Reload a file if modified by an external program.

        :param filepath: QString Filepath to the modified file.
        """
        # find index of corresponding tab
        if filepath not in self.files_list:
            return
        
        # reload file
        try:
            index = self.files_list.index(filepath)
            tab_widget: IodeAbstractWidget = self.widget(index)
            tab_widget.load(filepath, True)
        except Exception as e:
            QMessageBox.warning(None, "WARNING", f"Could not reload file {filepath}:\n{str(e)}")

    def save_project_as(self, new_project_dir: QDir) -> bool:
        """
        Save current project to another (new) directory.

        :param new_project_dir: (new) directory to save the current project in.
        :return: bool whether or not all files have been saved, i.e. the user as 
        not clicked on the Discard button.
        """
        try:
            new_project_path = Path(new_project_dir.absolutePath())

            if not new_project_path.exists():
                answer = QMessageBox.question(None, "WARNING", f"Directory {new_project_path} "
                                              "does not exist.\nWould like to create it?", 
                                              QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No)
                if answer == QMessageBox.StandardButton.No:
                    return False
                else:
                    new_project_path.mkdir()
                shutil.copytree(self.project_dir_path, new_project_path, dirs_exist_ok=True)
            return True
        except Exception as e:
            QMessageBox.critical(None, "ERROR", str(e))
            return False

    def add_new_tab(self, file_type: IodeFileType, filepath: str, force_as_text: bool = False) -> int:
        """
        Create a new tab corresponding to the newly open file.
        Called by loadFile().

        :param file_type: IodeFileType type of the loaded file (IODE database file, .log, .ini, .txt, other).
        :param filepath: path to the loaded file.
        :param force_as_text: bool whether or not to force to open the corresponding file as text
                              (if not an IODE database file or report file)
        :return: int index of the new tab
        """
        # Check if file_type does not correspond to a database file
        if file_type <= IodeFileType.FILE_VARIABLES:
            return -1

        try:
            filepath = Path(filepath).resolve()
            str_filepath = str(filepath)

            # Check if file is already opened
            if filepath in self.files_list:
                index = self.files_list.index(filepath)
                return index

            # Prepare and add the new tab
            if file_type == IodeFileType.FILE_REP:
                index = self._add_report_tab(str_filepath)
            elif force_as_text or filepath.suffix in SHOW_IN_TEXT_TAB_EXTENSIONS_LIST:
                index = self._add_text_tab(str_filepath, file_type, force_as_text)
            else:
                # ask OS to open the file with the default application
                QDesktopServices.openUrl(QUrl.fromLocalFile(str_filepath))
                index = -1

            return index
        except Exception as e:
            QMessageBox.warning(None, "WARNING", f"Could not add new tab for file {filepath.name}:\n{str(e)}")
            return -1

    def update_object_tab(self, iode_type: IodeType) -> int:
        """
        Update the tab corresponding to a given Iode database.

        Args:
            iode_type (IodeType): Iode type of the Iode database to search in tabs.

        Returns:
            int: index of the tab corresponding to the 'iode_type' database. 
                 -1 if failed to update tab.
        """
        try:
            index = self._get_iode_obj_tab_index(iode_type)
            if index < 0:
                return index

            # Extract corresponding tab widget
            tab_widget: AbstractIodeObjectWidget = self.widget(index)

            # Compute hash
            tab_widget.compute_hash()

            # Try to rerun filter (with silent = True)
            tab_widget.filter(silent=True)

            # Update tab text and tooltip
            self.setTabText(index, tab_widget.tab_text)
            self.setTabToolTip(index, tab_widget.tooltip)
        except Exception as e:
            QMessageBox.critical(None, "ERROR", str(e))
            return -1

        return index

    def show_tab(self, index: int = -1):
        """
        Show tab corresponding to index.
        Update current tab if index is -1.

        :param index: int index of the tab to show.
        """
        # get the index of the tab currently visible if not passed to the method
        if index < 0:
            index = self.currentIndex()

        # update widget if needed
        tab_widget = self.widget(index)
        if tab_widget is not None:
            tab_widget.update()

        self.setCurrentIndex(index)

    def reset_filters(self):
        """
        Reset all filters on IODE objects.
        """
        for tab_widget in self.tabs_database:
            tab_widget.reset_filter()

    def reset_filter(self, iode_type: IodeType):
        """
        Reset the corresponding filter.

        Args:
            iode_type: The type of IODE object.
        """
        tab_database: AbstractIodeObjectWidget = self._get_iode_obj_tab(iode_type)
        if tab_database is not None:
            tab_database.reset_filter()

    def clear_workspace(self):
        """
        Clear all global IODE databases and update all IODE database tabs.
        """
        for tab_database in self.tabs_database:
            tab_database.clear_database()
        
        for iode_type in IodeType:
            self.update_object_tab(iode_type)

    def get_selected_objects_names(self, iode_type: IodeType) -> Optional[List[str]]:
        """
        Get the list of names of selected objects.

        Args:
            iode_type: The type of IODE object.

        Returns:
            A list of selected object names.
        """
        try:
            tab_database = self._get_iode_obj_tab(iode_type)
            if tab_database is not None:
                return tab_database.get_selected_objects_names()  
            else:
                return None
        except Exception as e:
            QMessageBox.critical(None, "ERROR", str(e))

    def open_add_dialog(self, iode_type: IodeType):
        """
        To be called from executed command lines or IODE reports

        :param iode_type:
        """
        try:
            index = self._get_iode_obj_tab_index(iode_type)
            if index < 0:
                QMessageBox.warning(None, "WARNING", "Failed to open add dialog box "
                                    f"for {iode_type.name} objects.")
                return
            tab_database: AbstractIodeObjectWidget = self.widget(index)
            if tab_database is not None:
                tab_database.open_add_dialog()
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))

    def open_edit_dialog(self, iode_type: IodeType):
        """
        To be called from executed command lines or IODE reports
        """
        try:
            index = self._get_iode_obj_tab_index(iode_type)
            if index < 0:
                QMessageBox.warning(None, "WARNING", "Failed to open edit dialog box "
                                    f"for {iode_type.name} objects.")
                return
            tab_database: AbstractIodeObjectWidget = self.widget(index)
            if tab_database is not None:
                tab_database.open_edit_dialog()
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))

    @Slot(str, bool)
    def tab_text_modified_slot(self, filepath: str, modified: bool):
        """
        updates the tab name and tooltip and sends the file_content_modified signal to be
        caught by the file explorer.

        :param filepath: the path of the file
        :param modified: whether the file is modified or not
        """
        try:
            # get index
            if filepath not in self.files_list:
                return
            index = self.files_list.index(filepath)

            # get the tab widget at the given index
            tab_widget: IodeAbstractWidget = self.widget(index)

            # updates the text and tooltip of the tab
            self.setTabText(index, tab_widget.tab_text)
            self.setTabToolTip(index, tab_widget.tooltip)

            # sends a signal to the File Explorer widget
            self.file_content_modified.emit(tab_widget.filepath, modified)
        except Exception as e:
            QMessageBox.critical(None, "ERROR", str(e))

    @Slot(IodeType, bool)
    def tab_database_modified(self, iode_type: IodeType, modified: bool):
        """
        updates the tab name and tooltip and sends the file_content_modified signal to be
        caught by the file explorer.

        :param iode_type: the type of the database
        :param modified: whether the database is modified or not
        """
        try:
            # get index
            index = self._get_iode_obj_tab_index(iode_type)
            if index < 0:
                return

            # get the tab widget at the given index
            tab_widget: AbstractIodeObjectWidget = self.widget(index)

            # updates the text and tooltip of the tab
            self.setTabText(index, tab_widget.tab_text)
            self.setTabToolTip(index, tab_widget.tooltip)

            # sends a signal to the File Explorer widget
            self.file_content_modified.emit(tab_widget.filepath, modified)
        except Exception as e:
            QMessageBox.critical(None, "ERROR", str(e))

    @Slot(IodeType, list)
    def show_objects_list(self, iode_type: IodeType, obj_names: List[str] = None):
        """
        popup a new dialog box containing the list of objects.

        :param iode_type: the type of the database
        :param obj_names: the list of object names
        """
        if obj_names is None or len(obj_names) == 0:
            return

        dialog = QDialog(self)

        try:
            grid_layout = QGridLayout(dialog)
            grid_layout.setObjectName("gridLayout")

            database_widget = None
            if iode_type == IodeType.COMMENTS:
                database_widget = CommentsWidget(dialog)
            elif iode_type == IodeType.EQUATIONS:
                database_widget = EquationsWidget(dialog)
            elif iode_type == IodeType.IDENTITIES:
                database_widget = IdentitiesWidget(dialog)
            elif iode_type == IodeType.LISTS:
                database_widget = ListsWidget(dialog)
            elif iode_type == IodeType.SCALARS:
                database_widget = ScalarsWidget(dialog)
            elif iode_type == IodeType.TABLES:
                database_widget = TablesWidget(dialog)
            elif iode_type == IodeType.VARIABLES:
                database_widget = VariablesWidget(dialog)

            index = self._get_iode_obj_tab_index(iode_type)
            if index < 0:
                raise RuntimeError(f"Cannot show IODE objects of type {iode_type.name}\n"
                                   f"Database of objects of type {iode_type.name} not found")

            tab_database: AbstractIodeObjectWidget = self.widget(index)
            database_widget.tab_database_modified.connect(lambda iode_type, modified: tab_database.set_modified(modified))
            
            database_widget.update()

            pattern = ";".join(obj_names)
            database_widget.filter(pattern)

            database_widget.setObjectName("database_widget")
            grid_layout.addWidget(database_widget, 0, 0, 1, 1)

            dialog.setWindowModality(Qt.WindowModality.NonModal)
            dialog.setModal(False)

            h = min(30 + database_widget.preferred_height(), self.height())
            dialog.resize(self.width(), h)

            self.subset_objects_dialog_requested.emit(dialog)

        except Exception as e:
            dialog.deleteLater()
            QMessageBox.critical(None, "ERROR", str(e))

    @Slot()
    @Slot(bool)
    def compute_hash(self, before: bool=False):
        """
        compute the hash of each database before/after executing
        a report command or an iode report.
        if hash is different after report command/iode report,
        a database_modified signal is emitted.

        :param before: whether the hash is calculated before or after the
                      execution of the report command/iode report.
        """
        for tab_database in self.tabs_database:
            tab_database.compute_hash(before)

    @Slot(int)
    def remove_tab(self, index: int):
        """
        ask to save if modified then close the tab + remove path from the
        system file watcher.

        :param index: the index of the tab to remove
        """
        try:
            # Ask to save if modified
            tab_widget: Union[TextWidget, ReportWidget] = self.widget(index)
            if tab_widget.modified:
                filepath = tab_widget.filepath
                msg = "Unsaved modifications.\n"
                msg += "Would you like to save modifications to " + filepath + " before closing the tab?"
                answer = QMessageBox.warning(None, "WARNING", msg,
                                            QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No | QMessageBox.StandardButton.Discard,
                                            QMessageBox.StandardButton.Yes)
                if answer == QMessageBox.StandardButton.Yes:
                    tab_widget.save()
                elif answer == QMessageBox.StandardButton.No:
                    tab_widget.tab_text_modified_slot(filepath, False)
                elif answer == QMessageBox.StandardButton.Discard:
                    return

            # Remove the path from the system file watcher
            self.file_system_watcher.removePath(tab_widget.filepath)

            # Close the tab
            super().removeTab(index)
        except Exception as e:
            QMessageBox.warning(None, "WARNING", "Could not close the tab.\n" + str(e))

    @Slot()
    def show_next_tab(self):
        """
        show next tab.
        """
        last_index = self.count() - 1
        if self.currentIndex() != last_index:
            self.setCurrentIndex(self.currentIndex() + 1)

    @Slot()
    def show_previous_tab(self):
        """
        show previous tab.
        """
        if self.currentIndex() != 0:
            self.setCurrentIndex(self.currentIndex() - 1)

    @Slot(int)
    def save_tab_content(self, index: int) -> bool:
        """
        saves the content of the tab at position index
        :param index: the index of the tab to save
        """
        try:
            success = self._save_tab_content(index)
            self.index_context_menu = -1
            return success
        except Exception as e:
            QMessageBox.warning(None, "WARNING", "Could not save tab.\n" + str(e))
            return False 

    @Slot()
    def save_all_tabs(self) -> bool:
        """
        save all open tabs.
        ask for a filepath if the tab represents an IODE database and 
        the database is not linked to any file.

        :return: whether or not the method has succeeded.
        """
        success = True
        for index in range(self.count()):
            if not self._save_tab_content(index):
                success = False
        return success

    @Slot(str)
    def update_project_dir(self, project_dir_path: str):
        """
        update the path to the current project directory.
        slot method called each time the project directory is changed.

        :param project_dir_path: path to the root directory of the file explorer
        """
        try:
            self.project_dir_path = project_dir_path
            # associate new project directory to each IODE database tab + clear global IODE databases
            project_dir = QDir(project_dir_path)
            for tab_database in self.tabs_database:
                tab_database.project_dir = project_dir
            self._reset_file_system_watcher()
        except Exception as e:
            QMessageBox.warning(None, "WARNING", f"Could not open the project directory "
                                f"'{project_dir_path}'.\n" + str(e))

    @Slot(str, str)
    def file_moved(self, old_filepath: str, new_filepath: str):
        """
        method called when a user moves a file using the file explorer.
        update the name and tooltip af tab associated with the file if open.

        :param old_filepath: previous filepath
        :param new_filepath: new filepath
        """
        try:
            # check if old_file_path present in currently open tabs
            if old_filepath in self.files_list:
                index = self.files_list.index(old_filepath)
                tab_widget: IodeAbstractWidget = self.widget(index)
                # update filepath in widget
                tab_widget.filepath = new_filepath
                # update name and tooltip of corresponding tab
                self.setTabText(index, tab_widget.tab_text)
                self.setTabToolTip(index, tab_widget.tooltip)
                # update list of open files (tabs)
                self.files_list[index] = new_filepath

            # update file system watcher
            self.file_system_watcher.removePath(old_filepath)
            self.file_system_watcher.addPath(new_filepath)
        except Exception as e:
            QMessageBox.warning(None, "WARNING", f"Could not move the file "
                                f"'{old_filepath}' to '{new_filepath}'.\n" + str(e))

    @Slot(str)
    def file_deleted(self, filepath: str):
        """
        method called when a user deletes a file using the file explorer.
        close the tab associated with the file if open.

        :param filepath: path to the deleted file
        """
        try:
            # check if filepath present in currently open tabs
            if filepath in self.files_list:
                index = self.files_list.index(filepath)
                self.remove_tab(index)
        except Exception as e:
            QMessageBox.warning(None, "WARNING", f"Could not delete the file "
                                f"'{filepath}'.\n" + str(e))
    
    @Slot(str, str, str)
    def file_renamed(self, path: str, old_name: str, new_name: str):
        """
        method called when a user renames a file using the file explorer.
        update the name and tooltip of the tab associated with the renamed file if open.

        :param path: path to the parent directory
        :param old_name: name of the file before renaming
        :param new_name: name of the file after renaming
        """
        parent_dir = QDir(path)
        old_filepath = parent_dir.absoluteFilePath(old_name)
        new_filepath = parent_dir.absoluteFilePath(new_name)
        self.file_moved(old_filepath, new_filepath)

    @Slot()
    def _build_files_list(self):
        """
        build the list of filepaths associated to open tabs.

        returns:
            list: list of open files (tabs)
        """
        self.files_list = [self.widget(i).filepath for i in range(self.count())]

    @Slot(QPoint)
    def onCustomContextMenu(self, point: QPoint):
        """
        popups a context menu.
        """
        if point.isNull():
            return

        self.index_context_menu = self.tabBar().tabAt(point)
        tab_widget: IodeAbstractWidget = self.widget(self.index_context_menu)
        global_point = self.tabBar().mapToGlobal(point)

        # by design, there is only one tab per type of iode database
        # the tabs representing an iode database cannot be closed
        if tab_widget.file_type <= IodeFileType.FILE_VARIABLES:
            self.action_close.setVisible(False)
            self.action_clear.setVisible(True)
        else:
            self.action_close.setVisible(True)
            self.action_clear.setVisible(False)

        # display option split or undo_split depending on the tab 
        # is already splitted or not
        if tab_widget.splitted:
            self.action_split_h.setVisible(False)
            self.action_split_v.setVisible(False)
            self.action_undo_split.setVisible(True)
        else:
            self.action_split_h.setVisible(True)
            self.action_split_v.setVisible(True)
            self.action_undo_split.setVisible(False)

        self.context_menu.exec(global_point)

    @Slot(int)
    def tab_inserted(self, index: int):
        """
        this method is called after a new tab was added or inserted at position index.

        :param index: the index of the tab that was inserted.
        """
        self._build_files_list()
        tab_widget: IodeAbstractWidget = self.widget(index)
        self.file_system_watcher.addPath(tab_widget.filepath)
        super().tabInserted(index)

    @Slot(int)
    def tab_removed(self, index: int):
        """
        this method is called after a tab was removed from position index.

        :param index: the index of the tab that was removed.
        """
        self._build_files_list()

    @Slot()
    def save_tab(self):
        try:
            index = self.index_context_menu if self.index_context_menu > 0 else self.currentIndex()
            self._save_tab_content(index)
            self.index_context_menu = -1
        except Exception as e:
            QMessageBox.warning(None, "WARNING", "Could not save tab.\n" + str(e))

    @Slot()
    def close_tab(self):
        """
        closes the tab at position index where index is:
        - current tab index if method called using a shortcut
        - index defined by the context menu if called from a context menu
        """
        try:
            index = self.index_context_menu if self.index_context_menu > 0 else self.currentIndex()
            tab_widget: IodeAbstractWidget = self.widget(index)
            file_type = tab_widget.file_type
            # Tabs representing an IODE database file cannot be closed.
            # This is by design.
            if file_type <= IodeFileType.FILE_VARIABLES:
                return
            self.removeTab(index)
            self.index_context_menu = -1
        except Exception as e:
            QMessageBox.warning(None, "WARNING", "Could not close tab.\n" + str(e))

    @Slot()
    def clear_tab(self):
        """
        clears the iode database displayed in the tab at position index where index is:
        - current tab index if method called using a shortcut
        - index defined by the context menu if called from a context menu
        """
        try:
            index = self.index_context_menu if self.index_context_menu > 0 else self.currentIndex()
            tab_widget: IodeAbstractWidget = self.widget(index)
            file_type = tab_widget.file_type

            if file_type <= IodeFileType.FILE_VARIABLES:
                tab_database: AbstractIodeObjectWidget = tab_widget
                answer = QMessageBox.warning(None, "WARNING",
                                            f"Are you sure to clear the {IODE_DATABASE_TYPE_NAMES[int(file_type)]} database",
                                            QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.Cancel, 
                                            QMessageBox.StandardButton.Yes)
                if answer == QMessageBox.StandardButton.Yes:
                    tab_database.clear_database()
                    self.update_object_tab(IodeType(int(file_type)))

            # reset context menu index
            self.index_context_menu = -1
        except Exception as e:
            QMessageBox.warning(None, "WARNING", "Could not clear tab.\n" + str(e))

    @Slot()
    def absolute_path(self):
        """
        copy the absolute path of the selected path to the selected file.
        """
        try:
            index = self.currentIndex()
            tab_widget: IodeAbstractWidget = self.widget(index)
            filepath = tab_widget.filepath
            clipboard = QApplication.clipboard()
            clipboard.setText(filepath)
            self.index_context_menu = -1
        except Exception as e:
             QMessageBox.warning(None, "WARNING", "Could not copy the absolute path to the clipboard.\n" + str(e))

    @Slot()
    def relative_path(self):
        """
        copy the relative path of the selected path to the selected file.
        """
        try:
            index = self.currentIndex()
            tab_widget: IodeAbstractWidget = self.widget(index)
            filepath = tab_widget.filepath
            project_dir = QDir(self.project_dir_path)
            relative_path = project_dir.relativeFilePath(filepath)
            clipboard = QApplication.clipboard()
            clipboard.setText(relative_path)
            self.index_context_menu = -1
        except Exception as e:
             QMessageBox.warning(None, "WARNING", "Could not copy the relative path to the clipboard.\n" + str(e))


    @Slot()
    def reveal_in_folder(self):
        """
        open an os file explorer and highlight the selected file.
        if several files selected, reveal only the last one (as in visual studio code).
        """
        try:
            index = self.currentIndex()
            tab_widget: IodeAbstractWidget = self.widget(index)
            filepath = tab_widget.filepath

            # For Windows
            if sys.platform.startswith('win'):
                args = ["/select,", QDir.toNativeSeparators(filepath)]
                if QProcess.startDetached("explorer", args):
                    return
            # For MacOS
            elif sys.platform.startswith('darwin'):
                args = [
                    "-e", "tell application \"Finder\"",
                    "-e", "activate",
                    "-e", f"select POSIX file \"{filepath}\"",
                    "-e", "end tell",
                    "-e", "return"
                ]
                if not QProcess.execute("/usr/bin/osascript", args):
                    return
            # For other platforms
            else:
                QDesktopServices.openUrl(QUrl.fromLocalFile(filepath))

            self.index_context_menu = -1
        except Exception as e:
            QMessageBox.warning(None, "WARNING", "Could not reveal the file in the file explorer.\n" + str(e))

    @Slot()
    def split_tab_horizontally(self):
        """
        split the tab horizontally.
        """
        try:
            self._split_tab(self.index_context_menu, Qt.Orientation.Horizontal)
            self.index_context_menu = -1
        except Exception as e:
            QMessageBox.warning(None, "WARNING", "Could not split the tab horizontally.\n" + str(e))

    @Slot()
    def split_tab_vertically(self):
        """
        split the tab vertically.
        """
        try:
            self._split_tab(self.index_context_menu, Qt.Orientation.Vertical)
            self.index_context_menu = -1
        except Exception as e:
            QMessageBox.warning(None, "WARNING", "Could not split the tab vertically.\n" + str(e))

    @Slot()
    def undo_split(self):
        """
        undo split of the tab.
        """
        try:
            self._undo_split_tab(self.index_context_menu)
            self.index_context_menu = -1
        except Exception as e:
            QMessageBox.warning(None, "WARNING", "Could not undo split of the tab.\n" + str(e))
