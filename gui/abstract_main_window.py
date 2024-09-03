from PyQt6.QtCore import Qt, QSettings, pyqtSlot
from PyQt6.QtWidgets import QMainWindow, QTextEdit, QLineEdit, QDialog

from text_edit.completer import IodeCompleter
from plot.plot import PlotDialog
from utils import DEFAULT_FONT_FAMILY

from typing import List
from iode import IodeTypes


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
        self.user_settings: QSettings = QSettings(QSettings.Scope.UserScope, self)
        self.project_path: str = self.user_settings.value("project_path", None)
        self.recent_projects: List[str] = self.user_settings.value("recent_projects", [])
        self.font_family: List[str] = self.user_settings.value("font_family", DEFAULT_FONT_FAMILY)

    @property
    def output(self) -> QTextEdit:
        raise NotImplementedError()
    
    @property
    def iode_command(self) -> QLineEdit:
        raise NotImplementedError()

    @pyqtSlot(QDialog)
    def append_dialog(self, dialog: QDialog):
        dialog.open()
        self.dialogs.append(dialog)

    @pyqtSlot(PlotDialog)
    def append_plot(self, dialog: PlotDialog):
        dialog.plot()
        self.dialogs.append(dialog)

    @pyqtSlot()
    @pyqtSlot(IodeTypes)
    def update_tab_and_completer(self, iode_type: IodeTypes = None):
        raise NotImplementedError()
