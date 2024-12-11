from PySide6.QtWidgets import QWidget
from PySide6.QtGui import QValidator, QDoubleValidator

from iode_gui.utils import NAN_REP
from typing import overload


class IodeDoubleValidator(QDoubleValidator):
    """
    A custom QDoubleValidator that accepts the NAN_REP string.
    """

    @overload
    def __init__(self, bottom: float, top: float, decimals: int, parent: QWidget=None):
        """
        Initialize the IodeDoubleValidator.

        :param bottom: The bottom value of the range.
        :param top: The top value of the range. 
        :param decimals: The maximum number of decimal places allowed. 
        :param parent: The parent of this validator. If None, this validator has no parent.
        """
        super().__init__(bottom, top, decimals, parent)
        
    @overload
    def __init__(parent: QWidget=None):
        """
        Initialize the IodeDoubleValidator.

        :param parent: The parent of this validator. If None, this validator has no parent.
        """ 
        super().__init__(parent)

    def validate(self, input: str, pos: int):
        """
        Validate the input string.

        :param input: The string to validate.
        :param pos: The position of the cursor in the string. This parameter is not used by this validator.
        :return: The validation state of the input string.
        """
        if input == NAN_REP:
            return QValidator.State.Acceptable
        else:
            return super().validate(input, pos)
