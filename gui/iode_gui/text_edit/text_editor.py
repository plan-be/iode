from PySide6.QtWidgets import QWidget
from PySide6.QtGui import (QPaintEvent, QResizeEvent, QFontMetricsF, QPainter, QShortcut, 
                           QKeySequence, QTextBlock, QTextCursor)
from PySide6.QtCore import Qt, QRect, QSize, QPoint, QSettings

from .find_and_replace_dialog import FindAndReplaceDialog
from .complete_text_edit import IodeAutoCompleteTextEdit


class LeftArea(QWidget):
    pass


class IodeTextEditor(IodeAutoCompleteTextEdit):
    """
    A custom text editor widget that supports auto-completion.
    """

    def __init__(self, parent=None):
        """
        Initializes a new instance of the IodeTextEditor class.

        :param parent: The parent widget.
        """
        super().__init__(parent)

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
        
        self.replace_shortcut = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Key.Key_R), self)
        self.replace_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.replace_shortcut.activated.connect(self.open_replace_box)
        
        self.duplicate_shortcut = QShortcut(QKeySequence(Qt.Modifier.SHIFT | Qt.Modifier.ALT | Qt.Key.Key_Down), self)
        self.duplicate_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.duplicate_shortcut.activated.connect(self.duplicate_line)

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
        self.find_replace_dialog.findAndReplace(find_only)
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
