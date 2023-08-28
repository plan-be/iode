#include "highlighter.h"


IodeHighlighter::IodeHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    // report command -> $ or # (group 0 -> Report internal fns)
    addRuleCommands(0, Qt::red);

    // report command -> $ or # (group 1)
    addRuleCommands(1, Qt::darkBlue);

    // report function -> @
    addRule(R"(@\w+\([^\)]*\))", Qt::darkGreen);

    // report comment
    // color = olive
    addRule(R"([$#]\s[^\n]*)", QColor::fromString("#808000"));

    // %<expression>%
    addRule(R"(%[^%]+%)", Qt::darkMagenta);

    // {expression}
    addRule(R"(\{(.*?)\})", Qt::darkGreen);
}

void IodeHighlighter::addRuleCommands(const int group, const QColor& color)
{
    QString q_func;

    // $-functions
    for(const std::string& func: build_command_functions_list(group))
    {
        q_func = "[$]!?\\b"  + QString::fromStdString(func).remove(0, 1) + "\\b";
        addRule(q_func, color);
    }

    // #-functions
    for(const std::string& func: build_command_functions_list(group, true))
    {
        q_func = "[#]!?\\b"  + QString::fromStdString(func).remove(0, 1) + "\\b";
        addRule(q_func, color);
    }
}

void IodeHighlighter::highlightBlock(const QString &text)
{
    foreach(const HighlightingRule& rule, highlightingRules) 
    {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) 
        {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
