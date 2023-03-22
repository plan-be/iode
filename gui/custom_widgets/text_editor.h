#pragma once
#include <QWidget>
#include <QPainter>
#include <QSettings>
#include <QTextBlock>
#include <QFontMetricsF>
#include <QPlainTextEdit>

#include "utils.h"


class LeftArea;


class TextEditor : public QPlainTextEdit
{
    Q_OBJECT

    QWidget* leftArea;

public:
    TextEditor(QWidget *parent = nullptr);

    void leftAreaPaintEvent(QPaintEvent *event);
    int leftAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLeftAreaWidth(int newBlockCount);

    /**
     * @brief Called when the updateRequest signal is sent.
     *        If the text is scrolled horizontally, rect will cover the entire viewport area. 
     *        If the text is scrolled vertically, dy carries the amount of pixels the viewport was scrolled.
     * 
     * @param rect 
     * @param dy 
     */
    void updateLeftArea(const QRect &rect, int dy);
};


/**
 * @brief Class dedicated to be used with the TextEditor in order to display:
 *        - line numbers
 *        - breakpoints
 * 
 * @note See https://doc.qt.io/qt-6/qtwidgets-widgets-codeeditor-example.html  
 * 
 */
class LeftArea : public QWidget
{
    TextEditor* editor;

public:
    LeftArea(TextEditor* editor) : QWidget(editor), editor(editor)
    {}

    QSize sizeHint() const override
    {
        return QSize(editor->leftAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        editor->leftAreaPaintEvent(event);
    }
};
