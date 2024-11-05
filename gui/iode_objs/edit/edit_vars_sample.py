from PySide6.QtCore import Slot
from PySide6.QtWidgets import QDialog, QMessageBox
from PySide6.QtGui import QDesktopServices

from .ui_edit_vars_sample import Ui_EditIodeSampleDialog
from utils import URL_MANUAL

from iode import variables, Period


class EditIodeSampleDialog(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.ui = Ui_EditIodeSampleDialog()
        self.ui.setupUi(self)

        sample = variables.sample
        if sample.nb_periods:
            self.ui.labelTitle.setText("Variables Sample")
            self.ui.sampleEdit_from.setText(str(sample.start))
            self.ui.sampleEdit_to.setText(str(sample.end))
        else:
            self.ui.labelTitle.setText("New Variables Sample")
    
    @property
    def from_period(self) -> str:
        return self.ui.sampleEdit_from.text()
    
    @property
    def to_period(self) -> str:
        return self.ui.sampleEdit_to.text()

    @Slot()
    def edit(self):
        try:
            # raises an error if from_period and/or to_period is/are invalid
            from_period = Period(self.ui.sampleEdit_from.text())
            to_period = Period(self.ui.sampleEdit_to.text())
            if from_period.periodicity != to_period.periodicity:
                raise ValueError("'from_period' and 'to_period' must have the same periodicity.\n" 
                                 f"Got periodicity '{from_period.periodicity}' (from_period) and "
                                 f"'{to_period.periodicity}' (to_period)")
            if to_period.difference(from_period) < 0:
                raise ValueError(f"'to_period' must be after 'from_period'.\nGot '{to_period}' "
                                 f"(to_period) and '{from_period}' (from_period)")
            self.accept()
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))

    @Slot()
    def help(self):
        """
        Opens the manual in a web browser.
        """
        QDesktopServices.openUrl(URL_MANUAL)
