from qtpy.QtWidgets import QWidget
from qtpy.QtGui import (QKeyEvent, QMouseEvent, QPaintEvent, QResizeEvent,
                        QFontMetricsF, QPainter, QShortcut, QKeySequence,
                        QTextBlock, QTextCursor)
from qtpy.QtCore import Qt, QRect, QSize, QPoint, QSettings, Slot

from .find_and_replace_dialog import FindAndReplaceDialog
from .complete_text_edit import IodeAutoCompleteTextEdit


class LeftArea(QWidget):
    pass


class IodeTextEditor(IodeAutoCompleteTextEdit):
    """
    A custom text editor widget that supports auto-completion and multiple
    cursors.

    Additional cursors can be added with Alt+click or Ctrl+Alt+Up/Down. The
    Alt+Shift+I shortcut adds a cursor at the end of every selected line.
    Escape returns to a single cursor.
    """

    INDENT = ' ' * 4

    def __init__(self, parent=None):
        """
        Initializes a new instance of the IodeTextEditor class.

        :param parent: The parent widget.
        """
        super().__init__(parent)

        # QPlainTextEdit only owns one cursor. Keep the other cursors here and
        # paint/edit them explicitly.
        self._extra_cursors: list[QTextCursor] = []

        self.left_area = LeftArea(self)

        self.blockCountChanged.connect(self.update_left_area_width)
        self.updateRequest.connect(self.update_left_area)

        # set tab = 4 whitespaces
        self.setTabStopDistance(QFontMetricsF(self.font()).horizontalAdvance(' ') * 4)

        self.update_left_area_width(0)

        # set font family
        user_settings = QSettings(QSettings.Scope.UserScope, self)
        font_family = user_settings.value("fontFamily", "defaultFontFamily")
        self.setStyleSheet(f"font-family: {font_family}")

        # prepare the find and replace dialog
        self.find_replace_dialog: FindAndReplaceDialog = FindAndReplaceDialog(self, parent)

        # create and connect shortcuts
        self.find_shortcut = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Key.Key_F), self)
        self.find_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.find_shortcut.activated.connect(self.open_find_box)
        
        self.replace_shortcut = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Key.Key_H), self)
        self.replace_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.replace_shortcut.activated.connect(self.open_replace_box)

        self.select_line_shortcut = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Key.Key_L), self)
        self.select_line_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.select_line_shortcut.activated.connect(self.select_line)

        self.duplicate_shortcut = QShortcut(QKeySequence(Qt.Modifier.SHIFT | Qt.Modifier.ALT | Qt.Key.Key_Down), self)
        self.duplicate_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.duplicate_shortcut.activated.connect(self.duplicate_line)

        self.delete_end_of_line_shortcut = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Key.Key_K), self)
        self.delete_end_of_line_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.delete_end_of_line_shortcut.activated.connect(self.delete_end_of_line)

        self.delete_line_shortcut = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Key.Key_U), self)
        self.delete_line_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.delete_line_shortcut.activated.connect(self.delete_line)

    def _all_cursors(self) -> list[QTextCursor]:
        """Return the primary cursor followed by the additional cursors."""
        return [QTextCursor(self.textCursor()), *self._extra_cursors]

    def _set_cursors(self, cursors: list[QTextCursor]):
        """Install a primary cursor and a de-duplicated set of extra ones."""
        unique_cursors: list[QTextCursor] = []
        positions: set[tuple[int, int]] = set()
        for cursor in cursors:
            cursor_position = (cursor.position(), cursor.anchor())
            if cursor_position not in positions:
                unique_cursors.append(cursor)
                positions.add(cursor_position)

        if not unique_cursors:
            return

        self.setTextCursor(unique_cursors[0])
        self._extra_cursors = [QTextCursor(cursor) for cursor in unique_cursors[1:]]
        self.viewport().update()

    def clear_extra_cursors(self):
        """Return the editor to its normal, single-cursor mode."""
        if self._extra_cursors:
            self._extra_cursors.clear()
            self.viewport().update()

    def _add_cursor_vertically(self, operation: QTextCursor.MoveOperation):
        """Add a cursor one line above or below the outermost cursor."""
        cursors = self._all_cursors()
        source = min(cursors, key=lambda cursor: cursor.position()) \
            if operation == QTextCursor.MoveOperation.Up \
            else max(cursors, key=lambda cursor: cursor.position())
        new_cursor = QTextCursor(source)
        new_cursor.clearSelection()
        if new_cursor.movePosition(operation):
            cursors.append(new_cursor)
            self._set_cursors(cursors)

    def _add_cursors_to_selected_lines(self):
        """Put a cursor at the end of every line in the current selection."""
        cursor = self.textCursor()
        if not cursor.hasSelection():
            return

        line_cursors: list[QTextCursor] = []
        for block in self._selected_blocks(cursor):
            line_cursor = QTextCursor(block)
            line_cursor.movePosition(QTextCursor.MoveOperation.EndOfBlock)
            line_cursors.append(line_cursor)
        self._set_cursors(line_cursors)

    def _selected_blocks(self, cursor: QTextCursor) -> list[QTextBlock]:
        """Return each document block touched by *cursor* exactly once."""
        start = cursor.selectionStart()
        end = cursor.selectionEnd()
        # A selection ending at the start of a block does not touch that block.
        last_position = end - 1 if end > start else end
        first_block = self.document().findBlock(start)
        last_block = self.document().findBlock(last_position)

        blocks: list[QTextBlock] = []
        block = first_block
        while block.isValid():
            blocks.append(block)
            if block == last_block:
                break
            block = block.next()
        return blocks

    def _change_indentation(self, cursors: list[QTextCursor], indent: bool):
        """Indent or unindent every line touched by *cursors*."""
        blocks = { block.blockNumber(): block for cursor in cursors
                   for block in self._selected_blocks(cursor)}
        edit_cursor = QTextCursor(cursors[0])
        edit_cursor.beginEditBlock()
        for block_number in sorted(blocks, reverse=True):
            block = blocks[block_number]
            line_cursor = QTextCursor(block)
            if indent:
                line_cursor.insertText(self.INDENT)
                continue

            text = block.text()
            characters_to_remove = 1 if text.startswith('\t') else \
                min(len(text) - len(text.lstrip(' ')), len(self.INDENT))
            if characters_to_remove:
                line_cursor.movePosition(
                    QTextCursor.MoveOperation.Right,
                    QTextCursor.MoveMode.KeepAnchor,
                    characters_to_remove)
                line_cursor.removeSelectedText()
        edit_cursor.endEditBlock()
        self._set_cursors(cursors)

    def _edit_at_all_cursors(self, edit):
        """Apply one edit at every cursor as a single undo operation."""
        cursors = self._all_cursors()
        edit_cursor = QTextCursor(cursors[0])
        edit_cursor.beginEditBlock()
        # Editing backwards keeps earlier cursor positions stable. QTextCursor
        # instances automatically follow changes made before their positions.
        for cursor in sorted(cursors, key=lambda item: item.selectionStart(), reverse=True):
            edit(cursor)
        edit_cursor.endEditBlock()
        self._set_cursors(cursors)

    def _move_all_cursors(self, operation: QTextCursor.MoveOperation,
                          mode: QTextCursor.MoveMode):
        """Move all cursors using the same QTextCursor operation."""
        cursors = self._all_cursors()
        for cursor in cursors:
            cursor.movePosition(operation, mode)
        self._set_cursors(cursors)

    # override QPlainTextEdit method
    def keyPressEvent(self, event: QKeyEvent):
        """Handle multi-cursor shortcuts and simultaneous edits."""
        key = event.key()
        modifiers = event.modifiers()
        control = bool(modifiers & Qt.KeyboardModifier.ControlModifier)
        alt = bool(modifiers & Qt.KeyboardModifier.AltModifier)
        shift = bool(modifiers & Qt.KeyboardModifier.ShiftModifier)

        if control and alt and key in (Qt.Key.Key_Up, Qt.Key.Key_Down):
            operation = QTextCursor.MoveOperation.Up \
                if key == Qt.Key.Key_Up else QTextCursor.MoveOperation.Down
            self._add_cursor_vertically(operation)
            event.accept()
            return

        if alt and shift and key == Qt.Key.Key_I:
            self._add_cursors_to_selected_lines()
            event.accept()
            return

        if key == Qt.Key.Key_Escape and self._extra_cursors:
            self.clear_extra_cursors()
            event.accept()
            return

        # Let an open completer consume Tab in the usual way.
        completer_visible = self._completer is not None and \
            self._completer.popup().isVisible()
        if key in (Qt.Key.Key_Tab, Qt.Key.Key_Backtab) and not completer_visible:
            cursors = self._all_cursors()
            if key == Qt.Key.Key_Backtab or shift:
                self._change_indentation(cursors, False)
            elif any(cursor.hasSelection() for cursor in cursors):
                self._change_indentation(cursors, True)
            else:
                self._edit_at_all_cursors(
                    lambda cursor: cursor.insertText(self.INDENT))
            event.accept()
            return

        if not self._extra_cursors:
            super().keyPressEvent(event)
            return

        self._completer.popup().hide()

        if event.matches(QKeySequence.StandardKey.Paste):
            from qtpy.QtWidgets import QApplication
            text = QApplication.clipboard().text()
            self._edit_at_all_cursors(lambda cursor: cursor.insertText(text))
        elif key == Qt.Key.Key_Backspace:
            self._edit_at_all_cursors(lambda cursor: cursor.deletePreviousChar())
        elif key == Qt.Key.Key_Delete:
            self._edit_at_all_cursors(lambda cursor: cursor.deleteChar())
        elif key in (Qt.Key.Key_Enter, Qt.Key.Key_Return):
            self._edit_at_all_cursors(lambda cursor: cursor.insertText('\n'))
        elif key in (Qt.Key.Key_Left, Qt.Key.Key_Right, Qt.Key.Key_Up,
                     Qt.Key.Key_Down, Qt.Key.Key_Home, Qt.Key.Key_End):
            if key == Qt.Key.Key_Left:
                operation = QTextCursor.MoveOperation.PreviousWord if control \
                    else QTextCursor.MoveOperation.Left
            elif key == Qt.Key.Key_Right:
                operation = QTextCursor.MoveOperation.NextWord if control \
                    else QTextCursor.MoveOperation.Right
            elif key == Qt.Key.Key_Up:
                operation = QTextCursor.MoveOperation.Up
            elif key == Qt.Key.Key_Down:
                operation = QTextCursor.MoveOperation.Down
            elif key == Qt.Key.Key_Home:
                operation = QTextCursor.MoveOperation.Start if control \
                    else QTextCursor.MoveOperation.StartOfLine
            else:
                operation = QTextCursor.MoveOperation.End if control \
                    else QTextCursor.MoveOperation.EndOfLine
            mode = QTextCursor.MoveMode.KeepAnchor if shift \
                else QTextCursor.MoveMode.MoveAnchor
            self._move_all_cursors(operation, mode)
        elif event.text() and not control and not alt:
            text = event.text()
            self._edit_at_all_cursors(lambda cursor: cursor.insertText(text))
        else:
            # Undo, redo, copy and other standard shortcuts still belong to
            # QPlainTextEdit. Clear secondary cursors if the document may move.
            if event.matches(QKeySequence.StandardKey.Undo) or \
                    event.matches(QKeySequence.StandardKey.Redo):
                self.clear_extra_cursors()
            super().keyPressEvent(event)
            return

        event.accept()

    # override QPlainTextEdit method
    def mousePressEvent(self, event: QMouseEvent):
        """Use Alt+left-click to add or remove an additional cursor."""
        if event.button() == Qt.MouseButton.LeftButton and \
                event.modifiers() & Qt.KeyboardModifier.AltModifier:
            clicked_cursor = self.cursorForPosition(event.position().toPoint())
            cursors = self._all_cursors()
            matching_index = next(
                (index for index, cursor in enumerate(cursors)
                 if cursor.position() == clicked_cursor.position()), None)
            if matching_index is None:
                cursors.append(clicked_cursor)
            elif matching_index > 0:
                cursors.pop(matching_index)
            self._set_cursors(cursors)
            self.setFocus()
            event.accept()
            return

        self.clear_extra_cursors()
        super().mousePressEvent(event)

    # override QPlainTextEdit method
    def paintEvent(self, event: QPaintEvent):
        """Paint the additional cursor carets after the editor contents."""
        super().paintEvent(event)
        if not self._extra_cursors or not self.hasFocus():
            return

        painter = QPainter(self.viewport())
        painter.setPen(Qt.PenStyle.NoPen)
        painter.setBrush(self.palette().text())
        for cursor in self._extra_cursors:
            cursor_rect = self.cursorRect(cursor)
            cursor_rect.setWidth(max(2, self.cursorWidth()))
            painter.drawRect(cursor_rect)

    def left_area_paint_event(self, event: QPaintEvent):
        """
        Handles the paint event for the left area.

        :param event: The paint event.
        """
        painter = QPainter(self.left_area)
        painter.fillRect(event.rect(), Qt.GlobalColor.lightGray)

        block: QTextBlock = self.firstVisibleBlock()
        blockNumber = block.blockNumber()
        top = round(self.blockBoundingGeometry(block).translated(self.contentOffset()).top())
        bottom = top + round(self.blockBoundingRect(block).height())

        while block.isValid() and top <= event.rect().bottom():
            if block.isVisible() and bottom >= event.rect().top():
                number = str(blockNumber + 1)
                painter.setPen(Qt.GlobalColor.black)
                painter.drawText(0, top, self.left_area.width(), self.fontMetrics().height(), Qt.AlignmentFlag.AlignRight, number)

            block = block.next()
            top = bottom
            bottom = top + round(self.blockBoundingRect(block).height())
            blockNumber += 1

    def left_area_width(self):
        """
        Returns the width of the left area.
        The width includes the required space to display the line numbers.
        """
        digits: int = 1
        max_line_number: int = max(1, self.blockCount())
        while max_line_number >= 10:
            max_line_number /= 10
            digits += 1

        space = 10 + self.fontMetrics().horizontalAdvance('9') * digits

        return space

    # override base class method
    def resizeEvent(self, event: QResizeEvent):
        """
        Handles the resize event for the widget.

        :param event: The resize event.
        """
        super().resizeEvent(event)

        cr: QRect = self.contentsRect()
        self.left_area.setGeometry(QRect(cr.left(), cr.top(), self.left_area_width(), cr.height()))

    def popup_find_replace_box(self, find_only: bool=True):
        """
        Opens the find and replace dialog box.

        :param find_only: Whether to open the dialog in find-only mode.
        """
        if not self.find_replace_dialog.isVisible():
            # Glue the dialog to the top right border of the editor
            top_right: QPoint = self.mapToGlobal(self.viewport().rect().topRight())
            x = top_right.x() - self.find_replace_dialog.width()
            y = top_right.y()
            self.find_replace_dialog.move(x, y)

        # copy currently selected text in the search line edit
        cursor: QTextCursor = self.textCursor()
        self.find_replace_dialog.set_text_to_find(cursor.selectedText())
        # set if replace part is showed
        self.find_replace_dialog.find_and_replace(find_only)
        # show the find and replace box
        self.find_replace_dialog.setVisible(True)

    def update_left_area_width(self, newBlockCount):
        """
        Updates the width of the left area based on the number of blocks in the text editor.

        :param newBlockCount: The new number of blocks in the text editor.
        """
        self.setViewportMargins(self.left_area_width(), 0, 0, 0)

    def update_left_area(self, rect: QRect, dy: int):
        """
        Updates the left area of the text editor.

        :param rect: The rectangle that needs to be updated.
        :param dy: The amount that the viewport was scrolled vertically.
        """
        if dy:
            self.left_area.scroll(0, dy)
        else:
            self.left_area.update(0, rect.y(), self.left_area.width(), rect.height())

        if rect.contains(self.viewport().rect()):
            self.update_left_area_width(0)

    def open_find_box(self):
        """
        Opens the find dialog box.
        """
        self.popup_find_replace_box(True)

    def open_replace_box(self):
        """
        Opens the replace dialog box.
        """
        self.popup_find_replace_box(False)

    @Slot()
    def select_line(self):
        """
        Select the current line in the text editor.
        """
        cursor: QTextCursor = self.textCursor()
        cursor.select(QTextCursor.SelectionType.LineUnderCursor)
        self.setTextCursor(cursor)
    
    @Slot()
    def delete_end_of_line(self):
        """
        Deletes the text from the cursor position to the end of the line.
        """
        cursor: QTextCursor = self.textCursor()
        cursor.movePosition(QTextCursor.MoveOperation.EndOfLine, QTextCursor.MoveMode.KeepAnchor)
        cursor.removeSelectedText()

    @Slot()
    def delete_line(self):
        """
        Deletes the entire line where the cursor is located.
        """
        cursor: QTextCursor = self.textCursor()
        cursor.select(QTextCursor.SelectionType.LineUnderCursor)
        cursor.removeSelectedText()

    @Slot()
    def duplicate_line(self):
        """
        Duplicates the current line in the text editor.
        """
        cursor: QTextCursor = self.textCursor()
        pos: int = cursor.position()

        # select the current line
        cursor.select(QTextCursor.SelectionType.LineUnderCursor)
        current_line_text = cursor.selectedText() + "\n"
        line_length = len(current_line_text)

        # move to next line
        cursor.movePosition(QTextCursor.MoveOperation.StartOfLine)
        cursor.movePosition(QTextCursor.MoveOperation.Down)

        # insert the current line below
        cursor.insertText(current_line_text)

        # move the cursor back to its position in the line
        cursor.setPosition(pos + line_length)

        self.setTextCursor(cursor)


class LeftArea(QWidget):
    """
    Class dedicated to be used with the IodeTextEditor in order to display:
    - line numbers
    - breakpoints

    Note: See https://doc.qt.io/qt-6/qtwidgets-widgets-codeeditor-example.html
    """

    def __init__(self, editor: IodeTextEditor):
        """
        Initializes the LeftArea widget with the given IodeTextEditor instance.
        """
        super().__init__(editor)
        self.editor: IodeTextEditor = editor

    # override Base class method
    def sizeHint(self):
        """
        Returns the size hint for the left area widget.
        """
        return QSize(self.editor.left_area_width(), 0)

    # override Base class method
    def paintEvent(self, event: QPaintEvent):
        """
        Handles the paint event for the left area widget.
        """
        self.editor.left_area_paint_event(event)
