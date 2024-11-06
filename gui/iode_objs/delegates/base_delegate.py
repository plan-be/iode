from PySide6.QtWidgets import (QStyledItemDelegate, QLineEdit, QPlainTextEdit, 
                             QWidget, QStyleOptionViewItem)
from PySide6.QtCore import Qt, QEvent, QObject, QModelIndex, QAbstractItemModel
from PySide6.QtGui import QKeyEvent

from iode import IodeType


class BaseDelegate(QStyledItemDelegate):
    """
    A base delegate class for handling item data in a model.
    """

    def __init__(self, iode_type: IodeType, parent=None):
        """
        Initializes the BaseDelegate with the given IodeType and parent.

        :param iode_type: The IodeType to use.
        :param parent: The parent object.
        """
        super().__init__(parent)
        self.iode_type: IodeType = iode_type
        self.lineEdit: bool = iode_type == IodeType.SCALARS or iode_type == IodeType.VARIABLES

    # override QStyledItemDelegate method
    def createEditor(self, parent: QWidget, option: QStyleOptionViewItem, index: QModelIndex):
        """
        Creates an editor widget for the given index.

        :param parent: The parent widget.
        :param option: The style option.
        :param index: The model index.
        :return: The editor widget.
        """
        if self.lineEdit:
            editor = QLineEdit(parent)
        else:
            editor = QPlainTextEdit(parent)
        return editor

    # override QStyledItemDelegate method
    def setEditorData(self, editor: QWidget, index: QModelIndex):
        """
        Sets the data for the editor widget.

        :param editor: The editor widget.
        :param index: The model index.
        """
        value = index.model().data(index, Qt.ItemDataRole.EditRole)
        if not value:
            return
        
        if self.lineEdit:
            editor.setText(value)
        else:
            editor.setPlainText(value)

    # override QStyledItemDelegate method
    def setModelData(self, editor: QWidget, model: QAbstractItemModel, index: QModelIndex):
        """
        Sets the data for the model from the editor widget.

        :param editor: The editor widget.
        :param model: The model.
        :param index: The model index.
        """
        if self.lineEdit:
            value = editor.text()
        else:
            value = editor.toPlainText()
        model.setData(index, value, Qt.ItemDataRole.EditRole)

    # override QStyledItemDelegate method
    def eventFilter(self, editor: QObject, event: QEvent):
        """
        Filters the events for the editor widget.

        :param editor: The editor widget.
        :param event: The event.
        :return: True if the event was handled, False otherwise.
        """
        if event.type() == QEvent.Type.KeyPress:
            keyEvent = QKeyEvent(event)
            if keyEvent.key() == Qt.Key.Key_Enter or keyEvent.key() == Qt.Key.Key_Return:
                # commit changes and close editor
                if keyEvent.modifiers() == Qt.KeyboardModifier.NoModifier:
                    self.commitData.emit(editor)
                    self.closeEditor.emit(editor)
                    return True
                # ALT + (ENTER or RETURN) -> starts a new line in the editor
                elif not self.lineEdit and keyEvent.modifiers() == Qt.KeyboardModifier.AltModifier:
                    editor.insertPlainText("\n")
                    return True
        return super().eventFilter(editor, event)
