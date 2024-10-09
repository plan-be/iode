from PyQt6.QtCore import QStringListModel, Qt, pyqtSlot
from PyQt6.QtWidgets import QCompleter, QWidget
from PyQt6.QtGui import QStandardItemModel, QStandardItem

from iode import (IodeTypes, comments, equations, identities, lists, scalars, tables, variables, 
                  _build_command_functions_list, _build_report_functions_list, _build_lec_functions_list)
from typing import Union, List, Set, Any

class IodeCompleter(QCompleter):
    """
    A custom QCompleter for Iode objects.
    """

    def __init__(self, report_commands: bool=True, lec_functions: bool=False, 
                 iode_types: Union[IodeTypes, List[IodeTypes]]=None, parent=None):
        """
        Initializes the IodeCompleter.

        :param report_commands: Whether to include report commands.
        :param lec_functions: Whether to include LEC functions.
        :param iode_types: The type(s) of Iode object to include.
        :param parent: The parent QObject.
        """
        super().__init__(parent)

        self.list_model = QStringListModel(self)
        self.setModel(self.list_model)
        self.setCaseSensitivity(Qt.CaseSensitivity.CaseInsensitive)
        self.setCompletionMode(QCompleter.CompletionMode.PopupCompletion)

        self.report_commands_list: List[str] = self._build_report_commands_list()
        self.lec_functions_list: List[str] = self._build_lec_functions_list()
        self.iode_databases: List[Any] = [comments, equations, identities, lists, 
                                          scalars, tables, variables] 
        self.iode_types_handled: Set[int] = set()

        self._report_commands: bool = report_commands
        self._lec_functions: bool = lec_functions

        if iode_types is not None:
            if not isinstance(iode_types, list):
                iode_types = [iode_types]
            self.iode_types_handled = set(int(iode_type) for iode_type in iode_types)

        self.update_iode_objects_list_names()

    @property
    def report_commands(self) -> bool:
        return self._report_commands
    
    @report_commands.setter
    def report_commands(self, value: bool):
        self._report_commands = value
        self.update_iode_objects_list_names()

    @property
    def lec_functions(self) -> bool:
        return self._lec_functions
    
    @lec_functions.setter
    def lec_functions(self, value: bool):
        self._lec_functions = value
        self.update_iode_objects_list_names()

    def _build_report_commands_list(self) -> List[str]:
        """
        Builds the list of report commands.
        """
        report_commands_list: List[str] = []

        func_list = _build_command_functions_list(-1)
        report_commands_list.extend(func_list)

        func_list = _build_command_functions_list(-1, True)
        report_commands_list.extend(func_list)

        func_list = _build_report_functions_list()
        report_commands_list.extend(func_list)

        report_commands_list.sort()
        return report_commands_list

    def _build_lec_functions_list(self) -> List[str]:
        """
        Builds the list of LEC functions.
        """
        lec_functions_list: List[str] = []
        func_list = _build_lec_functions_list()
        lec_functions_list.extend(func_list)

        lec_functions_list.sort()
        return lec_functions_list

    def handle_iode_type(self, iode_type: IodeTypes, update_model: bool=True):
        self.iode_types_handled.add(int(iode_type))
        if update_model:
            self.update_iode_objects_list_names()
    
    def reset_iode_types(self, update_model: bool=True):
        self.iode_types_handled = set()
        if update_model:
            self.update_iode_objects_list_names()

    @pyqtSlot()
    def update_iode_objects_list_names(self):
        """
        Updates the list of Iode object names currently loaded in the memory
        """
        names_list: List[str] = []

        # Concatenate report commands/functions and Iode object names
        if self.report_commands:
            names_list += self.report_commands_list

        if self.lec_functions:
            names_list += self.lec_functions_list

        # Build list of all Iode objects
        object_names: List[str] = []
        for iode_type in self.iode_types_handled:
            object_names += self.iode_databases[iode_type].names
        # Sort and remove duplicates from object names
        object_names = set(object_names)
        object_names = list(object_names)
        object_names.sort()

        # Add object names to the list
        names_list += object_names

        # Update completer
        self.list_model.setStringList(names_list)
