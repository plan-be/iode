from PySide6.QtWidgets import QLineEdit
from PySide6.QtGui import QKeyEvent

from .completer import IodeWidgetWithCompleter


class IodeAutoCompleteLineEdit(QLineEdit, IodeWidgetWithCompleter):
    """
    Special line field which tries to auto-complete user input.
    """
    def __init__(self, parent=None, completer_enabled: bool = True):
        QLineEdit.__init__(self, parent)
        IodeWidgetWithCompleter.__init__(self, completer_enabled)
        self.setup_completer()

    # override QLineEdit method
    def keyPressEvent(self, event: QKeyEvent):
        """
        Overridden method to handle key press events.
        """
        self._key_press_event(super(), event)

    def insert_completion(self, completion: str):
        """
        Insert a completion into the line edit.

        :param completion: The completion to insert.
        """
        text = self.text()
        if not len(text):
            return

        # delete completion prefix
        for _ in range(len(self._completer.completionPrefix())):
            self.backspace()

        # replaces with full function or Iode object name
        self.insert(completion)
