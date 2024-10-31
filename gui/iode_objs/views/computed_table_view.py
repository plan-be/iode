from PySide6.QtCore import Slot
from PySide6.QtWidgets import QTableView, QMenu
from PySide6.QtGui import QContextMenuEvent

from .numerical_view import NumericalTableView


class ComputedTableView(QTableView, NumericalTableView):
    def __init__(self, parent=None):
        QTableView.__init__(parent)
        NumericalTableView.__init__(False)

    # override QTableView method
    @Slot(QContextMenuEvent)
    def contextMenuEvent(self, event: QContextMenuEvent):
        self.popup_context_menu(event)
        event.accept()
