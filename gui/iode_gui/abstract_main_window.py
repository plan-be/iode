from PySide6.QtCore import Qt, QSettings, Slot
from PySide6.QtWidgets import QMainWindow, QTextEdit, QLineEdit, QDialog

from iode_gui.text_edit.completer import IodeCompleter
from iode_gui.plot.plot import PlotDialog
from iode_gui.utils import DEFAULT_FONT_FAMILY

from typing import List
from iode import IodeType


class AbstractMainWindow(QMainWindow):
    
    def __init__(self, parent=None):
        super().__init__(parent)

        # ---- prepare auto-completion ----
        self.completer = IodeCompleter(True, False, -1, self)

        # ---- list of open dialogs ----
        self.dialogs: List[QDialog] = []

        # ---- dock widgets ----
        # make left dock widget to extend to bottom of the window
        self.setCorner(Qt.Corner.BottomLeftCorner, Qt.DockWidgetArea.LeftDockWidgetArea)

        # ---- user settings ----
        # location if QSettings::UserScope -> FOLDERID_RoamingAppData
        # see https://doc.qt.io/qt-6/qsettings.html#setPath
        self.user_settings: QSettings = QSettings(QSettings.Scope.UserScope)
        self.project_path: str = None
        self.recent_projects: List[str] = self.user_settings.value("recent_projects", [])
        self.font_family: List[str] = self.user_settings.value("font_family", DEFAULT_FONT_FAMILY)

    @property
    def output(self) -> QTextEdit:
        raise NotImplementedError()
    
    @property
    def iode_command(self) -> QLineEdit:
        raise NotImplementedError()

    @Slot(QDialog)
    def append_dialog(self, dialog: QDialog):
        dialog.open()
        self.dialogs.append(dialog)

    @Slot(PlotDialog)
    def append_plot(self, dialog: PlotDialog):
        dialog.plot()
        dialog.open()
        self.dialogs.append(dialog)

    @Slot()
    def open_compute_identities_dialog(self):
        raise NotImplementedError()

    @Slot()
    def open_graphs_variables_dialog(self):
        raise NotImplementedError()

    @Slot(list, str, str)
    def open_graphs_variables_dialog_from_vars_view(self, variable_names: List[str], 
                                                    from_period: str, to_period: str):
        raise NotImplementedError()

    @Slot(bool)
    def compute_hash(self, value: bool):
        raise NotImplementedError()

    @Slot()
    @Slot(IodeType)
    def update_tab_and_completer(self, iode_type: IodeType = None):
        raise NotImplementedError()
