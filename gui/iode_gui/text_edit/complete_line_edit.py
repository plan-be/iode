from qtpy.QtWidgets import QLineEdit
from qtpy.QtGui import QKeyEvent, QTextCursor

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

    def _text_under_cursor(self) -> str:
        line = self.text().strip()
        if not line:
            return ""

        pos = self.cursorPosition()

        # selects the "word" under cursor
        # NOTE: cursorWordBackward(mark) selects only
        #       alpha characters (i.e. the selection will not include 
        #       characters such as $, # and @)
        self.cursorWordBackward(True)

        # NOTE: we need to add the lines below because we need to know
        #       if there is a character $, # or @ before the "word"
        if self.cursorPosition() > 0:
            ch = line[self.cursorPosition() - 1]
            # cursorBackward(mark): Moves the cursor back. If mark is true 
            # each character moved over is added to the selection
            if ch in ['$', '#', '@']:
                self.cursorBackward(True)

        str = self.selectedText()
        self.deselect()
        self.setCursorPosition(pos)

        return str