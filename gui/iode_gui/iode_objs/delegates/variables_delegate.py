from .base_delegate import BaseDelegate
from iode_gui.util.double_validator import IodeDoubleValidator

from PySide6.QtCore import QModelIndex
from PySide6.QtWidgets import QLineEdit, QWidget, QStyleOptionViewItem

from iode import IodeType


class VariablesDelegate(BaseDelegate):
    """
    A delegate for handling variables in a model-view structure.

    This delegate creates a QLineEdit widget for editing variables,
    and sets a validator to ensure that the input is a double.
    """

    def __init__(self, parent=None):
        """
        Initializes a new instance of the VariablesDelegate class.

        :param parent: The parent object of this VariablesDelegate instance.
        """
        super().__init__(IodeType.VARIABLES, parent)

    # override BaseDelegate method
    def createEditor(self, parent: QWidget, option: QStyleOptionViewItem, index: QModelIndex):
        """
        Creates a QLineEdit widget for editing variables.

        :param parent: The parent widget for the editor.
        :param option: The style options for the editor.
        :param index: The model index of the item being edited.
        :return: The QLineEdit widget for editing variables.
        """
        editor = QLineEdit(parent)
        editor.setValidator(IodeDoubleValidator(parent=editor))
        return editor
