import sys
import shutil
from typing import List, Tuple
from pathlib import Path

from PyQt6.QtCore import (QModelIndex, QPersistentModelIndex, QDir, QFile, QFileInfo, 
                          QPoint, Qt, QProcess, QUrl, pyqtSignal, pyqtSlot)
from PyQt6.QtGui import (QAction, QKeySequence, QShortcut, QFileSystemModel, 
                         QDesktopServices, QCloseEvent, QDragMoveEvent, QDropEvent)
from PyQt6.QtWidgets import QTreeView, QMenu, QMessageBox, QApplication

from settings import ProjectSettings
from .file_explorer_proxy import FileExplorerProxyModel
from .file_delegate import FileDelegate
from ..tab_widget.tab_widget import IodeTabWidget


class IodeFileExplorer(QTreeView):
    """
    The present class represents a File Explorer (i.e. File Tree) widget
    which lists the files and subdirectories of an IODE project.
    The File Explorer widget is visible on as a left panel in the GUI.

    Implemented features:

    - Show the root directory of an IODE project at the top of the file tree.
    - Ask user to save all modifications before to switch to another IODE project.
    - Double click on a file opens a new tab (accepted files are KDB files, IODE Report files and text files).
    - Pressing ENTER opens the selected file(s).
    - It is possible to create/delete/copy/rename files and/or directories from a context menu (or via shortcuts):
       -> CTRL + N creates a new file.
       -> CTRL + SHIFT + N creates a new directory.
       -> CTRL + X cuts the selected file(s)/directory(ies).
       -> CTRL + C copies the selected file(s)/directory(ies).
       -> CTRL + P pastes file(s)/directory(ies).
       -> SHIFT + ALT + C copies the absolute filepath.
       -> SHIFT + ALT + R reveals file in OS file explorer.
       -> F2 puts file/directory in edit mode (rename).
       -> Del deletes the selected  file(s)/directory(ies).
    - It is possible to move files and/or directories inside the project tree via drag and drop.
    """

    fileMoved = pyqtSignal(str, str)

    def __init__(self, parent=None):
        super().__init__(parent)
        
        self.file_system_model = QFileSystemModel(self)
        self.proxy_model = FileExplorerProxyModel(self)
        self.proxy_model.setSourceModel(self.file_system_model)

        # set model editable
        self.file_system_model.setReadOnly(False)

        # attach model to view
        self.setModel(self.proxy_model)

        # show only Name column (not Size and Date Modified columns)
        for i in range(1, self.file_system_model.columnCount()):
            self.hideColumn(i)

        # delegate (for renaming files and directories)
        self.setItemDelegate(FileDelegate(self, parent))

        # selection -> see main_window.ui
        # Selection mode = ExtendedSelection which means:
        # When the user selects an item in the usual way, the selection is cleared 
        # and the new item selected. However, if the user presses the Ctrl key when clicking on an item, 
        # the clicked item gets toggled and all other items are left untouched. 
        # If the user presses the Shift key while clicking on an item, all items between the current item 
        # and the clicked item are selected or unselected, depending on the state of the clicked item. 
        # Multiple items can be selected by dragging the mouse over them.

        # drag and drop -> see main_window.ui
        # - showDropIndicator = true which means the drop indicator is shown when dragging items and dropping.
        # - dragEnabled = true which means the view (File Explorer) supports dragging of its own items.
        # - acceptDrops = true which means the view (File Explorer) accepts drop of items (file or directory)
        # - dragDropOverwriteMode = true which means the selected data will overwrite the existing item data 
        #   when dropped, while moving the data will clear the item.
        # - dragDropMode = InternalMove which means the view (File Explorer) accepts move (not copy) operations 
        #   only from itself.
        # - defaultDropAction = MoveAction which means the data is moved from the source to the target.
        # More details on https://doc.qt.io/qt-6/model-view-programming.html#using-drag-and-drop-with-item-views

        # prepare shortcuts
        self.new_file_shortcut = QShortcut(QKeySequence.StandardKey.New, self)
        self.new_directory_shortcut = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Modifier.SHIFT | Qt.Key.Key_N), self)
        self.cut_shortcut = QShortcut(QKeySequence.StandardKey.Cut, self)
        self.copy_shortcut = QShortcut(QKeySequence.StandardKey.Copy, self)
        self.paste_shortcut = QShortcut(QKeySequence.StandardKey.Paste, self)
        self.filepath_shortcut = QShortcut(QKeySequence(Qt.Modifier.SHIFT | Qt.Modifier.ALT | Qt.Key.Key_C), self)
        self.reveal_explorer_shortcut = QShortcut(QKeySequence(Qt.Modifier.SHIFT | Qt.Modifier.ALT | Qt.Key.Key_R), self)
        self.rename_shortcut = QShortcut(QKeySequence(Qt.Key.Key_F2), self)
        self.delete_shortcut = QShortcut(QKeySequence.StandardKey.Delete, self)
        self.enter_shortcut = QShortcut(QKeySequence(Qt.Key.Key_Enter), self)
        self.cancel_shortcut = QShortcut(QKeySequence.StandardKey.Cancel, self)

        # set context
        self.new_file_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.new_directory_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.cut_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.copy_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.paste_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.filepath_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.reveal_explorer_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.rename_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.delete_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.enter_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.cancel_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)

        # signals and slots
        self.doubleClicked.connect(self._open_file)
        self.customContextMenuRequested.connect(self.onCustomContextMenu)

        self.new_file_shortcut.activated.connect(self.create_file)
        self.new_directory_shortcut.activated.connect(self.create_dir)
        self.cut_shortcut.activated.connect(self.cut)
        self.copy_shortcut.activated.connect(self.copy)
        self.paste_shortcut.activated.connect(self.paste)
        self.filepath_shortcut.activated.connect(self.absolute_path)
        self.reveal_explorer_shortcut.activated.connect(self.reveal_in_folder)
        self.rename_shortcut.activated.connect(self.rename)
        self.delete_shortcut.activated.connect(self.remove)
        self.enter_shortcut.activated.connect(lambda: self.open_files(False))
        self.cancel_shortcut.activated.connect(self.cancel)

        # context menu setup
        self.setContextMenuPolicy(Qt.ContextMenuPolicy.CustomContextMenu)
        self._setup_context_menu()

        # miscellaneous
        self.project_dir: QDir = None
        self.items_to_past: List[QFileInfo] = []
        self.cut_indexes: List[QModelIndex] = []
        self.modified_indexes: List[QModelIndex] = []
        self.items_to_past: List[Tuple[QFileInfo, bool]] = []
        self.called_from_context_menu = False
        self.index_context_menu: QModelIndex = None

    def _add_action(self, name: str, tooltip: str, context_menu: QMenu, slot_method, shortcut: QShortcut=None) -> QAction:
        """
        Add an action to the context menu.

        :param name: The name of the action.
        :param tooltip: The tooltip of the action.
        :param context_menu: The context menu to which the action will be added.
        :param slot_method: The slot method to be called when the action is triggered.
        :param shortcut: The shortcut of the action.
        """
        action = QAction(name, self)
        action.setToolTip(tooltip)
        action.setVisible(True)
        if shortcut:
            action.setShortcut(shortcut.key())
        context_menu.addAction(action)
        action.triggered.connect(slot_method)
        return action

    def _setup_context_menu(self):
        """
        Set up the context menu for the file explorer.
        """
        # Add actions to the context menus
        # ---- directory context menu ----
        self.context_menu_directory = QMenu(self)
        self._add_action("New File", "Add new file", self.context_menu_directory, self.create_file, self.new_file_shortcut)
        self._add_action("New Folder", "Add new subdirectory", self.context_menu_directory, self.create_dir, self.new_directory_shortcut)
        self.context_menu_directory.addSeparator()
        self._add_action("Cut", "Cut Directory", self.context_menu_directory, self.cut, self.cut_shortcut)
        self._add_action("Copy", "Copy Directory", self.context_menu_directory, self.copy, self.copy_shortcut)
        self.paste_action_directory = self._add_action("Paste", "Paste", self.context_menu_directory, self.paste, self.paste_shortcut)
        self.context_menu_directory.addSeparator()
        self._add_action("Copy Absolute Path", "Copy Absolute Path To The Clipboard", self.context_menu_directory, self.absolute_path, self.filepath_shortcut)
        self._add_action("Copy Relative Path", "Copy Relative Path To The Clipboard", self.context_menu_directory, self.relative_path)
        self._add_action("Reveal in File Explorer", "open an OS file explorer and highlights the selected file", self.context_menu_directory, 
                         self.reveal_in_folder, self.reveal_explorer_shortcut)
        self.context_menu_directory.addSeparator()
        self._add_action("Rename", "Rename Directory", self.context_menu_directory, self.rename, self.rename_shortcut)
        self._add_action("Delete", "Delete Directory", self.context_menu_directory, self.remove, self.delete_shortcut)

        # ---- file context menu ----
        self.context_menu_file = QMenu(self)
        self._add_action("Open", "Open File In A New Tab", self.context_menu_file, self.open_files)
        self._add_action("Open As Text", "Open File As Text In A New Tab", self.context_menu_file, lambda: self.open_files(True))
        self.context_menu_file.addSeparator()
        self._add_action("Cut", "Cut File", self.context_menu_file, self.cut, self.cut_shortcut)
        self._add_action("Copy", "Copy File", self.context_menu_file, self.copy, self.copy_shortcut)
        self.context_menu_file.addSeparator()
        self._add_action("Copy Absolute Path", "Copy Absolute Path To The Clipboard", self.context_menu_file, self.absolute_path, self.filepath_shortcut)
        self._add_action("Copy Relative Path", "Copy Relative Path To The Clipboard", self.context_menu_file, self.relative_path)
        self._add_action("Reveal in File Explorer", "open an OS file explorer and highlights the selected file", self.context_menu_file, 
                         self.reveal_in_folder, self.reveal_explorer_shortcut)
        self.context_menu_file.addSeparator()
        self._add_action("Rename", "Rename Directory", self.context_menu_file, self.rename, self.rename_shortcut)
        self._add_action("Delete", "Delete Directory", self.context_menu_file, self.remove, self.delete_shortcut)

        # ---- file explorer menu (when a user clicks outside range of directories and files) ----
        self.context_menu_explorer = QMenu(self)
        self._add_action("New File", "Add new file to project", self.context_menu_explorer, self.create_file, self.new_file_shortcut)
        self._add_action("New Folder", "Add new directory to project", self.context_menu_explorer, self.create_dir, self.new_directory_shortcut)
        self.context_menu_explorer.addSeparator()
        self.paste_action_explorer = self._add_action("Paste", "Paste", self.context_menu_explorer, self.paste, self.paste_shortcut)

        self._disable_paste()

    def _enable_paste(self):
        """Enable the paste actions."""
        self.paste_action_directory.setEnabled(True)
        self.paste_action_explorer.setEnabled(True)

    def _disable_paste(self):
        """Disable the paste actions."""
        self.paste_action_directory.setEnabled(False)
        self.paste_action_explorer.setEnabled(False)

    def _extract_list_of_items(self, cut=False):
        """
        Extracts a list of items (files and directories) to cut, copy or delete.

        :param cut: Whether or not to delete items after copy.
        :return: A list of SystemItem objects.
        """
        items: List[Tuple[QFileInfo, bool]] = []
        self.cut_indexes.clear()

        # get list of currently selected files and directories in the file explorer
        selected_indexes = self.selectedIndexes()

        # case 1: the user right clicked outside the range of selected files and directories
        # -> only the item (file or directory) on which the user right clicked will be cut, copied or deleted
        if self.index_context_menu.isValid() and self.index_context_menu not in selected_indexes:
            items.append((self.proxy_model.fileInfo(self.index_context_menu), cut))
            if cut:
                self.cut_indexes.append(self.index_context_menu)
        
        # case 2 : the user used a keyboard shortcut (indexContextMenu not valid -> no context menu).
        # case 3 : the user right clicked on one of the selected files and directories
        #          (indexContextMenu is in the selected_indexes list)
        # -> all selected items will be cut, copied or deleted
        else:
            for index in selected_indexes:
                items.append((self.proxy_model.fileInfo(index), cut))
                if cut:
                    self.cut_indexes.append(index)

        return items

    def _get_destination_dir(self, index_drop: QModelIndex=None) -> QDir:
        """
        Guess the destination directory (when pasting or creating file(s) or directory(ies)).
        There are 3 possible cases in which self method is called:
        - context menu: the user right clicked -> displaying a context menu,
        - drag and drop: the user moves file(s) or directory(ies) using the drag and drop,
        - keyboard shortcuts: the user pressed a keyboard shortcut to create / paste file(s) or directory(ies).

        :param index_drop: QModelIndex index representing where to drop the file(s)/directory(ies).
        :return: QDir destination directory.
        """
        index = None

        # case 1 (context menu): the user right clicked -> displaying a context menu
        if self.called_from_context_menu:
            # case a: the user right clicked on the destination directory
            if self.index_context_menu.isValid():
                index = self.index_context_menu
            # case b: the user right clicked below the file tree
            #         -> destination directory = project directory
            else:
                if self.project_dir is None:
                    QMessageBox.warning(self, "WARNING", "The project directory could not be found")
                return self.project_dir

        # case 2 (drag and drop): the user moves file(s) or directory(ies) using the drag and drop
        elif index_drop is not None and index_drop.isValid():
            index = index_drop

        # case 3 (keyboard shortcuts): the user pressed a keyboard shortcut to create / paste file(s) or directory(ies)
        else:
            # in the case of keyboard shortcuts, we check the selected items in the file tree
            selected_indexes = self.selectedIndexes()
            # case a: one or several items are selected when pressing the keyboard shortcut -> take the first selected
            #         -> same behavior as in Visual Studio Code
            if len(selected_indexes):
                index = selected_indexes[0]
            # case b: no item(s) selected -> assume that the destination directory is the project directory
            else:
                if self.project_dir is None:
                    QMessageBox.warning(self, "WARNING", "The project directory could not be found")
                return self.project_dir

        if index is None:
            QMessageBox.warning(self, "WARNING", "Could not determine the destination directory.")
            return None

        try:
            fileInfo = self.proxy_model.fileInfo(index)
            # the index points to a directory -> destination directory
            if fileInfo.isDir():
                return QDir(fileInfo.absoluteFilePath())
            # the index points to a file -> destination directory = parent directory of the file
            else:
                return fileInfo.absoluteDir()
        except Exception as e:
            QMessageBox.warning(self, "WARNING", "Could not determine the destination directory.")
            return None

    def _cleanup_slot(self):
        """
        to be called after each call to a slot method.
        """
        self.called_from_context_menu = False
        self.index_context_menu = QModelIndex()

    def load_settings(self):
        """
        Load directories that were expanded when the program has been closed.
        """
        project_settings = ProjectSettings.project_settings
        if not project_settings:
            return

        # get list of expanded directories at the moment the application was closed
        project_settings.beginGroup("FILE_EXPLORER")
        dirs_to_expand = project_settings.value("expanded_dirs")
        project_settings.endGroup()
        if dirs_to_expand is None or not isinstance(dirs_to_expand, str):
            return

        # expand directories in file explorer (if they still exist)
        dir_paths = [dir_info.absoluteFilePath() for dir_info in 
                     self.project_dir.entryInfoList(QDir.Filter.Dirs | QDir.Filter.NoDotAndDotDot)]
        dirs_to_expand = dirs_to_expand.split(",")
        for dir_to_expand in dirs_to_expand:
            if dir_to_expand in dir_paths:
                index = self.proxy_model.index_from_path(dir_to_expand)
                self.expand(index)

    def save_settings(self):
        """
        Save directories that are expanded before to close the program.
        """
        project_settings = ProjectSettings.project_settings
        if not project_settings:
            return

        # build list of expanded directories
        dir_paths = [dir_info.absoluteFilePath() for dir_info in 
                     self.project_dir.entryInfoList(QDir.Filter.Dirs | QDir.Filter.NoDotAndDotDot)]
        expanded_dirs = []
        for dir_path in dir_paths:
            index = self.proxy_model.index_from_path(dir_path)
            if self.isExpanded(index):
                expanded_dirs.append(dir_path)

        # save list of expanded directories to project settings
        project_settings.beginGroup("FILE_EXPLORER")
        project_settings.setValue("expanded_dirs", ",".join(expanded_dirs))
        project_settings.endGroup()

    def set_iode_tab_widget(self, tab_widget: IodeTabWidget):
        """
        Set the IodeTabWidget.

        :param tabWidget: The IodeTabWidget to set.
        """
        self.tab_widget = tab_widget
        # to update filepath, name and tooltip of corresponding tab when a file is moved
        self.fileMoved.connect(self.tab_widget.file_moved)
        # to update filepath, name and tooltip of corresponding tab when a file is renamed
        self.file_system_model.fileRenamed.connect(self.tab_widget.file_renamed)
        # to set corresponding file in color when its content is modified (and not yet saved to file)
        self.tab_widget.fileContentModified.connect(self.file_content_modified)

    def update_project_dir(self, project_dir: QDir, onStartup: bool=False):
        """
        Update the project path and the content of the File Explorer.

        :param project_dir: Directory containing the IODE project to open.
        :param onStartup: Whether or not the method is called on startup.
        """
        self.project_dir = project_dir

        if not onStartup:
            self.save_settings()

        # Notes: - see https://doc.qt.io/qt-6/qtwidgets-itemviews-dirview-example.html
        #        - We need the path to the parent directory of the project directory to be able to display
        #          the project directory as the root directory.
        #          see https://stackoverflow.com/questions/53430989/pyside-qfilesystemmodel-display-show-root-item
        project_path = project_dir.absolutePath()
        parent_path = QFileInfo(project_path).dir().absolutePath()

        self.file_system_model.setRootPath(parent_path)
        project_index = self.file_system_model.index(project_path)

        # Warning: the line below shifts all indexes for the Tree view.
        #          The index of the project (root) directory is now (0, 0).
        #          From now on, we must always use proxyModel methods.
        self.proxy_model.set_project_index(QPersistentModelIndex(project_index))
        parent_index = self.proxy_model.index_from_path(parent_path)

        self.setRootIndex(parent_index)

        # shifts project_index to be compatible with the new indexes of the Tree view
        project_index = self.proxy_model.mapFromSource(project_index)

        # expand the (root) project directory
        self.expand(project_index)

        # update project in the Tab widget
        self.tab_widget.update_project_dir(project_path)

        self.load_settings()

        self.called_from_context_menu = False
        self.index_context_menu = QModelIndex()

        self.show()

    # override method of QTreeView
    @pyqtSlot(QDragMoveEvent)
    def dragMoveEvent(self, event: QDragMoveEvent):
        try:
            # Check if the event has text data and if the mouse is over the widget
            if event.mimeData().hasText() and self.geometry().contains(event.position().toPoint()):
                super(IodeFileExplorer, self).dragMoveEvent(event)
                event.setDropAction(Qt.DropAction.MoveAction)
                event.acceptProposedAction()
            else:
                event.ignore()
        except Exception as e:
            QMessageBox.warning(self, "WARNING", "An error occurred while moving "
                                f"file(s) or directory(ies): {str(e)}")
            event.ignore()

    # override method of QTreeView
    @pyqtSlot(QDropEvent)
    def dropEvent(self, event: QDropEvent):
        # Check if the event has text data
        if event.mimeData().hasText():
            try:
                # Get the destination directory where the new file will be created
                index_drop = self.indexAt(event.position().toPoint())
                drop_dir = self._get_destination_dir(index_drop)
                drop_dir = Path(drop_dir.absolutePath())

                # Build list of items (files and directories) to drop
                mime_data_text = event.mimeData().text()
                mime_data_text = mime_data_text.replace("file:///", "")
                paths = mime_data_text.split("\n")

                # Move items (files and directories)
                for src_path in paths:
                    src_path = Path(src_path).resolve()
                    dst_path = drop_dir / src_path.name
                    if dst_path.exists():
                        answer = QMessageBox.question(self, "WARNING", f"File or directory "
                                                    f"'{dst_path}' already exists.\nWant to overwrite ?")
                        if answer == QMessageBox.StandardButton.No:
                            dst_path = dst_path.with_stem(dst_path.stem + "_copy")
                    src_path.replace(dst_path)
                    self.fileMoved.emit(str(src_path), str(dst_path))

                event.accept()
            except Exception as e:
                QMessageBox.warning(self, "WARNING", "An error occurred while moving "
                                    f"file(s) or directory(ies): {str(e)}")
                event.ignore()
        else:
            event.ignore()

    # override method of QTreeView
    @pyqtSlot(QPoint)
    def onCustomContextMenu(self, point: QPoint):
        """
        Pops up either directory or file context menu depending on the item selected.

        A context Menu is a menu displayed when the user right clicks somewhere.
        See Qt documentation of onCustomContextMenu.
        Overridden method.

        :param point: Position where the user right clicked
        """
        self.called_from_context_menu = True
        global_point = self.viewport().mapToGlobal(point)

        self.index_context_menu = self.indexAt(point)
        if self.index_context_menu.isValid():
            file_info = self.proxy_model.fileInfo(self.index_context_menu)
            context_menu_current = self.context_menu_directory if file_info.isDir() else self.context_menu_file
        else:
            context_menu_current = self.context_menu_explorer

        context_menu_current.exec(global_point)

    @pyqtSlot(QModelIndex)
    def _open_file(self, index: QModelIndex):
        """
        Function called when the user double clicked on a item in the tree (= file explorer) view.

        :param index: Index (row and column) of the item selected.
        """
        try:
            # do nothing if user double clicked on a folder
            if self.proxy_model.isDir(index):
                return

            # get path to the file on which the user double clicked
            filepath = self.proxy_model.filePath(index)

            # load the file
            self.tab_widget.load_file(filepath)
        except Exception as e:
            QMessageBox.warning(self, "WARNING", "An error occurred while trying to "
                                f"open a file: {str(e)}") 

    @pyqtSlot(str, bool)
    def file_content_modified(self, filepath: str, modified: bool):
        """
        Set item in color when a user modifies the content of the associated database
        or text/report file in tabs.

        :param filepath: The path of the file
        :param modified: True if the file is modified, False otherwise
        """
        index = self.proxy_model.index_from_path(filepath)
        if not index.isValid():
            return

        if modified:
            self.modified_indexes.append(index)
        else:
            self.modified_indexes.remove(index)

        self.viewport().repaint()

    @pyqtSlot()
    def create_file(self):
        """
        Create a new file.
        """
        try:
            parent_dir = self._get_destination_dir()
            new_filepath = parent_dir.filePath("new_file")
            new_file = QFile(new_filepath)
            if new_file.open(QFile.OpenModeFlag.ReadWrite):
                new_file.close()
                new_file_index = self.proxy_model.index_from_path(new_filepath)
                # ask user to set a filename
                self.edit(new_file_index)
                # user may have pushed escape to discard creating new file
                # if self.proxy_model.fileName(new_file_index) == "newfile":
                #     self.proxy_model.remove(new_file_index)
            self._cleanup_slot()
        except Exception as e:
            QMessageBox.warning(self, "WARNING", "An error occurred while trying to "
                                f"create a file: {str(e)}") 

    @pyqtSlot()
    def create_dir(self):
        """
        Create a new sub directory.
        """
        try:
            parent_dir = self._get_destination_dir()
            if parent_dir.mkdir("new_folder"):
                new_dir_path = parent_dir.filePath("new_folder")
                new_dir_index = self.proxy_model.index_from_path(new_dir_path)
                # ask user to set a directory name
                self.edit(new_dir_index)
                # user may have pushed escape to discard creating new file
                # if self.proxy_model.fileName(new_dir_index) == "new_folder":
                #     self.proxy_model.remove(new_dir_index)
                self._cleanup_slot()
        except Exception as e:
            QMessageBox.warning(self, "WARNING", "An error occurred while trying to "
                                f"create a directory: {str(e)}")            

    @pyqtSlot()
    def absolute_path(self):
        """
        Copy the absolute path of the selected path to the selected file.
        """
        clipboard = QApplication.clipboard()
        file_info = self.proxy_model.fileInfo(self.index_context_menu)
        abs_path = file_info.absoluteFilePath()
        clipboard.setText(abs_path)
        self._cleanup_slot()

    @pyqtSlot()
    def relative_path(self):
        """
        Copy the relative path of the selected path to the selected file.
        """
        clipboard = QApplication.clipboard()
        file_info = self.proxy_model.fileInfo(self.index_context_menu)
        rel_path = self.project_dir.relativeFilePath(file_info.absoluteFilePath())
        clipboard.setText(rel_path)
        self._cleanup_slot()

    @pyqtSlot()
    def reveal_in_folder(self):
        """
        Open an OS file explorer and highlight the selected file.
        If several files selected, reveal only the last one (as in Visual Studio Code).
        """
        selectedItems = self._extract_list_of_items()
        if not selectedItems:
            return
        item = selectedItems[-1]
        file_info = item[0]
        path = file_info.absoluteFilePath()
        if sys.platform.startswith('win'):
            args = []
            if not file_info.isDir():
                args.append("/select,")
            args.append(QDir.toNativeSeparators(path))
            if QProcess.startDetached("explorer", args):
                return
        elif sys.platform.startswith('darwin'):
            args = ["-e", "tell application \"Finder\"", "-e", "activate", "-e", f"select POSIX file \"{path}\"", "-e", "end tell", "-e", "return"]
            if not QProcess.execute("/usr/bin/osascript", args):
                return
        else:
            QDesktopServices.openUrl(QUrl.fromLocalFile(file_info.path() if file_info.isDir() else path))
        self.selectionModel().clearSelection()
        self._cleanup_slot()

    @pyqtSlot()
    def cut(self):
        """
        Cut file or directory.
        """
        try:
            self.items_to_past = self._extract_list_of_items(True)
            self._enable_paste()
            self._cleanup_slot()
        except Exception as e:
            QMessageBox.warning(self, "WARNING", "An error occurred while trying to cut "
                                f"and paste file or directory: {str(e)}")

    @pyqtSlot()
    def copy(self):
        """
        Copy file or directory.
        """
        try:
            self.items_to_past = self._extract_list_of_items()
            self._enable_paste()
            self._cleanup_slot()
        except Exception as e:
            QMessageBox.warning(self, "WARNING", "An error occurred while trying to "
                                f"copy/paste file or directory: {str(e)}")

    @pyqtSlot()
    def paste(self):
        """
        Paste file or directory.
        """
        try:
            destination_dir = self._get_destination_dir()
            if destination_dir is None:
                QMessageBox.warning(self, "WARNING", "No destination directory selected.")
                return

            # get the destination directory in which the new directory will be created
            destination_dir = Path(destination_dir.absolutePath())
            
            # paste directory or file
            for file_info, cut in self.items_to_past:
                src_path = Path(file_info.absoluteFilePath())
                dst_path = destination_dir / src_path.name
                if dst_path.exists():
                    answer = QMessageBox.question(self, "WARNING", f"File or directory "
                                                f"'{dst_path}' already exists.\nWant to overwrite ?")
                    if answer == QMessageBox.StandardButton.No:
                        dst_path = dst_path.with_stem(dst_path.stem + "_copy")
                if cut:
                    # Rename this file or directory to the given target, and return 
                    # a new Path instance pointing to target. If target points to an 
                    # existing file or empty directory, it will be unconditionally replaced. 
                    # https://docs.python.org/3.11/library/pathlib.html#pathlib.Path.replace
                    src_path.replace(dst_path)
                else:
                    if src_path.is_dir():
                        shutil.copytree(src_path, dst_path, dirs_exist_ok=True)
                    else:
                        # If destination specifies a file that already exists, 
                        # it will be replaced. 
                        # https://docs.python.org/3.11/library/shutil.html#shutil.copy
                        shutil.copy(src_path, dst_path)
                self.fileMoved.emit(str(src_path), str(dst_path))
            
            # cleanup everything
            self._disable_paste()
            self.items_to_past.clear()
            self.cut_indexes.clear()
            self.selectionModel().clearSelection()
            self._cleanup_slot()
        except Exception as e:
            QMessageBox.warning(self, "WARNING", f"An error occurred while pasting file or directory: {str(e)}")

    @pyqtSlot()
    def rename(self):
        """
        Rename file or directory.
        """
        try:
            # rename activated via context menu
            if self.index_context_menu.isValid():
                index_rename = self.index_context_menu
            # rename activated via shortcut
            else:
                selected_indexes = self.selectedIndexes()
                # no selected items -> do nothing
                if len(selected_indexes) == 0:
                    return
                # rename the last selected index
                index_rename = selected_indexes[-1]
            self.edit(index_rename)
            self._cleanup_slot()
        except Exception as e:
            QMessageBox.warning(self, "WARNING", f"An error occurred while renaming file or directory: {str(e)}")
    
    @pyqtSlot()
    def remove(self):
        """
        Delete file or directory.
        """
        try:
            items_to_delete = self._extract_list_of_items()
            if len(items_to_delete) > 0:
                msg = "Are you sure to delete <item> ?"
                arg = ""
                if len(items_to_delete) == 1:
                    file_info, _ = items_to_delete[0]
                    filename = file_info.fileName()
                    if file_info.isDir():
                        arg = f"the directory '{filename}' and its content"
                    else:
                        arg = f"the file '{filename}'"
                else:
                    at_least_one_file = False
                    at_least_one_directory = False
                    filenames = []
                    for file_info, _ in items_to_delete:
                        filenames.append(file_info.fileName())
                        if file_info.isFile():
                            at_least_one_file = True
                        if file_info.isDir():
                            at_least_one_directory = True
                    arg = f"the following {len(filenames)} "
                    if at_least_one_file:
                        arg += "files"
                    if at_least_one_file and at_least_one_directory:
                        arg += "/"
                    if at_least_one_directory:
                        arg += "directories and their content"
                    arg += ":\n\n"
                    arg += "\n".join(filenames)
                    arg += "\n\n"

                answer = QMessageBox.warning(None, "WARNING", msg.replace("<item>", arg), 
                                            QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No, QMessageBox.StandardButton.No)
                if answer == QMessageBox.StandardButton.Yes:
                    for item, _ in items_to_delete:
                        item = Path(item.absoluteFilePath())
                        if item.is_dir():
                            shutil.rmtree(item)
                        else:
                            item.unlink()

            self.selectionModel().clearSelection()
            self._cleanup_slot()
        except Exception as e:
            QMessageBox.warning(self, "WARNING", f"An error occurred while removing file or directory: {str(e)}")

    @pyqtSlot(bool)
    def open_files(self, force_as_text: bool = False):
        """
        Function called when the user double clicks on a item in the tree (= file explorer) view.

        :param force_as_text: Forces non IODE database or report files to be open as text tabs
        """
        try:
            if self.hasFocus():
                files_to_open = self._extract_list_of_items()
                for file, _ in files_to_open:
                    file = Path(file.absoluteFilePath())
                    if file.is_file():
                        self.tab_widget.load_file(str(file), True, False, -1, force_as_text)
                self.selectionModel().clearSelection()
                self._cleanup_slot()
        except Exception as e:
            QMessageBox.warning(self, "WARNING", f"An error occurred while opening file(s): {str(e)}")

    @pyqtSlot()
    def cancel(self):
        """
        Deselects all items + clear Clipboard.
        """
        if self.hasFocus():
            self._disable_paste()
            self.items_to_past.clear()
            self.cut_indexes.clear()
            self.selectionModel().clearSelection()
            self._cleanup_slot()
