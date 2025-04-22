from PySide6.QtCore import Slot
from PySide6.QtWidgets import QWidget, QMessageBox

from iode_gui.settings import MixinSettingsDialog
from iode_gui.text_edit.completer import IodeCompleter
from iode_gui.abstract_main_window import AbstractMainWindow
from .ui_list_calculus import Ui_MenuDataListCalculus

from itertools import product
from iode import IodeType, lists


class MenuDataListCalculus(MixinSettingsDialog):
    def __init__(self, parent: AbstractMainWindow):
        super().__init__(parent)
        self.ui = Ui_MenuDataListCalculus()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)

        self.v_operators = ["+", "-", "*", "x"]
        self.ui.comboBox_operator.addItems(self.v_operators)
        self.ui.comboBox_operator.setCurrentIndex(0)  

        self.ui.lineEdit_list1.setup_completer(iode_types=IodeType.LISTS)
        self.ui.lineEdit_list2.setup_completer(iode_types=IodeType.LISTS)

        self.ui.lineEdit_list_res.setText("_RES")

        self.load_settings()

    @Slot()
    def calculus(self):
        try:
            list1_name: str = self.ui.lineEdit_list1.text().strip()
            list2_name: str = self.ui.lineEdit_list2.text().strip()
            list_result_name:str = self.ui.lineEdit_list_res.text().strip()
            operator: str = self.ui.comboBox_operator.currentText()

            if list1_name == "":
                raise Exception("List 1 is empty")
            if list2_name == "":
                raise Exception("List 2 is empty")
            
            list1 = lists[list1_name]
            list2 = lists[list2_name]

            # union
            if operator == "+":
                set_result = set(list1) | set(list2)
            # difference
            elif operator == "-":
                set_result = set(list1) - set(list2)
            # intersection
            elif operator == "*":
                set_result = set(list1) & set(list2)
            # product: ["A", "B"] x ["C", "D"] => ["AB", "AC", "BC", "BD"]
            elif operator == "x":
                set_result = [item1 + item2 for item1, item2 in product(list1, list2)]

            list_result = sorted(list(set_result))

            if list_result_name:
                lists[list_result_name] = list_result

            self.ui.textEdit_res.setPlainText("; ".join(list_result))
            self.ui.label_count.setText(str(len(list_result)))
        except Exception as e:
            self.parent().display_output(f"ERROR -> {str(e)}")
            QMessageBox.warning(self, "WARNING", str(e))
