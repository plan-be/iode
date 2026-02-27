from qtpy.QtCore import Qt, Slot, QSettings
from qtpy.QtWidgets import QLabel, QComboBox, QLineEdit, QSpacerItem, QSizePolicy, QDialog
from qtpy.QtGui import QShortcut, QKeySequence

from iode_gui.settings import get_settings
from iode_gui.main_widgets.tab_widget.abstract_tab_widget import AbstractIodeTabWidget
from iode_gui.tabs.iode_objs.tab_database_abstract import MixinShowIodeDatabaseSubset, AbstractIodeObjectWidget
from iode_gui.tabs.iode_objs.tab_numerical_values import NumericalWidget
from iode_gui.iode_objs.models.table_model import  VariablesModel

from typing import List
from iode import IodeType, VarsMode


class CommentsWidget(AbstractIodeObjectWidget):
    """
    This class represents a widget for handling comments in the database.
    """
    def __init__(self, parent):
        """
        Initialize the CommentsWidget.

        :param parent: The parent widget for this CommentsWidget.
        """
        super().__init__(IodeType.COMMENTS, parent)

        self.database_model.dataChanged.connect(self.database_modified)
        self.database_model.headerDataChanged.connect(self.database_modified)
        self.database_model.rowsInserted.connect(self.database_modified)
        self.database_model.rowsRemoved.connect(self.database_modified)
        self.database_model.database_modified.connect(self.database_modified)
        self.database_view.database_modified.connect(self.database_modified)


class EquationsWidget(AbstractIodeObjectWidget):
    """
    This class represents a widget for handling equations in the database.
    """
    def __init__(self, parent):
        super().__init__(IodeType.EQUATIONS, parent)

        self.database_model.dataChanged.connect(self.database_modified)
        self.database_model.headerDataChanged.connect(self.database_modified)
        self.database_model.rowsInserted.connect(self.database_modified)
        self.database_model.rowsRemoved.connect(self.database_modified)
        self.database_model.database_modified.connect(self.database_modified)
        self.database_view.database_modified.connect(self.database_modified)


class IdentitiesWidget(AbstractIodeObjectWidget):
    """
    This class represents a widget for handling identities in the database.
    """
    def __init__(self, parent):
        super().__init__(IodeType.IDENTITIES, parent)

        # execution sample for identities
        self.label_idt_sample = QLabel("Execution sample:")
        self.label_idt_sample.setObjectName("label_idt_sample")
        self.top_horizontal_layout.insertWidget(3, self.label_idt_sample, alignment=Qt.AlignmentFlag.AlignLeft)

        self.lineEdit_idt_sample = QLineEdit()
        self.lineEdit_idt_sample.setObjectName("lineEdit_idt_sample")
        self.lineEdit_idt_sample.setPlaceholderText("from_period:to_period")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        self.lineEdit_idt_sample.setSizePolicy(sizePolicy)
        self.lineEdit_idt_sample.setMinimumSize(150, 0)
        self.top_horizontal_layout.insertWidget(4, self.lineEdit_idt_sample, alignment=Qt.AlignmentFlag.AlignLeft)

        self.lineEdit_idt_sample.textChanged.connect(self.database_view.set_execution_sample)

        self.database_model.dataChanged.connect(self.database_modified)
        self.database_model.headerDataChanged.connect(self.database_modified)
        self.database_model.rowsInserted.connect(self.database_modified)
        self.database_model.rowsRemoved.connect(self.database_modified)
        self.database_model.database_modified.connect(self.database_modified)
        self.database_view.database_modified.connect(self.database_modified)

    # NOTE: automatically called from IodeTabWidget.load_settings()
    def load_settings(self):
        """
        Loads settings from a QSettings object.
        """
        settings: QSettings = get_settings()
        if settings is None:
            return

        sample = settings.value("idt_sample", "", type=str)
        self.lineEdit_idt_sample.setText(sample)

    # NOTE: automatically called from IodeTabWidget.save_settings()
    def save_settings(self):
        """
        Saves settings to a QSettings object.
        """
        settings: QSettings = get_settings()
        if settings is None:
            return
        
        settings.setValue("idt_sample", self.lineEdit_idt_sample.text())


