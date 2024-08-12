from PyQt6.QtWidgets import QDialog, QMessageBox
from PyQt6.QtCore import pyqtSlot
from PyQt6.QtGui import QDesktopServices


from iode import variables, Sample
from ui_edit_vars_sample import Ui_EditIodeSampleDialog
from utils import URL_MANUAL


class EditIodeSampleDialog(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        # Create an instance of the widget defined in the .ui file
        self.ui = Ui_EditIodeSampleDialog()
        self.ui.setupUi(self)

        sample = variables.sample
        if sample.nb_periods() == 0:
            self.ui.labelTitle.setText("New Variables Sample")
        else:
            self.ui.labelTitle.setText("Variables Sample")
            if sample.start:
                self.ui.sampleEdit_from.setText(str(sample.start))
            if sample.end:
                self.ui.sampleEdit_to.setText(str(sample.end))

    @pyqtSlot()
    def edit(self):
        try:
            from_period = self.ui.sampleEdit_from.text()
            to_period = self.ui.sampleEdit_to.text()

            variables.sample = f"{from_period}:{to_period}"

            self.accept()
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))
            self.reject()

    @pyqtSlot()
    def help(self):
        QDesktopServices.openUrl(URL_MANUAL)
