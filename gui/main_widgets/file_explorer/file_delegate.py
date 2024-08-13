from typing import List
from pathlib import Path

from PyQt6.QtWidgets import QWidget, QStyledItemDelegate, QLineEdit, QStyleOptionViewItem, QMessageBox
from PyQt6.QtCore import QModelIndex, Qt, QObject, QAbstractItemModel
from PyQt6.QtGui import QPainter, QPalette, QColor


class FileDelegate(QStyledItemDelegate):
    """
    A custom delegate for the file explorer that provides editing functionality for file names.
    """

    def __init__(self, file_explorer, parent: QObject = None):
        """
        Initializes a new instance of the FileDelegate class.

        :param cut_indexes: A list of model indexes that have been cut.
        :param modified_indexes: A list of model indexes that have been modified.
        :param parent: The parent object for this delegate.
        """
        super().__init__(parent)
        self.file_explorer = file_explorer

    def createEditor(self, parent: QWidget, option: QStyleOptionViewItem, index: QModelIndex) -> QWidget:
        """
        Creates a new editor widget for editing the file name.

        :param parent: The parent widget for the editor.
        :param option: The style options for the editor.
        :param index: The model index for the file being edited.
        :return: A new QLineEdit widget for editing the file name.
        """
        return QLineEdit(parent)

    def setEditorData(self, editor: QWidget, index: QModelIndex) -> None:
        """
        Sets the initial data for the editor widget.

        :param editor: The editor widget.
        :param index: The model index for the file being edited.
        """
        old_name = index.model().data(index, Qt.ItemDataRole.EditRole)
        editor.setText(old_name)

    def setModelData(self, editor: QWidget, model: QAbstractItemModel, index: QModelIndex) -> None:
        """
        Sets the data for the model from the editor widget.

        :param editor: The editor widget.
        :param model: The model for the file explorer.
        :param index: The model index for the file being edited.
        """
        new_name = editor.text()
        old_name = model.data(index, Qt.ItemDataRole.DisplayRole)
        parent_dir = model.fileInfo(index).absoluteDir()
        try:
            new_file_path = Path(parent_dir.filePath(new_name))
            old_file_path = Path(parent_dir.filePath(old_name))
            if new_file_path.exists():
                answer = QMessageBox.question(self.file_explorer, "WARNING", f"File or directory "
                                              f"'{new_file_path}' already exists.\nWant to overwrite ?")
                if answer == QMessageBox.StandardButton.No:
                    new_file_path = new_file_path.with_stem(new_file_path.stem + "_copy")
            # Rename this file or directory to the given target, and return 
            # a new Path instance pointing to target. If target points to an 
            # existing file or empty directory, it will be unconditionally replaced. 
            # https://docs.python.org/3.11/library/pathlib.html#pathlib.Path.replace
            old_file_path.replace(new_file_path)    
            model.setData(index, new_name, Qt.ItemDataRole.EditRole)
        except Exception as e:
            QMessageBox.warning(None, "WARNING", f"Could not rename file:\n{e}")    

    def paint(self, painter: QPainter, option: QStyleOptionViewItem, index: QModelIndex) -> None:
        """
        Paints the item in the file explorer.

        :param painter: The painter object.
        :param option: The style options for the item.
        :param index: The model index for the file being painted.
        """
        itemOption = QStyleOptionViewItem(option)
        if index in self.file_explorer.modified_indexes:
            itemOption.palette.setColor(QPalette.ColorRole.Text, QColor(Qt.GlobalColor.blue))
        if index in self.file_explorer.cut_indexes:
            itemOption.palette.setColor(QPalette.ColorRole.Text, QColor(Qt.GlobalColor.gray))
        super().paint(painter, itemOption, index)
