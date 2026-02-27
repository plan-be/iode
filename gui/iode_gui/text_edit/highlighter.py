from qtpy.QtCore import QRegularExpression, QSettings
from qtpy.QtGui import QSyntaxHighlighter, QTextCharFormat, QColor
from qtpy.QtWidgets import QTextEdit, QMessageBox

from iode_gui.color_theme import ColorTheme
from iode_gui.settings import get_color_theme

from typing import List
from iode.reports import build_command_functions_list


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
        self.update_colors()

    def update_colors(self):
        color_theme: ColorTheme = get_color_theme()
        self.set_color_theme(color_theme)

    def set_color_theme(self, color_theme: ColorTheme):
        """
        Update the highlighter with a new color theme.
        Args:
            color_theme (ColorTheme): The new color theme to apply.
        """
        try:
            self.highlighting_rules.clear()
            # report command -> $ or # (group 0 -> Report internal fns)
            self._add_rule_commands(0, color_theme.report_internal_functions)
            # report command -> $ or # (group 1)
            self._add_rule_commands(1, color_theme.report_commands)
            # report function -> @
            self._add_rule(r"(@\w+\([^\)]*\))", color_theme.report_functions)
            # report comment
            self._add_rule(r"([$#]\s[^\n]*)", color_theme.report_comments)
            # %<expression>%
            self._add_rule(r"(%[^%]+%)", color_theme.report_macros)
            # {expression}
            self._add_rule(r"(\{(.*?)\})", color_theme.report_expressions)
            # update the widget with the new color theme
            self.rehighlight()
        except Exception as e:
            QMessageBox.warning(None, "Error", f"Highlighter: failed to set color theme: {e}")

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
        for func in build_command_functions_list(group):
            q_func = r"[$]!?\b" + func[1:] + r"\b"
            self._add_rule(q_func, color)

        # #-functions
        for func in build_command_functions_list(group, True):
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

