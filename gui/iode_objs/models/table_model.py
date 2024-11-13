from PySide6.QtCore import QObject, Qt, Slot
from PySide6.QtWidgets import QMessageBox
from .abstract_table_model import IodeAbstractTableModel
from .numerical_table_model import IodeNumericalTableModel
from utils import MAX_PRECISION_NUMBERS, NAN_REP

from typing import List, Any
from iode import (IodeType, VarsMode, comments, equations, identities, lists, 
                  scalars, tables, variables, Scalar, Table, NA, is_NA)


class CommentsModel(IodeAbstractTableModel):
    def __init__(self, parent: QObject = None):
        super().__init__(["Comment"], IodeType.COMMENTS, comments, parent)

    def data_cell(self, row: int, col: int) -> str:
        try:
            return self._displayed_database.i[row]
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))
            return ""

    def set_value(self, row: int, column: int, value: str) -> bool:
        try:
            self._displayed_database.i[row] = value
            return True
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))
            return False


class EquationsModel(IodeAbstractTableModel):
    def __init__(self, parent: QObject = None):
        super().__init__(["Equation"], IodeType.EQUATIONS, equations, parent)

    def data_cell(self, row: int, col: int) -> str:
        try:
            return self._displayed_database.get_lec(row)
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))
            return ""

    def set_value(self, row: int, column: int, value: str) -> bool:
        try:
            # here 'value' represents the LEC expression of the equation
            self._displayed_database.i[row] = value
            return True
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))
            return False


class IdentitiesModel(IodeAbstractTableModel):
    def __init__(self, parent: QObject = None):
        super().__init__(["Identity"], IodeType.IDENTITIES, identities, parent)

    def data_cell(self, row: int, col: int) -> str:
        try:
            return str(self._displayed_database.i[row])
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))
            return ""

    def set_value(self, row: int, column: int, value: str) -> bool:
        try:
            self._displayed_database.i[row] = value
            return True
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))
            return False
        
    def execute_identity(self, row: int) -> None:
        name = self.headerData(row, Qt.Orientation.Vertical, Qt.ItemDataRole.DisplayRole)
        try:
            sample = variables.sample
            self._displayed_database.execute(name, sample.start, sample.end)
            QMessageBox.information(None, "INFO", f"Identity {name} successfully executed")
        except Exception as e:
            QMessageBox.warning(None, "WARNING", f"Could not execute identity {name}:\n\n{str(e)}")


class ListsModel(IodeAbstractTableModel):
    def __init__(self, parent: QObject = None):
        super().__init__(["List"], IodeType.LISTS, lists, parent)

    def data_cell(self, row: int, col: int) -> str:
        try:
            return "; ".join(self._displayed_database.i[row])
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))
            return ""

    def set_value(self, row: int, column: int, value: str) -> bool:
        try:
            self._displayed_database.i[row] = value
            return True
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))
            return False


class ScalarsModel(IodeAbstractTableModel, IodeNumericalTableModel):
    def __init__(self, precision: int=2, format: str='f', scalars_db=scalars, parent: QObject = None):
        IodeAbstractTableModel.__init__(self, ["Value", "Relax", "Std", "T-Stat"], IodeType.SCALARS, scalars_db, parent)
        IodeNumericalTableModel.__init__(self, precision, format)

    def data_cell(self, row: int, col: int) -> str:
        try:
            scalar: Scalar = self._displayed_database.i[row]
            if col == 0:
                return self.value_to_string(scalar.value)
            elif col == 1:
                return self.value_to_string(scalar.relax)
            elif col == 2:
                return self.value_to_string(scalar.std)
            elif col == 3:
                if scalar.std != 0:
                    return self.value_to_string(scalar.value / scalar.std)
                else:
                    return NAN_REP
            else:
                return ""
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))
            return ""

    def set_value(self, row: int, column: int, value: str) -> bool:
        try:
            scalar: Scalar = self._displayed_database.i[row]

            if column == 0:
                scalar.value = NA if value == NAN_REP or value == "" else float(value)
            elif column == 1:
                scalar.relax = float(value)

            return True
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))
            return False
    
    @Slot()
    def update_display_values(self):
        self.reset()


class TablesModel(IodeAbstractTableModel):
    def __init__(self, parent: QObject = None):
        super().__init__(["Table"], IodeType.TABLES, tables, parent)

    def data_cell(self, row: int, col: int) -> str:
        try:
            return self._displayed_database.get_title(row)
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))
            return ""

    def set_value(self, row: int, column: int, value: str) -> bool:
        try:
            # here 'value' represents the title of the table
            table: Table = self._displayed_database.i[row]
            table.title = value
            return True
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))
            return False


class VariablesModel(IodeAbstractTableModel, IodeNumericalTableModel):
    def __init__(self, precision: int=2, format: str='f', mode: VarsMode = VarsMode.LEVEL, 
                 parent: QObject = None):
        IodeAbstractTableModel.__init__(self, [], IodeType.VARIABLES, variables, parent)
        IodeNumericalTableModel.__init__(self, precision, format)
        self.set_mode(mode)

    def data_cell(self, row: int, col: int) -> str:
        try:
            value = self._displayed_database.i[row, col]
            return self.value_to_string(value)
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))
            return ""

    def set_value(self, row: int, column: int, value: float) -> bool:
        try:
            self._displayed_database.i[row, column] = value
            return True
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))
            return False

    @Slot()
    def update_display_values(self):
        self.reset()

    # override base method
    @Slot()
    def reset(self):
        self.column_names = self._displayed_database.periods
        super().reset()

    @Slot(VarsMode)
    def set_mode(self, mode: VarsMode) -> None:
        if self._database is not None:
            self._database.mode = mode
        if self._database_subset is not None:
            self._database_subset.mode = mode
        self.reset()
