from PySide6.QtCore import Qt, QDir, QSettings, QFileInfo, Slot
from PySide6.QtGui import QAction, QShortcut, QKeySequence, QCloseEvent, QDesktopServices
from PySide6.QtWidgets import (QMainWindow, QMessageBox, QDialog, QLabel, QPlainTextEdit, 
                               QGridLayout, QFileDialog, QTextEdit, QLineEdit)

try:
    from qtconsole.rich_jupyter_widget import RichJupyterWidget
    from qtconsole.inprocess import QtInProcessKernelManager
    from IPython import get_ipython

    # make sure there is no IPython instance already running
    # see notes from file editor.py of the larray-editor project:
    #     https://github.com/larray-project/larray-editor/blob/master/larray_editor/editor.py
    ipython_instance = get_ipython()
    if ipython_instance is None:
        qtconsole_available = True
    else:
        qtconsole_available = False
except ImportError:
    RichJupyterWidget = None
    qtconsole_available = False

from iode import IodeType, comments, equations, identities, lists, scalars, tables, variables
from typing import List

from iode_gui.utils import (IODE_VERSION_MAJOR, IODE_VERSION_MINOR, IODE_VERSION_PATCH, 
                            ORGANIZATION_NAME, URL_HOMEPAGE, URL_CHANGELOG, URL_PYTHON_API, 
                            URL_MANUAL, URL_SHORTCUTS, DEFAULT_FONT_FAMILY)

from iode_gui.abstract_main_window import AbstractMainWindow
from iode_gui.settings import ProjectSettings
from iode_gui.iode_objs.edit.edit_vars_sample import EditIodeSampleDialog

from iode_gui.menu.file.file_new_project import MenuFileNewProject
from iode_gui.menu.file.file_import_comments import MenuFileImportComments
from iode_gui.menu.file.file_import_variables import MenuFileImportVariables
from iode_gui.menu.file.file_export import MenuFileExport
from iode_gui.menu.file.file_settings import MenuFileSettings

from iode_gui.menu.workspace.workspace_copy_into import MenuWorkspaceCopyInto
from iode_gui.menu.workspace.workspace_merge_into import MenuWorkspaceMergeInto
from iode_gui.menu.workspace.workspace_describe import MenuWorkspaceDescribe
from iode_gui.menu.workspace.workspace_extrapolate_variables import MenuWorkspaceExtrapolateVariables
from iode_gui.menu.workspace.workspace_high_to_low import MenuWorkspaceHighToLow
from iode_gui.menu.workspace.workspace_low_to_high import MenuWorkspaceLowToHigh
from iode_gui.menu.workspace.workspace_seasonal_adjustment import MenuWorkspaceSeasonalAdjustment
from iode_gui.menu.workspace.workspace_trend_correction import MenuWorkspaceTrendCorrection

from iode_gui.menu.data.sort_expand_list import MenuDataSortExpandList
from iode_gui.menu.data.list_calculus import MenuDataListCalculus
from iode_gui.menu.data.file_contents import MenuDataFileContents
from iode_gui.menu.data.file_compare import MenuDataFileCompare
from iode_gui.menu.data.search_text import MenuDataSearchText
from iode_gui.menu.data.scan_objects import MenuDataScanObjects
from iode_gui.menu.data.edit_tables import MenuDataEditTables
from iode_gui.menu.data.duplicate import MenuDataDuplicateObj

from iode_gui.menu.compute.compute_identities import MenuComputeIdentities
from iode_gui.menu.compute.compute_simulation import MenuComputeSimulation
from iode_gui.menu.compute.compute_model import MenuComputeModel
from iode_gui.menu.compute.compute_scc_decomposition import MenuComputeSCCDecomposition
from iode_gui.menu.compute.compute_scc_simulation import MenuComputeSCCSimulation

from iode_gui.menu.print_graph.print_tables import MenuPrintTables
from iode_gui.menu.print_graph.print_variables import MenuPrintVariables
from iode_gui.menu.print_graph.graph_tables import MenuGraphTables
from iode_gui.menu.print_graph.graph_variables import MenuGraphVariables

