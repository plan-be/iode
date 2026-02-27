from qtpy.QtCore import QStringListModel, QKeyCombination, Qt, Slot
from qtpy.QtWidgets import QCompleter, QLineEdit, QWidget
from qtpy.QtGui import QKeyEvent

from iode_gui.abstract_main_window import AbstractMainWindow

from iode import IodeType, comments, equations, identities, lists, scalars, tables, variables
from iode.reports import build_command_functions_list, build_report_functions_list, build_lec_functions_list
from typing import Union, List, Set, Any


class IodeCompleter(QCompleter):
    """
    A custom QCompleter for Iode objects.
    """

    def __init__(self, widget: QWidget, report_commands: bool=True, lec_functions: bool=False, 
                 iode_types: Union[int, IodeType, List[Union[int, IodeType]]]=None):
        """
        Initializes the IodeCompleter.

        :param report_commands: Whether to include report commands.
        :param lec_functions: Whether to include LEC functions.
        :param iode_types: The type(s) of Iode object to include.
        """
        super().__init__()
        self.setWidget(widget)

        self.list_model = QStringListModel(self)
        self.setModel(self.list_model)
        self.setCaseSensitivity(Qt.CaseSensitivity.CaseInsensitive)
        self.setCompletionMode(QCompleter.CompletionMode.PopupCompletion)

        self.report_commands_list: List[str] = self._build_report_commands_list()
        self.lec_functions_list: List[str] = self.build_lec_functions_list()
        self.iode_databases: List[Any] = [comments, equations, identities, lists, 
                                          scalars, tables, variables] 
        self._iode_types_handled: Set[int] = set()

        self._report_commands: bool = report_commands
        self._lec_functions: bool = lec_functions
        self._update_model: bool = False

        if iode_types is not None:
            # If iode_types is -1, then all IODE types are handled
            if isinstance(iode_types, int) and iode_types == -1:
                self.iode_types = iode_types
            else:
                if not isinstance(iode_types, list):
                    iode_types = [iode_types]
                for iode_type in iode_types:
                    self.iode_types = iode_type

        self.update_iode_objects_list_names()
        self._update_model = True

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

        func_list = build_command_functions_list(-1)
        report_commands_list.extend(func_list)

        func_list = build_command_functions_list(-1, True)
        report_commands_list.extend(func_list)

        func_list = build_report_functions_list()
        report_commands_list.extend(func_list)

        report_commands_list.sort()
        return report_commands_list

    def build_lec_functions_list(self) -> List[str]:
        """
        Builds the list of LEC functions.
        """
        lec_functions_list: List[str] = []
        func_list = build_lec_functions_list()
        lec_functions_list.extend(func_list)

        lec_functions_list.sort()
        return lec_functions_list

    @property
    def update_model(self):
        return self._update_model
    
    @update_model.setter
    def update_model(self, value: bool):
        self._update_model = value

    @property
    def iode_types(self) -> List[int]:
        return list(self._iode_types_handled)

    @iode_types.setter
    def iode_types(self, value: Union[int, IodeType]):
        # If iode_types is -1, then all IODE types are handled.
        if isinstance(value, int) and value == -1:
            self._iode_types_handled = set(range(len(self.iode_databases)))
        else:
            if isinstance(value, IodeType):
                value = int(value)
            self._iode_types_handled.add(value)
        
        if self.update_model:
            self.update_iode_objects_list_names()
    
    def reset_iode_types(self):
        self._iode_types_handled = set()
        if self.update_model:
            self.update_iode_objects_list_names()

    @Slot()
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

        # Build list of all IODE objects
        object_names: List[str] = []
        for iode_type in self.iode_types:
            object_names += self.iode_databases[iode_type].names
        # remove duplicates
        object_names = set(object_names)
        # Sort object names
        object_names = list(object_names)
        object_names.sort()

        # Add object names to the list
        names_list += object_names

        # Update completer
        self.list_model.setStringList(names_list)


class IodeWidgetWithCompleter():
    """
    Special text widget which tries to auto-complete user input.

    Auto-completion starts at the beginning of the line
    or after a whitespace or any characters which is not
    A-Z, a-z, 0-9, _, $, #, and @.
    """
    iode_func_prefixes = ['@', '#', '$']

    def __init__(self, completer_enabled: bool = True):
        self._completer: IodeCompleter = None
        self._completer_enabled = completer_enabled

    def setup_completer(self, main_window: AbstractMainWindow=None, report_commands: bool=False, 
                        lec_functions: bool=False, iode_types: Union[int, IodeType, List[IodeType]]=None):
        self._completer = IodeCompleter(self, report_commands, lec_functions, iode_types)
        if main_window is not None:
            main_window.update_completer_requested.connect(self.update_completer)
        self._completer.activated.connect(self.insert_completion)

    def enable_autocomplete(self, enabled: bool):
        self._completer_enabled = enabled

    def _handle_special_keys(self, event: QKeyEvent) -> bool:
        return False

    def _key_press_event(self, super_cls, event: QKeyEvent):
        if self._completer is None:
            self._handle_special_keys(event)
            return 
        
        if not self._completer_enabled:
            super_cls.keyPressEvent(event)
            return
        
        if self._completer.popup().isVisible():
            # The following keys are forwarded by the completer to the widget
            if event.key() in [Qt.Key.Key_Enter, Qt.Key.Key_Return, Qt.Key.Key_Escape, 
                               Qt.Key.Key_Tab, Qt.Key.Key_Backtab]:
                event.ignore()
                return  # let the completer do default behavior
            elif event.key() == Qt.Key.Key_Space:
                self._completer.popup().hide()
                super_cls.keyPressEvent(event)
                return

        exit: bool = self._handle_special_keys(event)
        if exit:
            return

        # CTRL + Space forces auto-completion
        force_autocomplete = event.keyCombination() == QKeyCombination(Qt.KeyboardModifier.ControlModifier | Qt.Key.Key_Space)
        # see https://doc.qt.io/qt-6/qt.html#Key-enum for maximal value of letter + digit key
        if force_autocomplete or event.key() == Qt.Key.Key_Backspace or event.key() < 255:
            super_cls.keyPressEvent(event)

            self._completer.setCompletionPrefix(self._text_under_cursor())
            if force_autocomplete or len(self._completer.completionPrefix()):
                if isinstance(self, QLineEdit):
                    self._completer.complete()
                else:
                    cr = self.cursorRect()
                    cr.setWidth(self._completer.popup().sizeHintForColumn(0) + 
                                self._completer.popup().verticalScrollBar().sizeHint().width())
                    self._completer.complete(cr)
            else:
                self._completer.popup().hide()
        else:
            super_cls.keyPressEvent(event)

    def _text_under_cursor(self) -> str:
        raise NotImplementedError()
    
    def insert_completion(self, completion: str):
        raise NotImplementedError()

    @Slot()
    def update_completer(self):
        self._completer.update_iode_objects_list_names()
