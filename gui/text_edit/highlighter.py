from PySide6.QtCore import Qt, QRegularExpression
from PySide6.QtGui import QSyntaxHighlighter, QTextCharFormat, QColor
from PySide6.QtWidgets import QTextEdit

from typing import List
from iode import _build_command_functions_list


class HighlightingRule():
    def __init__(self, pattern: QRegularExpression, format: QTextCharFormat) -> None:
        self.pattern = pattern
        self.format = format


class IodeHighlighter(QSyntaxHighlighter):
    """
    A syntax highlighter for the Iode text editor.

    Note:
        See https://doc.qt.io/qt-6/qtwidgets-richtext-syntaxhighlighter-example.html
    """

    def __init__(self, parent=None):
        super(IodeHighlighter, self).__init__(parent)

        self.highlighting_rules: List[HighlightingRule] = []

        # report command -> $ or # (group 0 -> Report internal fns)
        self._add_rule_commands(0, Qt.GlobalColor.red)

        # report command -> $ or # (group 1)
        self._add_rule_commands(1, Qt.GlobalColor.darkBlue)

        # report function -> @
        self._add_rule(r"(@\w+\([^\)]*\))", Qt.GlobalColor.darkGreen)

        # report comment
        # color = olive
        self._add_rule(r"([$#]\s[^\n]*)", QColor.fromString("#808000"))

        # %<expression>%
        self._add_rule(r"(%[^%]+%)", Qt.GlobalColor.darkMagenta)

        # {expression}
        self._add_rule(r"(\{(.*?)\})", Qt.GlobalColor.darkGreen)


    def _add_rule(self, pattern: str, foreground_color: QColor):
        """
        Add a new highlighting rule.

        Args:
            pattern (str): The regular expression pattern to match.
            foreground_color (QColor): The color to apply to the matched text.
        """
        pattern = QRegularExpression(pattern, QRegularExpression.PatternOption.CaseInsensitiveOption)
        format = QTextCharFormat()
        format.setForeground(foreground_color)
        self.highlighting_rules.append(HighlightingRule(pattern, format))

    def _add_rule_commands(self, group: int, color: QColor):
        """
        Add a new command highlighting rule.

        Args:
            group (int): The group number of the command.
            color (QColor): The color to apply to the command.
        """
        # $-functions
        for func in _build_command_functions_list(group):
            q_func = r"[$]!?\b" + func[1:] + r"\b"
            self._add_rule(q_func, color)

        # #-functions
        for func in _build_command_functions_list(group, True):
            q_func = r"[#]!?\b" + func[1:] + r"\b"
            self._add_rule(q_func, color)

    # override QSyntaxHighlighter method
    def highlightBlock(self, text: str):
        """
        Highlight the given block of text.

        Args:
            text (str): The text to highlight.
        """
        for rule in self.highlighting_rules:
            expression = QRegularExpression(rule.pattern)
            iterator = expression.globalMatch(text)
            while iterator.hasNext():
                match = iterator.next()
                self.setFormat(match.capturedStart(), match.capturedLength(), rule.format)