from iode_gui.ui_main_window import Ui_MainWindow


_iode_databases = {'comments': comments, 'equations': equations, 
                   'identities': identities, 'lists': lists, 'scalars': scalars, 
                   'tables': tables, 'variables': variables}


class MainWindow(AbstractMainWindow):
    """
    Implemented features:

    - CTRL + ALT + N creates a new project.
    - CTRL + O opens an existing project.
    - CTRL + ALT + S saves current project to another directory.
    - CTRL + S saves the current tab content.
    - CTRL + SHIFT + S saves all tabs content.
    - CTRL + SHIFT + D clears the whole workspace.
    """

    MAX_RECENT_PROJECTS = 30

    def __init__(self, parent=None, files_to_load: List[str]=None):
        super().__init__(parent)

        # ---- setup the present class ----
        # Create an instance of the widgets defined in the .ui file
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        self.ui.textEdit_output.setStyleSheet(f"font-family: {self.font_family}")
        self.ui.lineEdit_iode_command.setStyleSheet(f"font-family: {self.font_family}")

        self._setup_ipython_console()

        # ---- restore geometry and state ----
        self.restoreGeometry(self.user_settings.value("geometry"))
        self.restoreState(self.user_settings.value("windowState"))
        self.ui.dockWidget_file_explorer.restoreGeometry(self.user_settings.value("dockWidget_file_explorer_geometry"))
        self.ui.dockWidget_tools.restoreGeometry(self.user_settings.value("dockWidget_tools_geometry"))

        # ---- open file(s) passed as files_to_load argument ----
        files_to_load_ = []
        if files_to_load:
            parent_dir = None
            for filepath in files_to_load:
                fileInfo = QFileInfo(filepath)
                if not fileInfo.exists():
                    QMessageBox.warning(self, "WARNING", f"Cannot open '{filepath}'. File does not exist")
                elif not fileInfo.isFile():
                    QMessageBox.warning(self, "WARNING", "Only files can be open")
                    break
                else:
                    if parent_dir is None:
                        parent_dir = fileInfo.absolutePath()
                    if fileInfo.absolutePath() != parent_dir:
                        QMessageBox.warning(self, "WARNING", "All files to open must come from the same directory")
                        parent_dir = None
                        files_to_load_.clear()
                        break
                    files_to_load_.append(fileInfo.absoluteFilePath())
            if parent_dir:
                self.project_path = parent_dir

        # ---- menus ----
        self._build_recent_projects_menu()

        # ---- file explorer ----
        # connect the Tabs widget to the File Explorer
        self.ui.treeView_file_explorer.set_iode_tab_widget(self.ui.tabWidget_IODE_objs)

        # ---- iode commands line ----
        # setup the line widget to execute IODE commands
        self.ui.lineEdit_iode_command.setup(self)

        # ---- shortcuts ----
        self.full_screen_shortcut = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Key.Key_X), self)
        self.full_screen_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)

        # ---- signals and slots ----
        self.ui.lineEdit_iode_command.ask_compute_hash.connect(self.ui.tabWidget_IODE_objs.compute_hash)
        self.full_screen_shortcut.activated.connect(self.showMaximized)

        # ---- load project (if any) ----
        # first time launching the GUI -> ask the user to either start a new project
        # or to open an existing folder containing reports and/or KDB files as project
        if self.project_path is None:
            QMessageBox.information(self, "IODE interface",
                                    "<p align='center'>First time with the IODE interface ?<br><br>" +
                                    "Please go the File menu to either create a new project folder or to open an folder containing " +
                                    "reports and/or IODE database files.</p>" +
                                    "<p align='left'>To open file(s) from the file tree (left panel):<br>" +
                                    "- double click on a file,<br>" +
                                    "- select several files with CTRL and then press ENTER.</p>")
            self.ui.treeView_file_explorer.hide()
            self.ui.tabWidget_IODE_objs.hide()
            self.ui.dockWidget_file_explorer.hide()
        else:
            self.open_directory(self.project_path, True)

        # files_to_load is not empty
        for filepath in files_to_load_:
            self.ui.tabWidget_IODE_objs.load_file(filepath, True, True)

    @property
    def output(self) -> QTextEdit:
        return self.ui.textEdit_output
    
    @property
    def iode_command(self) -> QLineEdit:
        return self.ui.lineEdit_iode_command
    
    def _setup_ipython_console(self):
        self.grid_layout_tab_python = QGridLayout(self.ui.tab_python_console)
        self.grid_layout_tab_python.setObjectName(u"gridLayout")
        self.jupyter_widget = None

        if not qtconsole_available:
            QMessageBox.warning(self, "WARNING", "IPython console not available. "
                                "Please install qtconsole and ipython packages.")
            self.label_python_command = QLabel(self.ui.tab_python_console)
            self.label_python_command.setObjectName(u"label_python_command")
            self.grid_layout_tab_python.addWidget(self.label_python_command, 0, 0, 1, 1)

            self.lineEdit_python_command = QLineEdit(self.ui.tab_python_console)
            self.lineEdit_python_command.returnPressed.connect(self.execute_python_command)

            self.lineEdit_python_command.setObjectName(u"lineEdit_python_command")
            self.grid_layout_tab_python.addWidget(self.lineEdit_python_command, 0, 1, 1, 1)

            self.plainTextEdit_python_output = QPlainTextEdit(self.ui.tab_python_console)
            self.plainTextEdit_python_output.setObjectName(u"plainTextEdit_python_output")
            self.grid_layout_tab_python.addWidget(self.plainTextEdit_python_output, 1, 0, 1, 2)
        else:
            # ---- Start a kernel, connect to it, and create a RichJupyterWidget to use it ----
            # see https://github.com/jupyter/qtconsole 
            # and https://github.com/jupyter/qtconsole/blob/main/examples/embed_qtconsole.py 
            kernel_manager = QtInProcessKernelManager()
            kernel_manager.start_kernel()
            self.kernel = kernel_manager.kernel

            kernel_client = kernel_manager.client()
            kernel_client.start_channels()

            self.jupyter_widget = RichJupyterWidget()
            self.jupyter_widget.kernel_manager = kernel_manager
            self.jupyter_widget.kernel_client = kernel_client
            self.jupyter_widget.executed.connect(self.ipython_cell_executed)

            self.jupyter_widget.setMinimumHeight(20)
            self.jupyter_widget.setObjectName("ipython_widget")
            self.grid_layout_tab_python.addWidget(self.jupyter_widget, 0, 0, -1, -1)

        self._reset_kernel()

    def _reset_kernel(self):
        if self.jupyter_widget:
            self.kernel.shell.reset()
            # add iode public API to kernel namespace
            self.kernel.shell.run_cell('from iode import *', store_history=False)
            # push global IODE databases
            self.kernel.shell.push(_iode_databases)
        else:
            self.lineEdit_python_command.setText('')
            self.lineEdit_python_command.setPlaceholderText("Enter Python command here...")

    def _build_recent_projects_menu(self):
        self.ui.menuRecent_Projects.clear()
        for recent_project in self.recent_projects:
            project_dir = QDir(recent_project)
            action = QAction(project_dir.dirName(), self)
            absolute_path = project_dir.absolutePath()
            action.setStatusTip(absolute_path)
            action.setToolTip(absolute_path)
            action.setData(absolute_path)
            action.setVisible(True)
            action.triggered.connect(self.open_recent_project)
            self.ui.menuRecent_Projects.addAction(action)

    def _add_project_path_to_list(self, project_dir: QDir):
        project_path = project_dir.absolutePath()

        # add project directory path to list of recently opened projects (= directories)
        if project_path in self.recent_projects:
            self.recent_projects.remove(project_path)
            self.recent_projects = [project_path] + self.recent_projects
        else:
            self.recent_projects = [project_path] + self.recent_projects[:self.MAX_RECENT_PROJECTS - 1]

        # update user settings
        self.user_settings.setValue("recent_projects", self.recent_projects)

        # setup Menu > Recent Projects > list of recent projects
        self._build_recent_projects_menu()

    def _ask_save_all_tabs(self) -> QMessageBox.StandardButton:
        answer = QMessageBox.warning(self, "WARNING", "Save content of all open tabs?",
                                    QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No | QMessageBox.StandardButton.Discard,
                                    QMessageBox.StandardButton.Yes)
        if answer == QMessageBox.StandardButton.Yes:
            self.ui.tabWidget_IODE_objs.save_all_tabs()
        return answer

    def open_directory(self, dir_path: str, onStartup: bool = False) -> bool:
        """
        Update the project path and the content of the File Explorer.
        Ask to save workspace and content of all other tabs before to switch from project directory.
        If the new project directory contains a project settings file (.ini), reopen tabs listed in
        the project settings file.

        :param dir_path: path to the directory to open as IODE project root.
        :return: bool whether or not the IODE project root has been updated.
        """
        if not dir_path:
            QMessageBox.warning(self, "WARNING", "Empty path for new Project")
            return False
        
        # update project path
        project_dir = QDir(dir_path)
        if not project_dir.exists():
            QMessageBox.warning(self, "WARNING", "Directory " + project_dir.absolutePath() + " does not exist")
            return False
        self.project_path = project_dir.absolutePath()
        
        # create/update settings
        self.ui.treeView_file_explorer.save_settings()
        self.ui.tabWidget_IODE_objs.save_settings()
        ProjectSettings.change_project(project_dir, self)

        # update current directory (chdir)
        QDir.setCurrent(project_dir.absolutePath())

        # show File Explorer widget (in case it was hidden)
        self.ui.dockWidget_file_explorer.show()
        
        # update file explorer view
        self.ui.treeView_file_explorer.update_project_dir(project_dir, onStartup)
        
        # (re)open tabs
        self.ui.tabWidget_IODE_objs.setup(self)

        # add directory path to list of recently opened projects (= directories)
        self._add_project_path_to_list(project_dir)

        # set currentProjectPath global settings
        self.user_settings.setValue("project_path", self.project_path)

        # reset the IPython kernel
        # NOTE: this must be done after to update the File Explorer and the tabs widgets
        #       because the call to IodeTabWidget.setup() below will (re)open tabs and may  
        #       (re)load IODE databases. Since the reset_kernel() method first clears the 
        #       IPython namespace and then pushes the IODE global IODE databases, it must 
        #       be called after to (re)load IODE databases.
        self._reset_kernel()

        # open either the IODE commands tab of the IPython console tab
        project_settings: QSettings = ProjectSettings.project_settings
        tab_index = project_settings.value("tools_tab_index", type=int, defaultValue=0)
        self.ui.tabWidget_tools.setCurrentIndex(tab_index)

        # update auto-completion
        self.completer.update_iode_objects_list_names()

        return True

    def display_output(self, output):
        """
        Display output in the text edit widget.

        :param output: QString containing the output to display.
        """
        self.ui.textEdit_output.append(output)

    """
    NOTE: In case users ask to restore Alt + F<...> behavior.
          Alt + F4 is the OS (Windows) shortcut to close a program.
          So, Alt + F4 will actually triggers the closeEvent() method and since it is
          triggered directly by the OS, it will bypass the keyPressEvent() method.
          To force closeEvent() not to proceed and to propagate the key combination Alt + F4 
          to the whole program, you can do the following:
          
          void MainWindow::closeEvent(QCloseEvent* event)
          {
              // Check if the close event has been triggered by Alt or any Alt + <...> key combination
              // In that case, create a QKeyEvent CTRL + F4 and post it in the events queue
              if ((QApplication::queryKeyboardModifiers() == Qt::AltModifier))
              {
                  event->ignore();
                  QKeyEvent* event = new QKeyEvent(QEvent::KeyPress, Qt::Key_F4, Qt::AltModifier);
                  // postEvent() posts the event on a queue for later dispatch. 
                  // The next time Qt's main event loop runs, it dispatches all posted events, with some optimization. 
                  // https://doc.qt.io/qt-6/eventsandfilters.html#sending-events
                  QApplication::postEvent(this, event);
                  return;
              }
              (...)
          }
    """
    def closeEvent(self, event: QCloseEvent):
        """
        Handle the close event of the main window.

        :param event: QCloseEvent object representing the close event.
        """
        # ask to save all current tabs content before to switch
        answer = self._ask_save_all_tabs()

        if answer == QMessageBox.StandardButton.Discard: 
            event.ignore()
        else:
            self.user_settings.setValue("project_path", self.project_path)
            self.user_settings.setValue("font_family", self.font_family)
            self.user_settings.setValue("geometry", self.saveGeometry())
            self.user_settings.setValue("windowState", self.saveState())
            self.user_settings.setValue("dockWidget_file_explorer_geometry", self.ui.dockWidget_file_explorer.saveGeometry())
            self.user_settings.setValue("dockWidget_tools_geometry", self.ui.dockWidget_tools.saveGeometry())

            for dialog in self.dialogs: 
                dialog.close()
            self.dialogs.clear()

            self.ui.treeView_file_explorer.save_settings()
            self.ui.tabWidget_IODE_objs.save_settings()
            project_settings: QSettings = ProjectSettings.project_settings
            project_settings.setValue("tools_tab_index", self.ui.tabWidget_tools.currentIndex())

            event.accept()

    def _check_vars_sample(self):
        """
        Check the variables sample and ask to set it if not already defined.
        """
        if not variables.nb_periods:
            reply = QMessageBox.question(None, "Sample", "Sample undefined. Set it?", 
                                         QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No)
            if reply == QMessageBox.StandardButton.Yes:
                self.compute_hash(True)
                try:
                    dialog = EditIodeSampleDialog(self)
                    if dialog.exec() == QDialog.DialogCode.Accepted:
                        variables.set_sample(dialog.from_period, dialog.to_period)
                    self.update_tab_and_completer(IodeType.VARIABLES)
                except Exception as e:
                    QMessageBox.warning(None, "WARNING", str(e))

    # ==== slot methods ====

    @Slot(bool)
    def compute_hash(self, value: bool):
        self.ui.tabWidget_IODE_objs.compute_hash(value)

    @Slot()
    @Slot(IodeType)
    def update_tab_and_completer(self, iode_type: IodeType = None):
        """
        Update the tab(s) and the completer based on the iode_type.

        :param iode_type: The type of IODE object.
        """
        # compute hash + update tab(s)
        if iode_type:
            index = self.ui.tabWidget_IODE_objs.update_object_tab(iode_type)
        else:
            index = -1
            for iode_type in IodeType:
                self.ui.tabWidget_IODE_objs.update_object_tab(iode_type)

        # update the list of Iode object names available for autocomplete
        self.completer.update_iode_objects_list_names()

        # show the corresponding tab
        self.ui.tabWidget_IODE_objs.show_tab(index)

    @Slot()
    def execute_python_command(self):
        import iode
        last_input = self.lineEdit_python_command.text()
        try:
            output = eval(last_input, iode.__dict__)
            to_print = f">>> {last_input}\n"
            if output is not None and isinstance(output, str):
                to_print += output + "\n"
            self.plainTextEdit_python_output.toPlainText(to_print)
        except Exception as e:
            QMessageBox.warning(self, "WARNING", f"Could not execute '{last_input}':\n{e}")

    @Slot()
    def ipython_cell_executed(self):
        self.update_tab_and_completer()

    # File Menu

    @Slot()
    def create_new_project(self):
        # ask to save all current tabs content before to switch
        if self.project_path:
            answer = self._ask_save_all_tabs()
            if answer == QMessageBox.StandardButton.Discard:
                return

        dialog = MenuFileNewProject(self.project_path, self)
        if dialog.exec() == QDialog.DialogCode.Accepted:
            newProjectPath = dialog.get_path_new_project()
            self.open_directory(newProjectPath)

    @Slot()
    def open_project(self):
        # ask to save all current tabs content before to switch
        if self.project_path:
            answer = self._ask_save_all_tabs()
            if answer == QMessageBox.StandardButton.Discard:
                return

        dir = QFileDialog.getExistingDirectory(self, "Open Directory", self.project_path, 
                                               QFileDialog.Option.ShowDirsOnly | QFileDialog.Option.DontResolveSymlinks)
        # check if user clicked on the Cancel button
        if not dir:
            return

        # update current project path + open it in the Iode GUI File Explorer
        self.project_path = dir
        self.open_directory(self.project_path)

    @Slot()
    def open_recent_project(self):
        action = self.sender()
        if action:
            project_path = action.data()
            if QDir(project_path).exists():
                # ask to save all current tabs content before to switch
                answer = self._ask_save_all_tabs()
                if answer == QMessageBox.StandardButton.Discard:
                    return
                self.open_directory(project_path)
            else:
                QMessageBox.warning(self, "WARNING", f"Directory {project_path} seems to no longer exist")
                self.recent_projects.remove(project_path)
                self.user_settings.setValue("recent_projects", self.recent_projects)

    @Slot()
    def save_current_tab(self):
        self.ui.tabWidget_IODE_objs.save_tab()

    @Slot()
    def save_all_tabs(self):
        self.ui.tabWidget_IODE_objs.save_all_tabs()

    @Slot()
    def open_import_comments_dialog(self):
        self.compute_hash(True)

        dialog = MenuFileImportComments(self)
        if dialog.exec() == QDialog.DialogCode.Accepted:
            self.tabWidget_IODE_objs.reset_filter(IodeType.COMMENTS)
            self.update_tab_and_completer(IodeType.COMMENTS)

    @Slot()
    def open_import_variables_dialog(self):
        self.compute_hash(True)

        dialog = MenuFileImportVariables(self)
        if dialog.exec() == QDialog.DialogCode.Accepted:
            self.tabWidget_IODE_objs.reset_filter(IodeType.VARIABLES)
            self.update_tab_and_completer(IodeType.VARIABLES)

    @Slot()
    def open_export_dialog(self):
        dialog = MenuFileExport(self)
        dialog.exec()

    @Slot()
    def open_settings(self):
        dialog = MenuFileSettings(self)
        dialog.exec()

    # Workspace Menu

    @Slot()
    def clear_workspace(self):
        answer = QMessageBox.warning(self, "WARNING", "Are you sure to clear the whole workspace?", 
                                     QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No, 
                                     QMessageBox.StandardButton.No)
        if answer == QMessageBox.StandardButton.Yes:
            self.compute_hash(True)
            self.ui.tabWidget_IODE_objs.clear_workspace()
            self.update_tab_and_completer()

    @Slot()
    def open_copy_into_workspace_dialog(self):
        self.compute_hash(True)
        dialog = MenuWorkspaceCopyInto(self)
        if dialog.exec() == QDialog.DialogCode.Accepted:
            self.tabWidget_IODE_objs.reset_filters()
            self.update_tab_and_completer()

    @Slot()
    def open_merge_into_workspace_dialog(self):
        self.compute_hash(True)
        dialog = MenuWorkspaceMergeInto(self)
        if dialog.exec() == QDialog.DialogCode.Accepted:
            self.tabWidget_IODE_objs.reset_filters()
            self.update_tab_and_completer()

    @Slot()
    def open_change_workspace_descriptions_dialog(self):
        dialog = MenuWorkspaceDescribe(self)
        dialog.exec()

    @Slot()
    def open_change_variables_sample_dialog(self):
        self.compute_hash(True)
        try:
            dialog = EditIodeSampleDialog(self)
            if dialog.exec() == QDialog.DialogCode.Accepted:
                from_period = dialog.from_period
                to_period = dialog.to_period
                variables.sample = f"{from_period}:{to_period}"
                self.update_tab_and_completer(IodeType.VARIABLES)
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))

    @Slot()
    def open_extrapolate_variables_dialog(self):
        self.compute_hash(True)

        dialog = MenuWorkspaceExtrapolateVariables(self)
        if dialog.exec() == QDialog.DialogCode.Accepted:
            self.ui.tabWidget_IODE_objs.reset_filter(IodeType.VARIABLES)
            self.update_tab_and_completer(IodeType.VARIABLES)

    @Slot()
    def open_high_to_low_dialog(self):
        self._check_vars_sample()
        self.compute_hash(True)

        dialog = MenuWorkspaceHighToLow(self)
        if dialog.exec() == QDialog.DialogCode.Accepted:
            self.tabWidget_IODE_objs.reset_filter(IodeType.VARIABLES)
            self.update_tab_and_completer(IodeType.VARIABLES)

    @Slot()
    def open_low_to_high_dialog(self):
        self._check_vars_sample()
        self.compute_hash(True)

        dialog = MenuWorkspaceLowToHigh(self)
        if dialog.exec() == QDialog.DialogCode.Accepted:
            self.tabWidget_IODE_objs.reset_filter(IodeType.VARIABLES)
            self.update_tab_and_completer(IodeType.VARIABLES)

    @Slot()
    def open_seasonal_adjustment_dialog(self):
        self.compute_hash(True)

        dialog = MenuWorkspaceSeasonalAdjustment(self)
        if dialog.exec() == QDialog.DialogCode.Accepted:
            self.update_tab_and_completer()

    @Slot()
    def open_trend_correction_dialog(self):
        self.compute_hash(True)

        dialog = MenuWorkspaceTrendCorrection(self)
        if dialog.exec() == QDialog.DialogCode.Accepted:
            self.update_tab_and_completer()

    # Data Menu

    @Slot()
    def open_sort_and_calculus_dialog(self):
        self.compute_hash(True)

        dialog = MenuDataSortExpandList(self)
        if dialog.exec() == QDialog.DialogCode.Accepted:
            self.update_tab_and_completer(IodeType.LISTS)

    @Slot()
    def open_list_calculus_dialog(self):
        self.compute_hash(True)

        dialog = MenuDataListCalculus(self)
        if dialog.exec() == QDialog.DialogCode.Accepted:
            self.update_tab_and_completer(IodeType.LISTS)

    @Slot()
    def open_file_contents_dialog(self):
        self.compute_hash(True)

        dialog = MenuDataFileContents(self)
        if dialog.exec() == QDialog.DialogCode.Accepted:
            self.update_tab_and_completer(IodeType.LISTS)

    @Slot()
    def open_file_compare_dialog(self):
        self.compute_hash(True)

        dialog = MenuDataFileCompare(self)
        if dialog.exec() == QDialog.DialogCode.Accepted:
            self.update_tab_and_completer(IodeType.LISTS)

    @Slot()
    def open_search_text_dialog(self):
        self.compute_hash(True)

        dialog = MenuDataSearchText(self)
        if dialog.exec() == QDialog.DialogCode.Accepted:
            self.update_tab_and_completer(IodeType.LISTS)

    @Slot()
    def open_scan_objects_dialog(self):
        self.compute_hash(True)

        dialog = MenuDataScanObjects(self)
        if dialog.exec() == QDialog.DialogCode.Accepted:
            self.update_tab_and_completer(IodeType.LISTS)

    @Slot()
    def open_edit_tables_variables_dialog(self):
        self.compute_hash(True)

        dialog = MenuDataEditTables(self)
        dialog.exec()

    @Slot()
    def open_duplicate_objects_dialog(self):
        self.compute_hash(True)

        dialog = MenuDataDuplicateObj(self)
        if dialog.exec() == QDialog.DialogCode.Accepted:
            self.update_tab_and_completer(IodeType.LISTS)

    # Compute Menu

    @Slot()
    def open_compute_identities_dialog(self):
        self.compute_hash(True)

        dialog = MenuComputeIdentities(self)

        identities = self.ui.tabWidget_IODE_objs.get_selected_objects_names(IodeType.IDENTITIES)
        if len(identities):
            dialog.identities_names = identities

        if dialog.exec() == QDialog.DialogCode.Accepted:
            self.update_tab_and_completer()

    @Slot()
    def open_compute_simulation_dialog(self):
        self.compute_hash(True)

        dialog = MenuComputeSimulation(self)
        if dialog.exec() == QDialog.DialogCode.Accepted:
            self.update_tab_and_completer()

    @Slot()
    def open_compute_model_dialog(self):
        self.compute_hash(True)

        dialog = MenuComputeModel(self)
        if dialog.exec() == QDialog.DialogCode.Accepted:
            self.update_tab_and_completer()

    @Slot()
    def open_compute_scc_decomposition_dialog(self):
        self.compute_hash(True)

        dialog = MenuComputeSCCDecomposition(self)
        if dialog.exec() == QDialog.DialogCode.Accepted:
            self.update_tab_and_completer()

    @Slot()
    def open_compute_scc_simulation_dialog(self):
        self.compute_hash(True)

        dialog = MenuComputeSCCSimulation(self)
        if dialog.exec() == QDialog.DialogCode.Accepted:
            self.update_tab_and_completer()

    # Print/Graph Menu

    @Slot()
    def open_print_tables_dialog(self):
        dialog = MenuPrintTables(self)
        dialog.exec()

    @Slot()
    def open_print_variables_dialog(self):
        dialog = MenuPrintVariables(self)
        dialog.exec()

    @Slot()
    def open_print_objects_definitions_dialog(self):
        QMessageBox.warning(self, "WARNING", "Object Definitions not yet implemented")

    @Slot()
    def open_graphs_tables_dialog(self):
        dialog = MenuGraphTables(self)
        dialog.new_plot.connect(self.append_dialog)
        dialog.exec()

    @Slot()
    def open_graphs_variables_dialog(self):
        dialog = MenuGraphVariables(self)
        dialog.new_plot.connect(self.append_dialog)
        dialog.exec()

    @Slot(list, str, str)
    def open_graphs_variables_dialog_from_vars_view(self, variable_names: List[str], 
                                                    from_period: str, to_period: str):
        dialog = MenuGraphVariables(self)
        dialog.variables_names = variable_names
        dialog.from_period = from_period
        dialog.to_period = to_period
        dialog.new_plot.connect(self.append_dialog)
        dialog.exec()

    # Help Menu

    @Slot()
    def about(self):
        msg = f"""
<style>
* {{text-align: center;}}
</style>

<h1>Welcome to IODE</h1>
<h2>MODELLING SOFTWARE</h2>
<h2>Version {IODE_VERSION_MAJOR}.{IODE_VERSION_MINOR}.{IODE_VERSION_PATCH}</h2>

<h3>Developed by the {ORGANIZATION_NAME}</h3>
<p>
<ul> 
    <li>Geert Bryon &nbsp;&nbsp;&nbsp;&nbsp;  
    <li>Jean-Marc Paul
    <li>Alix Damman 
    <li>Yannick Van den Abbeel &nbsp;&nbsp;&nbsp;   
</ul>
</p>

<h3>Tools</h3>         
<p>SCR/AL1(c) 1986-2020, JM.B.Paul</p>

<h3>Website</h3>       
<p><a href='{URL_HOMEPAGE.toString()}'>{URL_HOMEPAGE.toString()}</a></p>
"""
        QMessageBox.about(self, "About IODE", msg)
 
    @Slot()
    def open_release_notes(self):
        QDesktopServices.openUrl(URL_CHANGELOG)

    @Slot()
    def open_python_api(self):
        QDesktopServices.openUrl(URL_PYTHON_API)

    @Slot()
    def open_iode_home(self):
        QDesktopServices.openUrl(URL_HOMEPAGE)

    @Slot()
    def open_iode_manual(self):
        QDesktopServices.openUrl(URL_MANUAL)

    @Slot()
    def display_keyboard_shortcuts(self):
        QDesktopServices.openUrl(URL_SHORTCUTS)
