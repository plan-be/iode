from PySide6.QtCore import Qt, QModelIndex, QAbstractItemModel
from PySide6.QtWidgets import QStyledItemDelegate, QWidget, QStyleOptionViewItem, QLineEdit
from .edit_table_model import EditTableModel

from iode import TableLineType

class EditTableDelegate(QStyledItemDelegate):

    def __init__(self, parent=None):
        super().__init__(parent)

    def is_editable(self, index: QModelIndex):
        table_model: EditTableModel = index.model()
        line_type = table_model.line_type(index.row())
        return line_type == TableLineType.TITLE or line_type == TableLineType.CELL

    def createEditor(self, parent: QWidget, option: QStyleOptionViewItem, index: QModelIndex):
        if self.is_editable(index):
            editor = QLineEdit(parent)
            return editor
        else:
            return None

    def setEditorData(self, editor: QWidget, index: QModelIndex):
        if self.is_editable(index):
            value = index.model().data(index, Qt.ItemDataRole.EditRole)
            lineEdit: QLineEdit = editor
            lineEdit.setText(value)

    def setModelData(self, editor: QWidget, model: QAbstractItemModel, index: QModelIndex):
        if self.is_editable(index):
            lineEdit: QLineEdit = editor
            value = lineEdit.text()
            model.setData(index, value, Qt.ItemDataRole.EditRole)
