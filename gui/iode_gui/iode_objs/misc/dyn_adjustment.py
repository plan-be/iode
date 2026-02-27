from qtpy.QtWidgets import QMessageBox
from qtpy.QtCore import Slot

from iode_gui.settings import MixinSettingsDialog
from .ui_dyn_adjustment import Ui_DynAdjustmentDialog

from iode import AdjustmentMethod, dynamic_adjustment


class DynAdjustmentDialog(MixinSettingsDialog):

    def __init__(self, lec: str, parent=None):
        super().__init__(parent)
        self.lec: str = lec

        self.ui = Ui_DynAdjustmentDialog()
        self.ui.setupUi(self)

        self.ui.textEdit_equation.setText(lec)
        self.v_adjustment_methods = list(AdjustmentMethod)
        self.ui.comboBox_method.addItems([method.name for method in self.v_adjustment_methods])

    @Slot()
    def adjust(self):
        try:
            lec = self.ui.textEdit_equation.toPlainText()
            method_index = self.ui.comboBox_method.currentIndex()
            method = self.v_adjustment_methods[method_index]
            coef1 = self.ui.lineEdit_coef1.text()
            coef2 = self.ui.lineEdit_coef2.text()

            self.lec = dynamic_adjustment(method, lec, coef1, coef2)
            self.accept()
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))
