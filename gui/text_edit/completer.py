from PyQt6.QtCore import Qt, pyqtSlot, QStringListModel
from PyQt6.QtWidgets import QCompleter

from iode import IodeTypes, comments, equations, identities, lists, scalars, tables, variables 
from iode.iode_python import _build_command_functions_list, _build_report_functions_list, _build_lec_functions_list


_report_commands_list = _build_command_functions_list(-1) 
_report_commands_list += _build_command_functions_list(-1, True) 
_report_commands_list += _build_report_functions_list()
_report_commands_list.sort()

_lec_functions_list = _build_lec_functions_list()
_lec_functions_list.sort()


class IodeCompleter(QCompleter):
    def __init__(self, report_commands: bool=True, lec_functions: bool=False, iode_type: IodeTypes=None, parent=None):
        super().__init__(parent)

        self.report_commands = report_commands
        self.lec_functions = lec_functions

        self.setModel(QStringListModel())
        self.setCaseSensitivity(Qt.CaseInsensitive)
        self.setCompletionMode(QCompleter.PopupCompletion)

        self.set_iode_type(iode_type, True)

    @property
    def report_commands(self) -> bool:
        return self.report_commands_

    @report_commands.setter
    def report_commands(self, value: bool):
        self.report_commands_ = value
        self.update_iode_objects_list_names()

    @property
    def lec_functions(self) -> bool:
        return self.lec_functions_

    @lec_functions.setter
    def lec_functions(self, value: bool):
        self.lec_functions_ = value
        self.update_iode_objects_list_names()

    def add_iode_type(self, iode_type: IodeTypes, update_list_model=True):
        if iode_type == IodeTypes.COMMENTS:
            self.iode_databases.append(comments)
        elif iode_type == IodeTypes.EQUATIONS:
            self.iode_databases.append(equations)
        elif iode_type == IodeTypes.IDENTITIES:
            self.iode_databases.append(identities)
        elif iode_type == IodeTypes.LISTS:
            self.iode_databases.append(lists)
        elif iode_type == IodeTypes.SCALARS:
            self.iode_databases.append(scalars)
        elif iode_type == IodeTypes.TABLES:
            self.iode_databases.append(tables)
        elif iode_type == IodeTypes.VARIABLES:
            self.iode_databases.append(variables)

        if update_list_model:
            self.update_iode_objects_list_names()

    def set_iode_type(self, iode_type: IodeTypes=None, update_list_model=True):
        self.iode_databases.clear()

        if iode_type:
            self.add_iode_type(iode_type, False)
        else:
            self.iode_databases = [comments, equations, identities, lists, scalars, tables, variables]

        if update_list_model:
            self.update_iode_objects_list_names()

    @pyqtSlot()
    def update_iode_objects_list_names(self):
        """
        Update the list of IODE object names currently loaded in the memory 
        """
        # Create an empty list
        model_list = []

        # Concatenate report commands/functions and IODE object names
        if self.report_commands:
            model_list += _report_commands_list

        if self.lec_functions:
            model_list += _lec_functions_list

        # Build list of all IODE objects
        object_names = set()
        for iode_db in self.iode_databases:
            object_names |= set(self.iode_databases[iode_db].names)
        model_list += sorted(list(object_names))

        # Update completer
        self.model().setStringList(model_list)
