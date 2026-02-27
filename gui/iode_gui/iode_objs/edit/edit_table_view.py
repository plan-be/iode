from qtpy.QtCore import Slot
from qtpy.QtWidgets import QTableView, QHeaderView

from iode_gui.color_theme import ColorTheme
from iode_gui.settings import get_color_theme
from .edit_table_delegate import EditTableDelegate
from .edit_table_model import EditTableModel

from iode import TableLineType


class EditTableView(QTableView):
    def __init__(self, parent=None):
        super().__init__(parent)
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
        self.horizontalHeader().setSectionResizeMode(QHeaderView.ResizeMode.ResizeToContents)
        self.verticalHeader().setStyleSheet("border: none")

    def setup_model(self, table_name: str):
        model = EditTableModel(table_name, self.parent())
        self.setModel(model)
        self.setItemDelegate(EditTableDelegate())
        model.display_data.connect(self.update_row)

    @Slot()
    def update_row(self, row: int, line_type: int, nb_columns: int):
        if line_type != TableLineType.CELL:
            self.setSpan(row, 0, 1, nb_columns)
        if line_type == TableLineType.SEP:
            self.setRowHeight(row, 3)
