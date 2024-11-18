from PySide6.QtCore import Qt, QObject, QSize, QSettings
from PySide6.QtGui import QKeySequence, QShortcut
from PySide6.QtWidgets import QLabel, QSpinBox, QCheckBox, QHBoxLayout

from utils import MAX_PRECISION_NUMBERS
from iode_objs.models.table_model import ScalarsModel, VariablesModel
from iode_objs.views.table_view import ScalarsView, VariablesView


from typing import List, Union


class NumericalWidget:
    """
    Defines a widget containing a Scalar or Variable table and a QSpinBox 
    indicating the number of significant digits to display.
    """
    COLUMN_SIZE_STEP = 10

    def __init__(self):
        self.table_model: Union[ScalarsModel, VariablesModel] = None
        self.table_view: Union[ScalarsView, VariablesView] = None

        self.bottom_layout: QHBoxLayout = None

        self.label_precision: QLabel = QLabel("Precision")
        self.spinBox_precision = QSpinBox()
        self.spinBox_precision.setMinimum(1)
        self.spinBox_precision.setMaximum(MAX_PRECISION_NUMBERS)
        self.spinBox_precision.setValue(4)
        self.spinBox_precision.setMinimumSize(QSize(70, 0))
        self.spinBox_precision.valueChanged.connect(self.update_precision)

        self.checkbox_scientific: QCheckBox = QCheckBox("Scientific")
        self.checkbox_scientific.setChecked(False)
        self.checkbox_scientific.stateChanged.connect(self.format_number)

    def set_model_and_view(self, table_model: Union[ScalarsModel, VariablesModel], 
                           table_view: Union[ScalarsView, VariablesView]):
        # need to do some initializations here instead of in the __init__() method above to avoid the error message:
        # RuntimeError: '__init__' method of object's base class (ScalarsWidget/VariablesWidget) not called
        # when running ScalarsWidget/VariablesWidget.__init__()
        self.table_model = table_model
        self.table_view = table_view

        self.bottom_layout = QHBoxLayout(self)
        iode_type_name = str(self.table_model.iode_type).lower()
        self.bottom_layout.setObjectName(f"bottom_layout_{iode_type_name}")

        self.bottom_layout.addWidget(self.label_precision, Qt.AlignmentFlag.AlignLeft)
        self.bottom_layout.addWidget(self.spinBox_precision, Qt.AlignmentFlag.AlignLeft)
        self.bottom_layout.addWidget(self.checkbox_scientific, Qt.AlignmentFlag.AlignLeft)

        self.shortcut_precision_plus = QShortcut(QKeySequence(Qt.Key.Key_F4), self)
        self.shortcut_precision_minus = QShortcut(QKeySequence(Qt.Modifier.SHIFT | Qt.Key.Key_F4), self)
        self.shortcut_increase_column_size = QShortcut(QKeySequence(Qt.Key.Key_F3), self)
        self.shortcut_decrease_column_size = QShortcut(QKeySequence(Qt.Modifier.SHIFT | Qt.Key.Key_F3), self)
        self.shortcut_resize_to_contents = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Modifier.SHIFT | Qt.Key.Key_F3), self)

        self.shortcut_precision_plus.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.shortcut_precision_minus.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.shortcut_increase_column_size.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.shortcut_decrease_column_size.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.shortcut_resize_to_contents.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)

        self.spinBox_precision.valueChanged.connect(self.update_precision)
        self.checkbox_scientific.stateChanged.connect(self.format_number)
        self.shortcut_precision_plus.activated.connect(lambda: self.spinBox_precision.stepUp())
        self.shortcut_precision_minus.activated.connect(lambda: self.spinBox_precision.stepDown())
        self.shortcut_increase_column_size.activated.connect(self.increase_column_size)
        self.shortcut_decrease_column_size.activated.connect(self.decrease_column_size)
        self.shortcut_resize_to_contents.activated.connect(lambda: self.table_view.resizeColumnsToContents())

    def increase_column_size(self):
        """
        Increase the size of the columns in the table view.
        """
        columns = self.table_view.horizontalHeader()
        max_size = columns.maximumSectionSize()
        for j in range(columns.count()):
            columns.resizeSection(j, min(columns.sectionSize(j) + self.COLUMN_SIZE_STEP, max_size))

    def decrease_column_size(self):
        """
        Decrease the size of the columns in the table view.
        """
        columns = self.table_view.horizontalHeader()
        min_size = columns.minimumSectionSize()
        for j in range(columns.count()):
            columns.resizeSection(j, max(columns.sectionSize(j) - self.COLUMN_SIZE_STEP, min_size))

    def update_precision(self, value):
        """
        Update the precision of the numerical values in the table model.
        """
        self.table_model.precision = value

    def format_number(self, state: Qt.CheckState):
        """
        Format the numerical values in the table model as either floating point or scientific notation.
        """
        format = 'e' if state == Qt.CheckState.Checked else 'f'
        self.table_model.format = format
        self.spinBox_precision.setMinimum(self.table_model.min_precision)

    def load_numeric_settings(self, project_settings: QSettings):
        """
        Load the numerical settings from a QSettings object.
        """
        if not project_settings:
            return

        checked = project_settings.value("scientific", False, type=bool)
        self.checkbox_scientific.setChecked(checked)
        self.format_number(self.checkbox_scientific.checkState())

        nb_digits = project_settings.value("precision", self.spinBox_precision.value(), type=int)
        self.spinBox_precision.setValue(nb_digits)

    def save_numeric_settings(self, project_settings: QSettings):
        """
        Save the numerical settings to a QSettings object.
        """
        if not project_settings:
            return

        project_settings.setValue("precision", self.spinBox_precision.value())
        project_settings.setValue("scientific", self.checkbox_scientific.isChecked())
