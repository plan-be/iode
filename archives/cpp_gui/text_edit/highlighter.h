#pragma once

#include <QList>
#include <QColor>
#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextCharFormat>

#include "utils.h"


/**
 * @brief 
 * 
 * @note see https://doc.qt.io/qt-6/qtwidgets-richtext-syntaxhighlighter-example.html 
 */
class IodeHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QList<HighlightingRule> highlightingRules;

private:
    void addRule(const QString& pattern, const QColor& foregroundColor)
    {
        HighlightingRule rule;

        rule.pattern = QRegularExpression(pattern, QRegularExpression::CaseInsensitiveOption);

        QTextCharFormat format;
        format.setForeground(foregroundColor);
        rule.format = format;

        highlightingRules.append(rule);
    }

    void addRuleCommands(const int group, const QColor& color);

protected:
    void highlightBlock(const QString &text) override;

public:
    IodeHighlighter(QTextDocument *parent = nullptr);
};
