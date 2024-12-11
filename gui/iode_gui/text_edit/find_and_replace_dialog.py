from PySide6.QtWidgets import QDialog, QPlainTextEdit
from PySide6.QtGui import (QTextCharFormat, QColor, QKeySequence, QShortcut, 
                           QTextCursor, QTextDocument)
from PySide6.QtCore import Qt

from .ui_find_and_replace_dialog import Ui_FindAndReplaceDialog


class FindAndReplaceDialog(QDialog):
    """
    Class dedicated to find and replace a given text in a document (text file or Iode report).

    Note: see https://doc.qt.io/qt-6/richtext-common-tasks.html#finding-text for an example
    """

    def __init__(self, editor: QPlainTextEdit, parent=None):
        """
        Initialize the FindAndReplaceDialog.

        :param editor: The QPlainTextEdit instance where the text will be searched and replaced.
        :param parent: The parent widget.
        """
        super().__init__(parent, Qt.WindowType.WindowStaysOnTopHint)

        self.ui = Ui_FindAndReplaceDialog()
        self.ui.previous = self.previous
        self.ui.next = self.next
        self.ui.replace = self.replace
        self.ui.replaceAll = self.replaceAll
        self.ui.onSearchEdited = self.onSearchEdited
        self.ui.setupUi(self)

        self.editor = editor
        self.doc = editor.document()

        self.default_format = editor.currentCharFormat()

        # light yellow = #F1EB9C
        self.all_occurrences_format = QTextCharFormat()
        self.all_occurrences_format.setBackground(QColor("#F1EB9C"))

        self.current_occurrence_format = QTextCharFormat()
        self.current_occurrence_format.setBackground(Qt.GlobalColor.yellow)

        self.full_height = self.height()
        self.nb_previous = 0
        self.nb_occurrences = 0

        self.exit_shortcut = QShortcut(QKeySequence(Qt.Key.Key_Escape), self)
        self.exit_shortcut.setContext(Qt.ShortcutContext.WidgetShortcut)
        self.exit_shortcut.activated.connect(self.close)

        self.replace_shortcut = QShortcut(QKeySequence(Qt.Key.Key_R | Qt.Modifier.CTRL), self)
        self.replace_shortcut.setContext(Qt.ShortcutContext.WidgetShortcut)
        self.replace_shortcut.activated.connect(lambda: self.findAndReplace(False))

    def closeEvent(self, event):
        """
        Override the closeEvent to clear the formatting before closing the dialog.

        :param event: The close event.
        """
        self.clear()
        return super().closeEvent(event)

    def set_text_to_find(self, text: str):
        """
        Set the text to find in the line search.

        :param text: The text to find.
        """
        self.ui.lineSearch.setText(text)

    def clear(self):
        """
        Clear the formatting of the editor.
        """
        cursor = self.editor.textCursor()
        cursor_pos = cursor.position()
        cursor.select(QTextCursor.SelectionType.Document)
        cursor.setCharFormat(self.default_format)
        cursor.clearSelection()
        cursor.setPosition(cursor_pos)
        self.editor.setTextCursor(cursor)

    def set_label_nb_occurrences(self):
        """
        Set the text of the labelNbOccurrences.
        """
        self.ui.labelNbOccurrences.setText(f"{self.nb_previous} of {self.nb_occurrences}")

    def onSearchEdited(self, text: str):
        """
        Slot for when the search text is edited.

        :param text: The text to find.
        """
        self.clear()

        if not text:
            self.ui.labelNbOccurrences.setText("No Results")
            return

        cursor = self.editor.textCursor()
        cursor_pos = cursor.position()
        self.nb_previous = 0
        self.nb_occurrences = 0

        doc_cursor = QTextCursor(self.doc)
        while not doc_cursor.isNull() and not doc_cursor.atEnd():
            doc_cursor = self.doc.find(text, doc_cursor, QTextDocument.FindFlag.FindWholeWords | 
                                       QTextDocument.FindFlag.FindCaseSensitively)
            if not doc_cursor.isNull():
                doc_cursor.mergeCharFormat(self.all_occurrences_format)
                current_pos = doc_cursor.position()
                self.nb_occurrences += 1
                if current_pos < cursor_pos:
                    self.nb_previous += 1

        self.set_label_nb_occurrences()

        cursor.setPosition(cursor_pos)
        self.editor.setTextCursor(cursor)

    def next(self):
        """
        Moves the cursor to the next occurrence of the search text.

        :return: True if the next occurrence was found, False otherwise.
        """
        cursor = self.editor.textCursor()
        if cursor.hasSelection():
            cursor.mergeCharFormat(self.all_occurrences_format)

        newCursor = self.doc.find(self.ui.lineSearch.text(), cursor, QTextDocument.FindFlag.FindWholeWords | 
                                  QTextDocument.FindFlag.FindCaseSensitively)
        if not newCursor.isNull():
            newCursor.mergeCharFormat(self.current_occurrence_format)
            self.editor.setTextCursor(newCursor)
            self.nb_previous += 1
            self.set_label_nb_occurrences()
            return True
        else:
            return False

    def previous(self):
        """
        Moves the cursor to the previous occurrence of the search text.

        :return: True if the previous occurrence was found, False otherwise.
        """
        cursor = self.editor.textCursor()
        if cursor.hasSelection():
            cursor.mergeCharFormat(self.all_occurrences_format)

        newCursor = self.doc.find(self.ui.lineSearch.text(), cursor, QTextDocument.FindFlag.FindWholeWords | 
                                  QTextDocument.FindFlag.FindCaseSensitively | QTextDocument.FindFlag.FindBackward)
        if not newCursor.isNull():
            newCursor.mergeCharFormat(self.current_occurrence_format)
            self.editor.setTextCursor(newCursor)

            self.nb_previous -= 1
            self.set_label_nb_occurrences()

            return True
        else:
            return False

    def replace(self):
        """
        Replaces the current occurrence of the search text with the replace text.

        :return: True if the replacement was successful, False otherwise.
        """
        cursor = self.editor.textCursor()
        if cursor.hasSelection():
            cursor.removeSelectedText()
            cursor.insertText(self.ui.lineReplace.text())
            self.nb_occurrences -= 1

            if self.next():
                self.nb_previous -= 1

            self.set_label_nb_occurrences()

            return True
        else:
            return False

    def replaceAll(self):
        """
        Slot for replacing all occurrences.
        """
        self.onSearchEdited(self.ui.lineSearch.text())
        if not self.nb_occurrences:
            return

        cursor = self.editor.textCursor()
        cursor_pos = cursor.position()

        cursor.movePosition(QTextCursor.MoveOperation.Start, QTextCursor.MoveMode.MoveAnchor)
        self.editor.setTextCursor(cursor)
        self.next()
        while self.replace():
            pass

        cursor.setPosition(cursor_pos)
        self.editor.setTextCursor(cursor)

        self.nb_previous = 0
        self.nb_occurrences = 0
        self.set_label_nb_occurrences()

    def findAndReplace(self, find_only: bool):
        """
        Slot for finding and replacing text.

        :param find_only: Whether to find only.
        """
        if find_only:
            self.ui.lineReplace.hide()
            self.ui.replaceButton.hide()
            self.ui.replaceAllButton.hide()
            self.resize(self.width(), self.ui.lineSearch.height() + 20)
        else:
            self.ui.lineReplace.setHidden(False)
            self.ui.replaceButton.setHidden(False)
            self.ui.replaceAllButton.setHidden(False)
            self.resize(self.width(), self.full_height)

        self.ui.lineSearch.setFocus()
