from PyQt6.QtWidgets import QLineEdit, QTextEdit, QCompleter
from PyQt6.QtGui import QKeyEvent
from PyQt6.QtCore import Qt, QSettings, pyqtSignal, pyqtSlot


class IodeCommandLine(QLineEdit):
    """
    Special line field to execute an Iode command.
    It works like a 1-line terminal.

    The command is executed when the user presses Enter.
    If the command succeeded:
    - the command is added to the list of executed commands
    - the text "> <command>" is printed in the output panel
    - the content of the command field is erased
    If the command failed:
    - a warning box is displayed with error message
    - the error message is copied in the output panel

    Pressing up/down allows to reuse a previously executed command
    (like in a terminal).

    Auto-completion starts at the beginning of the line
    or after a whitespace or any characters which is not
    A-Z, a-z, 0-9, _, $, #, and @.
    """

    MAX_NB_COMMANDS_TO_REMEMBER = 20
    SETTINGS_GROUP_NAME = "IODE_COMMAND_LINE"

    ask_compute_hash = pyqtSignal(bool)

    def __init__(self, parent=None):
        super().__init__(parent)
        self.executed_commands_list = []

    def setup(self, output: QTextEdit, command_completer: QCompleter):
        self.output = output
        self.setCompleter(command_completer)
        self.load_settings()

    def handleSpecialKeys(self, event):
        """
        Handle special keys.

        :param event: The key event.
        :return: Whether or not to exit keyPressEvent().
        """
        # Implementation here

    def save_settings(self):
        """
        Store the last MAX_NB_COMMANDS_TO_REMEMBER executed commands to the project settings file.
        """
        settings = QSettings()
        settings.beginGroup(self.SETTINGS_GROUP_NAME)
        settings.setValue("executed_commands_list", self.executed_commands_list)
        settings.endGroup()

    def load_settings(self):
        """
        Load the list of the last MAX_NB_COMMANDS_TO_REMEMBER commands executed in the previous GUI session.
        """
        settings = QSettings()
        settings.beginGroup(self.SETTINGS_GROUP_NAME)
        self.executed_commands_list = settings.value("executed_commands_list", [])
        settings.endGroup()

    @pyqtSlot()
    def run_command(self):
        # Implementation here
        pass
