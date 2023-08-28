#include "text_editor.h"

TextEditor::TextEditor(QWidget *parent) : IodeAutoCompleteTextEdit(parent)
{
    leftArea = new LeftArea(this);

    connect(this, &TextEditor::blockCountChanged, this, &TextEditor::updateLeftAreaWidth);
    connect(this, &TextEditor::updateRequest, this, &TextEditor::updateLeftArea);

    // set tab = 4 whitspaces
    setTabStopDistance(QFontMetricsF(this->font()).horizontalAdvance(' ') * 4);

    updateLeftAreaWidth(0);

    // set font family
    QSettings user_settings(QSettings::UserScope, this);
    QString fontFamily = user_settings.value("fontFamily", defaultFontFamily).toString();
    this->setStyleSheet("font-family: " + fontFamily);

    // prepare the find and replace dialog.
    findReplaceDialog = new FindAndReplaceDialog(static_cast<QPlainTextEdit*>(this), parent);

    // create and connect shortcuts
    findShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_F), this);
    replaceShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_R), this);
    duplicateShortcut = new QShortcut(QKeySequence(Qt::SHIFT | Qt::ALT | Qt::Key_Down), this);

    findShortcut->setContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);
    replaceShortcut->setContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);
    duplicateShortcut->setContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);

    connect(findShortcut, &QShortcut::activated, this, &TextEditor::openFindBox);
    connect(replaceShortcut, &QShortcut::activated, this, &TextEditor::openReplaceBox);
    connect(duplicateShortcut, &QShortcut::activated, this, &TextEditor::duplicateLine);
}

TextEditor::~TextEditor()
{
    delete findShortcut;
    delete replaceShortcut;
    delete duplicateShortcut;

    delete findReplaceDialog;
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

void TextEditor::popupFindReplaceBox(const bool find_only)
{
    if(!findReplaceDialog->isVisible())
    {
        // Glue the dialog to the top right border of the editor
        QPoint topRight = mapToGlobal(viewport()->rect().topRight());
        int x = topRight.x() - findReplaceDialog->width();
        int y = topRight.y();
        findReplaceDialog->move(x, y);
    }
    // copy currently selected text in the search line edit
    QTextCursor cursor = textCursor();
    findReplaceDialog->setTextToFind(cursor.selectedText());
    // set if replace part is showed
    findReplaceDialog->findOnly(find_only);
    // show the find and replace box
    findReplaceDialog->setVisible(true);
}

void TextEditor::openFindBox()
{
    popupFindReplaceBox(true);
}

void TextEditor::openReplaceBox()
{
    popupFindReplaceBox(false);
}

void TextEditor::duplicateLine()
{
    QTextCursor cursor = textCursor();
    int pos = cursor.position();
    
    // select the current line
    cursor.select(QTextCursor::SelectionType::LineUnderCursor);
    QString currentLineText = cursor.selectedText() + "\n";
    int lineLength = currentLineText.size();

    // move to next line
    cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);

    // insert the current line below
    cursor.insertText(currentLineText);

    // move the cursor back to its position in the line
    cursor.setPosition(pos + lineLength);


    setTextCursor(cursor);
}
