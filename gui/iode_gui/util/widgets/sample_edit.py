from qtpy.QtCore import QRegularExpression
from qtpy.QtGui import QRegularExpressionValidator
from qtpy.QtWidgets import QLineEdit


class IodeSampleEdit(QLineEdit):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setPlaceholderText("yyyyPppp")
        self.setToolTip("-yyyy is the year\n"
                        "-P is the periodicity (Y=yearly, Q=quaterly, M=monthly, W=weekly)\n"
                        "-ppp is the period of the year")
        re = QRegularExpression("[1-9][0-9YQMW]{5,7}")
        validator = QRegularExpressionValidator(re, self)
        self.setValidator(validator)
