from .base_delegate import BaseDelegate
from iode_gui.util.double_validator import IodeDoubleValidator

from PySide6.QtCore import QModelIndex, QLocale
from PySide6.QtWidgets import QLineEdit, QWidget, QStyleOptionViewItem
from PySide6.QtGui import QDoubleValidator

from iode import IodeType


class ScalarsDelegate(BaseDelegate):
    """
    A delegate for handling scalar values in a view.
    """

    def __init__(self, parent=None):
        """
        Initializes a new instance of the ScalarsDelegate class.

        :param parent: The parent object of this ScalarsDelegate instance.
        """
        super().__init__(IodeType.SCALARS, parent)

    # override BaseDelegate method
    def createEditor(self, parent: QWidget, option: QStyleOptionViewItem, index: QModelIndex):
        """
        Creates an editor widget for the given index.

        :param parent: The parent widget.
        :param option: The style options for the view item.
        :param index: The model index.
        :return: The editor widget.
        """
        editor = QLineEdit(parent)

        if index.column() == 0:
            editor.setValidator(IodeDoubleValidator(parent=editor))
        elif index.column() == 1:
            # set the locale for the validator to ensure dot as decimal separator
            validator = QDoubleValidator(0., 1., 10)
            validator.setLocale(QLocale("C"))
            editor.setValidator(validator)
        else:
            editor = None

        return editor
