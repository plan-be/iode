from PySide6.QtCore import Qt, Slot, QSettings
from PySide6.QtWidgets import QDialog, QVBoxLayout, QSpacerItem, QSizePolicy
from PySide6.QtGui import QCloseEvent

from iode_gui.settings import ProjectSettings, class_name_to_settings_group_name
from iode_gui.tabs.iode_objs.tab_numerical_values import NumericalWidget
from iode_gui.iode_objs.models.computed_table_model import ComputedTableModel
from iode_gui.iode_objs.views.computed_table_view import ComputedTableView


class ComputedTableNumericalDialog(QDialog, NumericalWidget):
    def __init__(self, table_name: str, generalized_sample: str, nb_decimals: int, variables: str=None, parent=None):
        QDialog.__init__(self, parent)
        NumericalWidget.__init__(self)
        self.setGeometry(10, 50, 700, 250)

        self.vertical_layout = QVBoxLayout(self)
        self.vertical_layout.setContentsMargins(0, 0, 0, 0)
        self.vertical_layout.setObjectName("computed_table_dialog_layout")

        self.table_view = ComputedTableView(self)
        self.table_model = ComputedTableModel(table_name, generalized_sample, nb_decimals, variables, self)
        self.table_view.setModel(self.table_model)
        self.set_model_and_view(self.table_model, self.table_view)
        self.table_view.horizontalHeader().setStretchLastSection(False)
        stylesheet = "QHeaderView::section { background-color: lightGray; font: bold; border: 0.5px solid }"
        self.table_view.setStyleSheet(stylesheet)
        self.vertical_layout.addWidget(self.table_view)

        horizontalSpacer = QSpacerItem(800, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)
        self.bottom_layout.addSpacerItem(horizontalSpacer)

        self.vertical_layout.addLayout(self.bottom_layout)

        self.settings_group_name = class_name_to_settings_group_name(self)
        self.load_settings()

        self.setModal(True)
        if not variables:
            self.setWindowTitle(f"Table {table_name}")
        else:
            self.setWindowTitle("Table of series")

    # override QDialog method
    @Slot()
    def closeEvent(self, event: QCloseEvent):
        self.save_settings()
        event.accept()

    @Slot()
    def load_settings(self):
        project_settings: QSettings = ProjectSettings.project_settings
        if project_settings is None:
            return
        project_settings.beginGroup(self.settings_group_name)
        self.load_numeric_settings(project_settings)
        project_settings.endGroup()

    @Slot()
    def save_settings(self):
        project_settings: QSettings = ProjectSettings.project_settings
        if project_settings is None:
            return
        project_settings.beginGroup(self.settings_group_name)
        self.save_numeric_settings(project_settings)
        project_settings.endGroup()
