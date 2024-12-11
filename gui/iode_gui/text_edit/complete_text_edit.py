from PySide6.QtWidgets import QPlainTextEdit
from PySide6.QtGui import QKeyEvent, QTextCursor

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
        # NOTE: tc.select(QTextCursor::WordUnderCursor) selects only
        #       alpha characters (i.e. the selection will not include
        #       characters such as $, # and @)
        tc.select(QTextCursor.SelectionType.WordUnderCursor)
        word = tc.selectedText()

        # NOTE: we need to add the lines below because we need to know
        #       if there is a character $, # or @ before the "word"
        tc.movePosition(QTextCursor.MoveOperation.StartOfWord)
        pos = tc.positionInBlock()
        ch = block[pos - 1] if pos > 0 else ''
        completion_prefix = f"{ch}{word}" if ch in self.iode_func_prefixes else word

        return completion_prefix