class ListsWidget(AbstractIodeObjectWidget):
    """
    This class represents a widget for handling lists in the database.
    """
    def __init__(self, parent):
        super().__init__(IodeType.LISTS, parent)

        self.database_model.dataChanged.connect(self.database_modified)
        self.database_model.headerDataChanged.connect(self.database_modified)
        self.database_model.rowsInserted.connect(self.database_modified)
        self.database_model.rowsRemoved.connect(self.database_modified)
        self.database_model.database_modified.connect(self.database_modified)
        self.database_view.database_modified.connect(self.database_modified)


class ScalarsWidget(AbstractIodeObjectWidget, NumericalWidget):
    """
    This class represents a widget for handling scalars in the database.
    """
    def __init__(self, parent):
        AbstractIodeObjectWidget.__init__(self, IodeType.SCALARS, parent)
        NumericalWidget.__init__(self)
        self.set_model_and_view(self.database_model, self.database_view)

        horizontalSpacer = QSpacerItem(800, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)
        self.bottom_layout.addSpacerItem(horizontalSpacer)

        self.vertical_layout.addLayout(self.bottom_layout)

        self.database_model.dataChanged.connect(self.database_modified)
        self.database_model.headerDataChanged.connect(self.database_modified)
        self.database_model.rowsInserted.connect(self.database_modified)
        self.database_model.rowsRemoved.connect(self.database_modified)
        self.database_model.database_modified.connect(self.database_modified)
        self.database_view.database_modified.connect(self.database_modified)

    # NOTE: automatically called from IodeTabWidget.load_settings()
    def load_settings(self):
        settings: QSettings = get_settings()
        if settings is None:
            return
        self.load_numeric_settings(settings)

    # NOTE: automatically called from IodeTabWidget.save_settings()
    def save_settings(self):
        settings: QSettings = get_settings()
        if settings is None:
            return
        self.save_numeric_settings(settings)


class TablesWidget(AbstractIodeObjectWidget):
    """
    This class represents a widget for handling tables in the database.
    """
    def __init__(self, parent):
        super().__init__(IodeType.TABLES, parent)

        self.database_model.dataChanged.connect(self.database_modified)
        self.database_model.headerDataChanged.connect(self.database_modified)
        self.database_model.rowsInserted.connect(self.database_modified)
        self.database_model.rowsRemoved.connect(self.database_modified)
        self.database_model.database_modified.connect(self.database_modified)
        self.database_view.database_modified.connect(self.database_modified)


