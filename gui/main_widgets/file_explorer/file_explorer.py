from PyQt6.QtCore import pyqtSlot, QDir
from PyQt6.QtWidgets import QTreeView
from ..tab_widget.tab_widget import IodeTabWidget


class IodeFileExplorer(QTreeView):
    def __init__(self, parent=None):
        super(IodeFileExplorer, self).__init__(parent)

    def set_iode_tab_widget(self, iode_tab_widget: IodeTabWidget):
        self.iode_tab_widget = iode_tab_widget

    def update_project_dir(self, project_dir: QDir):
        pass

    @pyqtSlot()
    def save_settings(self) -> None:
        pass
