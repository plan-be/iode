from PySide6.QtCore import Qt, QSettings, Slot, Signal
from PySide6.QtWidgets import QTextEdit, QMessageBox
from PySide6.QtGui import QKeyEvent

from iode_gui.abstract_main_window import AbstractMainWindow
from iode_gui.settings import get_settings, class_name_to_settings_group_name
from iode_gui.text_edit.complete_line_edit import IodeAutoCompleteLineEdit

from typing import List
from iode import execute_command


class IodeCommandLine(IodeAutoCompleteLineEdit):
    """
    Special line field to execute an Iode command. It works like a 1-line terminal.

    The command is executed when the user presses Enter. If the command succeeded:
    - the command is added to the list of executed commands
    - the text "> <command>" is printed in the output panel
    - the content of the command field is erased
    If the command failed:
    - a warning box is displayed with error message
    - the error message is copied in the output panel

    Pressing up/down allows to reuse a previously executed command (like in a terminal).
    Auto-completion starts at the beginning of the line or after a whitespace or any characters which is not A-Z, a-z, 0-9, _, $, #, and @.
    """
    MAX_NB_COMMANDS_TO_REMEMBER = 20
    ask_compute_hash = Signal(bool)

    def __init__(self, parent=None):
        super().__init__(parent)
        self.main_window: AbstractMainWindow = None
        self.executed_commands_list: List[str] = []
        self.current_index: int = 0
        self.output: QTextEdit = None
        self.settings_group_name = class_name_to_settings_group_name(self)

    def setup(self, main_window: AbstractMainWindow):
        """
        Set up the IodeCommandLine instance.
        """
        self.main_window = main_window
        self.output = main_window.output
        self.setup_completer(main_window=main_window, report_commands=True, 
                             lec_functions=True, iode_types=-1)
        self.setPlaceholderText("Enter a command here...")

    # override the _handle_special_keys() method from 
    # the IodeWidgetWithCompleter class 
    def _handle_special_keys(self, event: QKeyEvent) -> bool:
        """
        Handle special keys' events.

        :param event: The key event.
        :return: Whether or not to exit the _key_press_event() method 
                 from the IodeWidgetWithCompleter class.
        """
        if event.key() == Qt.Key.Key_Up:
            if self.executed_commands_list:
                # get previous command from the list
                try:
                    if self.current_index > 0:
                        self.current_index -= 1
                    else:
                        self.current_index = len(self.executed_commands_list) - 1
                    previous_command = self.executed_commands_list[self.current_index]
                    self.setText(previous_command)
                    return True
                except Exception as e:
                    QMessageBox.warning(self, "Error", f"Failed to get previous command:\n{str(e)}")
        elif event.key() == Qt.Key.Key_Down:
            if self.executed_commands_list:
                # get next command from the list
                try:
                    if self.current_index < len(self.executed_commands_list) - 1:
                        self.current_index += 1
                    else:
                        self.current_index = 0
                    next_command = self.executed_commands_list[self.current_index]
                    self.setText(next_command)
                    return True
                except Exception as e:
                    QMessageBox.warning(self, "Error", f"Failed to get next command:\n{str(e)}")
        elif event.key() in (Qt.Key.Key_Enter, Qt.Key.Key_Return):
            self.run_command()
            return True
        
        return False

    def save_settings(self):
        """
        Store the last MAX_NB_COMMANDS_TO_REMEMBER executed commands 
        to the project settings file.
        """
        settings: QSettings = get_settings()
        if not settings:
            return
        
        # end all groups to be sure we are at the top level
        while settings.group():
            settings.endGroup()

        settings.beginGroup(self.settings_group_name)

        nb_commands_to_save = min(len(self.executed_commands_list), self.MAX_NB_COMMANDS_TO_REMEMBER)
        commands_to_save = self.executed_commands_list[-nb_commands_to_save:]
        if isinstance(commands_to_save, str):
            commands_to_save = [commands_to_save]
        settings.setValue("LAST_EXECUTED_COMMANDS", commands_to_save)

        settings.endGroup()

    def load_settings(self):
        """
        Load the list of the last MAX_NB_COMMANDS_TO_REMEMBER commands executed 
        in the previous GUI session.
        """
        settings: QSettings = get_settings()
        if not settings:
            return
    
        # end all groups to be sure we are at the top level
        while settings.group():
            settings.endGroup()

        settings.beginGroup(self.settings_group_name)

        commands_to_load = settings.value("LAST_EXECUTED_COMMANDS", [])
        if isinstance(commands_to_load, str):
            commands_to_load = [commands_to_load]
        self.executed_commands_list = commands_to_load
        self.current_index = len(self.executed_commands_list)

        settings.endGroup()

    @Slot()
    def run_command(self):
        """
        Executes the command in the command line and displays the output in the output panel.
        If the command is empty, the method returns without doing anything.
        If the command execution is successful, the command is added to the list of executed commands.
        If the command execution fails, an error message is displayed in the output panel.
        """
        cmd = self.text()
        if not cmd:
            return

        # ask to compute the hash of all databases
        self.ask_compute_hash.emit(True)

        # reset the input command line
        self.setText("")

        # try to execute the command
        try:
            execute_command(cmd)
            if len(self.executed_commands_list) > 0 and cmd != self.executed_commands_list[-1]:
                self.executed_commands_list.append(cmd)
            if len(self.executed_commands_list) > self.MAX_NB_COMMANDS_TO_REMEMBER:
                self.executed_commands_list = self.executed_commands_list[-self.MAX_NB_COMMANDS_TO_REMEMBER:]
            # move current index to the end of the list
            self.current_index = len(self.executed_commands_list) - 1
            # connected to MainWindow.update_tab_and_completer
            self.returnPressed.emit()
            self.save_settings()
        except Exception as e:
            self.output.append("Failed to execute " + cmd)
            self.output.append(str(e))
            QMessageBox.warning(None, "WARNING", f"{str(e)}\n")
