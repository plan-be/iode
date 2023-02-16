#include "text_editor.h"

TextEditor::TextEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    leftArea = new LeftArea(this);

    connect(this, &TextEditor::blockCountChanged, this, &TextEditor::updateLeftAreaWidth);
    connect(this, &TextEditor::updateRequest, this, &TextEditor::updateLeftArea);

    // set tab = 4 whitspaces
    setTabStopDistance(QFontMetricsF(this->font()).horizontalAdvance(' ') * 4);

    updateLeftAreaWidth(0);
}

void TextEditor::leftAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(leftArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) 
    {
        if (block.isVisible() && bottom >= event->rect().top()) 
        {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, leftArea->width(), fontMetrics().height(), Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

int TextEditor::leftAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) 
    {
        max /= 10;
        ++digits;
    }

    int space = 10 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}

void TextEditor::resizeEvent(QResizeEvent* event)
{
    QPlainTextEdit::resizeEvent(event);

    QRect cr = contentsRect();
    leftArea->setGeometry(QRect(cr.left(), cr.top(), leftAreaWidth(), cr.height()));
}

void TextEditor::updateLeftAreaWidth(int newBlockCount)
{
    setViewportMargins(leftAreaWidth(), 0, 0, 0);
}

void TextEditor::updateLeftArea(const QRect &rect, int dy)
{
    // scroll vertically
    if (dy)
        leftArea->scroll(0, dy);
    // scroll horizontally
    else
        leftArea->update(0, rect.y(), leftArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLeftAreaWidth(0);
}
