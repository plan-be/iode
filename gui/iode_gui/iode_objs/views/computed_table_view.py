from qtpy.QtCore import Slot
from qtpy.QtWidgets import QTableView, QMenu
from qtpy.QtGui import QContextMenuEvent

from iode_gui.color_theme import ColorTheme
from iode_gui.settings import get_color_theme
from .numerical_view import NumericalTableView


class ComputedTableView(QTableView, NumericalTableView):
    def __init__(self, parent=None):
        QTableView.__init__(self, parent)
        NumericalTableView.__init__(self)
        NumericalTableView.setup(self, allow_to_paste=False)
        self.update_colors()

    def update_colors(self):
        """
        Set the highlighting colors according to the current color theme.
        """
        color_theme: ColorTheme = get_color_theme()
        background_color = color_theme.header_background.name()
        text_color = color_theme.header_text_color.name()
        stylesheet = f"QHeaderView::section {{ background-color: {background_color}; color: {text_color}, "\
                     "font: bold; border: 0.5px solid }"
        self.setStyleSheet(stylesheet)

    # override QTableView method
    @Slot(QContextMenuEvent)
    def contextMenuEvent(self, event: QContextMenuEvent):
        self.popup_context_menu(event)
        event.accept()
