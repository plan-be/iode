from PySide6.QtCore import QStringListModel, QKeyCombination, Qt, Slot
from PySide6.QtWidgets import QCompleter, QLineEdit
from PySide6.QtGui import QKeyEvent, QTextCursor

from iode import IodeType, comments, equations, identities, lists, scalars, tables, variables
from iode.reports import build_command_functions_list, build_report_functions_list, build_lec_functions_list
from typing import Union, List, Set, Any


class IodeCompleter(QCompleter):
    """
    A custom QCompleter for Iode objects.
    """

    def __init__(self, report_commands: bool=True, lec_functions: bool=False, 
                 iode_types: Union[IodeType, List[IodeType]]=None, parent=None):
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
        self.lec_functions_list: List[str] = self.build_lec_functions_list()
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

    def handle_iode_type(self, iode_type: IodeType, update_model: bool=True):
        self.iode_types_handled.add(int(iode_type))
        if update_model:
            self.update_iode_objects_list_names()
    
    def reset_iode_types(self, update_model: bool=True):
        self.iode_types_handled = set()
        if update_model:
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


class IodeWidgetWithCompleter():
    """
    Special text widget which tries to auto-complete user input.

    Auto-completion starts at the beginning of the line
    or after a whitespace or any characters which is not
    A-Z, a-z, 0-9, _, $, #, and @.
    """
    iode_func_prefixes = ['@', '#', '$']

    def __init__(self, completer_enabled: bool = True):
        self._completer = IodeCompleter()
        self._completer_enabled = completer_enabled

    def setup_completer(self):
        self._completer.setWidget(self)
        self._completer.activated.connect(self.insert_completion)

    def enable_autocomplete(self, enabled: bool):
        self._completer_enabled = enabled

    def _key_press_event(self, super_cls, event: QKeyEvent):
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
        tc = self.textCursor()
        pos = tc.position()

        # select the whole line
        tc.select(QTextCursor.SelectionType.BlockUnderCursor)
        block = tc.selectedText().strip('\u2029')

        # line is empty
        if not len(block):
            return ''

        # NOTE: tc.select move the cursor to the end of the selection.
        #       we need to move it back to its original position
        tc.setPosition(pos)

        # selects the "word" under cursor
        # WARNING: tc.select(QTextCursor::WordUnderCursor) selects only
        #          alpha characters (i.e. the selection will not include
        #          characters such as $, # and @)
        tc.select(QTextCursor.SelectionType.WordUnderCursor)
        word = tc.selectedText()

        # NOTE: we need to add the lines below because we need to know
        #       if there is a character $, # or @ before the "word"
        tc.movePosition(QTextCursor.MoveOperation.StartOfWord)
        pos = tc.positionInBlock()
        ch = block[pos - 1] if pos > 0 else ''
        completion_prefix = f"{ch}{word}" if ch in self.iode_func_prefixes else word

        return completion_prefix
    
    def insert_completion(self, completion: str):
        raise NotImplementedError()
    
    def handle_iode_type(self, iode_type: IodeType):
        """
        Sets the iode type for the completer.
        """
        self._completer.handle_iode_type(iode_type)

    def include_iode_command(self, value: bool):
        """
        Includes iode commands in the completer if ok is True.
        """
        self._completer.report_commands = value

    def include_lec_functions(self, value: bool):
        """
        Includes lec functions in the completer if ok is True.
        """
        self._completer.lec_functions = value

    def update_completer(self):
        self._completer.update_iode_objects_list_names()
