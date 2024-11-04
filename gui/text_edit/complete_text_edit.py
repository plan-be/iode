from PySide6.QtWidgets import QPlainTextEdit
from PySide6.QtGui import QKeyEvent

from .completer import IodeWidgetWithCompleter


class IodeAutoCompleteTextEdit(QPlainTextEdit, IodeWidgetWithCompleter):
    """
    Special text field which tries to auto-complete user input.
    """
    def __init__(self, parent=None, completer_enabled: bool = True):
        QPlainTextEdit.__init__(self, parent)
        IodeWidgetWithCompleter.__init__(self, completer_enabled)
        self.setup_completer()

    # override QPlainTextEdit method
    def keyPressEvent(self, event: QKeyEvent):
        """
        Overridden method to handle key press events.
        """
        self._key_press_event(super(), event)

    def insert_completion(self, completion: str):
        """
        Inserts the completion into the text edit.
        """
        tc = self.textCursor()

        # delete completion prefix
        for _ in range(len(self._completer.completionPrefix())):
            tc.deletePreviousChar()

        # replaces with full function or Iode object name
        tc.insertText(completion)

        # set cursor after introduced function or Iode object name
        self.setTextCursor(tc)
