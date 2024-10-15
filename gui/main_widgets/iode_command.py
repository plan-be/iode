from PySide6.QtCore import Slot, Signal
from PySide6.QtWidgets import QLineEdit, QTextEdit, QCompleter


class IodeCommandLine(QLineEdit):
    
    ask_compute_hash = Signal(bool)

    def __init__(self, parent=None):
        super(IodeCommandLine, self).__init__(parent)

    def setup(self, output: QTextEdit, command_completer: QCompleter):
        # Set up the command line edit widget
        self.output = output
        self.command_completer = command_completer

    @Slot()
    def run_command(self):
        pass
