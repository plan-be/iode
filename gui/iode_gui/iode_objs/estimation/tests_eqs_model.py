from PySide6.QtCore import QAbstractTableModel, QModelIndex, Qt

from typing import List
from iode.util import EditAndEstimateEquations
from iode import EqTest, EQ_TEST_NAMES, Equation


class TestsEqsModel(QAbstractTableModel):
    def __init__(self, edit_est_eqs: EditAndEstimateEquations, parent=None):
        super().__init__(parent)
        self.v_tests_names = [member.name.lower() for member in EqTest] 
        
        # NOTE: Be aware that the Equations database belonging to the EditAndEstimateEquations 
        #       instance may contain more equations than the ones contained in the currently 
        #       evaluated block. The property equations_list gives the list of currently 
        #       estimated equations.
        equations_db = edit_est_eqs.equations_db
        self.v_equations: List[Equation] = [equations_db[equation_name] for equation_name 
                                            in edit_est_eqs.equations_list]

    def rowCount(self, parent=QModelIndex()) -> int:
        """
        Override method from QAbstractTableModel
        :param parent: QModelIndex
        :return: int
        """
        return len(EQ_TEST_NAMES)

    def columnCount(self, parent=QModelIndex()) -> int:
        """
        Override method from QAbstractTableModel
        :param parent: QModelIndex
        :return: int
        """
        return len(self.v_equations)

    def headerData(self, section: int, orientation: Qt.Orientation, role: Qt.ItemDataRole):
        """
        Override method from QAbstractTableModel
        :param section: int
        :param orientation: Qt.Orientation
        :param role: Qt.ItemDataRole
        """
        if role != Qt.ItemDataRole.DisplayRole:
            return
        
        if orientation == Qt.Orientation.Horizontal:
            return self.v_equations[section].endogenous
        else:
            return EQ_TEST_NAMES[section]

    def data(self, index: QModelIndex, role: Qt.ItemDataRole=Qt.ItemDataRole.DisplayRole):
        """
        Override method from QAbstractTableModel
        :param index: QModelIndex
        :param role: Qt.ItemDataRole
        """
        if not index.isValid():
            return

        if role == Qt.ItemDataRole.TextAlignmentRole:
            return int(Qt.AlignmentFlag.AlignRight)

        if role == Qt.ItemDataRole.DisplayRole:
            equation = self.v_equations[index.column()]
            test_name = self.v_tests_names[index.row()]
            value: float = equation.tests[test_name]
            return f"{value:.8g}"

        return
