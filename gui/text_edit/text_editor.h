#pragma once
#include <QWidget>
#include <QDialog>
#include <QPainter>
#include <QLineEdit>
#include <QShortcut>
#include <QSettings>
#include <QTextBlock>
#include <QScrollBar> 
#include <QPushButton>
#include <QGridLayout>
#include <QFontMetricsF>
#include <QPlainTextEdit>

#include "utils.h"
#include "find_and_replace_dialog.h"
#include "complete_text_edit.h"


class LeftArea;


class IodeTextEditor : public IodeAutoCompleteTextEdit
{
    Q_OBJECT

    QWidget* leftArea;
    FindAndReplaceDialog* findReplaceDialog;

    QShortcut* findShortcut;
    QShortcut* replaceShortcut;
    QShortcut* duplicateShortcut;

public:
    IodeTextEditor(QWidget *parent = nullptr);
    ~IodeTextEditor();

    void leftAreaPaintEvent(QPaintEvent *event);
    int leftAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void popupFindReplaceBox(const bool find_only=true);

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

    /**
     * @brief open a box in the upper left of the text editor to search for a text occurences.
     * 
     */
    void openFindBox();

    /**
     * @brief open a box in the upper left of the text editor to find and replace a text.
     * 
     */
    void openReplaceBox();

    /**
     * @brief duplicate the current line
     * 
     */
    void duplicateLine();
};


/**
 * @brief Class dedicated to be used with the IodeTextEditor in order to display:
 *        - line numbers
 *        - breakpoints
 * 
 * @note See https://doc.qt.io/qt-6/qtwidgets-widgets-codeeditor-example.html  
 * 
 */
class LeftArea : public QWidget
{
    IodeTextEditor* editor;

public:
    LeftArea(IodeTextEditor* editor) : QWidget(editor), editor(editor)
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
