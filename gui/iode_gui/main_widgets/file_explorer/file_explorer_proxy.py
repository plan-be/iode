from qtpy.QtCore import QPersistentModelIndex, QSortFilterProxyModel, QModelIndex, QFileInfo


class FileExplorerProxyModel(QSortFilterProxyModel):
    """
    This class is required to show the root directory of the current project as top directory in the QTreeView.

    Note: see https://stackoverflow.com/questions/53430989/pyside-qfilesystemmodel-display-show-root-item
    """

    def __init__(self, parent=None):
        super().__init__(parent)
        self.project_index = QPersistentModelIndex()

    def set_project_index(self, project_index: QPersistentModelIndex):
        """
        Set the project index.

        :param project_index: The project index.
        """
        self.project_index = project_index
        self.invalidateFilter()

    def filterAcceptsRow(self, source_row: int, source_parent: QModelIndex) -> bool:
        """
        Filter the rows to show only the project directory and its subdirectories.

        :param source_row: The row number in the source model.
        :param source_parent: The parent index in the source model.
        :return: True if the row should be shown, False otherwise.
        """
        if not self.project_index.isValid():
            return False
        index = self.sourceModel().index(source_row, 0, source_parent)
        # hide all folders at the same level of the project directory
        if source_parent == self.project_index.parent():
            return index == self.project_index
        else:
            file_info = self.sourceModel().fileInfo(index)
            if file_info.suffix() == "ini":
                return False
            else:
                return super().filterAcceptsRow(source_row, source_parent)

    def index_from_path(self, path: str, column: int=0):
        """
        Get the index of the given path.

        :param path: The path.
        :param column: The column number.
        :return: The index.
        """
        return self.mapFromSource(self.sourceModel().index(path, column))

    def isDir(self, proxy_index: QModelIndex):
        """
        Check if the given index is a directory.

        :param proxy_index: The index in the proxy model.
        :return: True if the index is a directory, False otherwise.
        """
        return self.sourceModel().isDir(self.mapToSource(proxy_index))

    def filePath(self, proxy_index: QModelIndex):
        """
        Get the file path of the given index.

        :param proxy_index: The index in the proxy model.
        :return: The file path.
        """
        return self.sourceModel().filePath(self.mapToSource(proxy_index))

    def fileInfo(self, proxy_index: QModelIndex) -> QFileInfo:
        """
        Get the file info of the given index.

        :param proxy_index: The index in the proxy model.
        :return: The file info.
        """
        return self.sourceModel().fileInfo(self.mapToSource(proxy_index))