class VariablesWidget(AbstractIodeObjectWidget, NumericalWidget):
    """
    A widget for handling VariablesModel and VariablesView objects.
    """
    def __init__(self, parent=None):
        AbstractIodeObjectWidget.__init__(self, IodeType.VARIABLES, parent)
        NumericalWidget.__init__(self)
        self.set_model_and_view(self.database_model, self.database_view)

        sizePolicy = QSizePolicy(QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)

        # periods filter
        self.label_filter_periods = QLabel("Periods:")
        self.label_filter_periods.setObjectName("label_periods_filter")
        self.top_horizontal_layout.insertWidget(3, self.label_filter_periods, alignment=Qt.AlignmentFlag.AlignLeft)

        self.lineEdit_filter_periods = QLineEdit()
        self.lineEdit_filter_periods.setObjectName("lineEdit_filter_periods")
        self.lineEdit_filter_periods.setPlaceholderText("periods range here")
        sizePolicyFilter = QSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Fixed)
        sizePolicyFilter.setHorizontalStretch(0)
        sizePolicyFilter.setVerticalStretch(0)
        self.lineEdit_filter_periods.setSizePolicy(sizePolicyFilter)
        self.lineEdit_filter_periods.setMinimumSize(150, 0)
        self.top_horizontal_layout.insertWidget(4, self.lineEdit_filter_periods, alignment=Qt.AlignmentFlag.AlignLeft)

        self.database_view.set_filter_periods(self.lineEdit_filter_periods)
        self.database_view_2.set_filter_periods(self.lineEdit_filter_periods)

        # Add the mode label and combo box
        self.label_mode = QLabel("mode")
        self.label_mode.setObjectName("label_mode")
        self.label_mode.setSizePolicy(sizePolicy)
        self.bottom_layout.addWidget(self.label_mode, Qt.AlignmentFlag.AlignLeft)

        self.combo_mode = QComboBox()
        self.combo_mode.setObjectName("combobox_mode")
        self.v_variable_modes = list(VarsMode)
        v_variable_modes_names = [item.name.replace("_" , " ").title() for item in self.v_variable_modes]
        self.combo_mode.addItems(v_variable_modes_names)
        self.combo_mode.setSizePolicy(sizePolicy)
        self.combo_mode.setCurrentIndex(0)
        self.bottom_layout.addWidget(self.combo_mode, Qt.AlignmentFlag.AlignLeft)

        # Add a spacer
        horizontalSpacer = QSpacerItem(800, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)
        self.bottom_layout.addSpacerItem(horizontalSpacer)

        # Add the bottom layout to the main layout
        self.vertical_layout.addLayout(self.bottom_layout)

        # Create shortcuts
        self.shortcut_mode_plus = QShortcut(QKeySequence(Qt.Key.Key_F5), self)
        self.shortcut_mode_plus.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)

        self.shortcut_mode_minus = QShortcut(QKeySequence(Qt.Modifier.SHIFT | Qt.Key.Key_F5), self)
        self.shortcut_mode_minus.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)

        # Connect signals to slots
        self.lineEdit_filter_periods.returnPressed.connect(self.database_view.filter_slot)

        self.combo_mode.currentIndexChanged.connect(self.update_mode)
        self.shortcut_mode_plus.activated.connect(self.next_mode)
        self.shortcut_mode_minus.activated.connect(self.previous_mode)

        self.database_model.dataChanged.connect(self.database_modified)
        self.database_model.headerDataChanged.connect(self.database_modified)
        self.database_model.rowsInserted.connect(self.database_modified)
        self.database_model.rowsRemoved.connect(self.database_modified)
        self.database_model.database_modified.connect(self.database_modified)
        self.database_model.database_loaded.connect(self.reset_mode)
        self.database_model.database_cleared.connect(self.reset_mode)
        self.database_view.database_modified.connect(self.database_modified)

    def reset_filter(self):
        """
        Reset the filter.
        """
        self.lineEdit_filter.setText("")
        self.lineEdit_filter_periods.setText("")
        self.database_view.filter()

    # NOTE: automatically called from IodeTabWidget.load_settings()
    def load_settings(self):
        """
        Loads settings from a QSettings object.
        """
        settings: QSettings = get_settings()
        if settings is None:
            return

        self.load_numeric_settings(settings)
        mode = settings.value("mode", 0, type=int)
        self.combo_mode.setCurrentIndex(mode)

    # NOTE: automatically called from IodeTabWidget.save_settings()
    def save_settings(self):
        """
        Saves settings to a QSettings object.
        """
        settings: QSettings = get_settings()
        if settings is None:
            return
        
        self.save_numeric_settings(settings)
        settings.setValue("mode", self.combo_mode.currentIndex())

    @Slot()
    def reset_mode(self):
        """
        Resets the mode of the VariablesModel object to LEVEL.
        """
        self.combo_mode.setCurrentIndex(0)

    @Slot(int)
    def update_mode(self, index):
        """
        Updates the mode of the VariablesModel object.
        """
        mode = self.v_variable_modes[index]
        _model: VariablesModel = self.database_model
        _model.set_mode(mode)

    @Slot()
    def next_mode(self):
        """
        Sets the next mode in the combo box.
        """
        index = self.combo_mode.currentIndex()
        index = 0 if index == self.combo_mode.count() - 1 else index + 1
        self.combo_mode.setCurrentIndex(index)

    @Slot()
    def previous_mode(self):
        """
        Sets the previous mode in the combo box.
        """
        index = self.combo_mode.currentIndex()
        index = self.combo_mode.count() - 1 if index == 0 else index - 1
        self.combo_mode.setCurrentIndex(index)


class DialogDatabaseSubset(MixinShowIodeDatabaseSubset, QDialog):
    def __init__(self, tab_widget: AbstractIodeTabWidget, parent=None):
        MixinShowIodeDatabaseSubset.__init__(self)
        QDialog.__init__(self, parent)
        self.tab_widget = tab_widget

        self.setObjectName("dialog_database_subset")
        self.setWindowTitle("IODE Database Subset")

    @Slot(IodeType, list)
    def show_database_subset(self, iode_type: IodeType, obj_names: List[str] = None):
        self.tab_widget.show_database_subset(iode_type, obj_names)
