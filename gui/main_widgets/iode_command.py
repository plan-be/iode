from PyQt6.QtCore import pyqtSlot, pyqtSignal
from PyQt6.QtWidgets import QLineEdit, QTextEdit, QCompleter


class IodeCommandLine(QLineEdit):
    
    ask_compute_hash = pyqtSignal(bool)

    def __init__(self, parent=None):
        super(IodeCommandLine, self).__init__(parent)

    def setup(self, output: QTextEdit, command_completer: QCompleter):
        # Set up the command line edit widget
        self.output = output
        self.command_completer = command_completer

    @pyqtSlot()
    def run_command(self):
        pass
