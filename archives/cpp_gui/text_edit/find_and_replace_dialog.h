#pragma once

#include <QWidget>
#include <QDialog>
#include <QShortcut>
#include <QPlainTextEdit>
#include <QMessageBox>

#include "ui_find_and_replace_dialog.h"


/**
 * @brief Class dedicated to find and replace a given text in a document (text file or Iode report).
 * 
 * @note see https://doc.qt.io/qt-6/richtext-common-tasks.html#finding-text for an example
 * 
 */
class FindAndReplaceDialog: public QDialog, public Ui::FindAndReplaceDialog
{
    Q_OBJECT

    QPlainTextEdit* editor;
    QTextDocument* doc;

    QTextCharFormat allOccurenceFormat;
    QTextCharFormat currentOccurenceFormat;
    QTextCharFormat defaultFormat;

    QShortcut* exitShortcut;
    QShortcut* replaceShortcut;

    int fullHeight;
    int nbPrevious;
    int nbOccurences;

public:
    FindAndReplaceDialog(QPlainTextEdit* editor, QWidget *parent=nullptr) 
        : QDialog(parent, Qt::WindowStaysOnTopHint), editor(editor), nbPrevious(0), nbOccurences(0)
    {
        setupUi(this);

        doc = editor->document();

        defaultFormat = editor->currentCharFormat();

        // light yellow = #F1EB9C
        allOccurenceFormat.setBackground(QColor("#F1EB9C"));

        currentOccurenceFormat.setBackground(Qt::yellow);

        fullHeight = height();

        exitShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
        exitShortcut->setContext(Qt::ShortcutContext::WidgetShortcut);
        connect(exitShortcut, &QShortcut::activated, this, &FindAndReplaceDialog::close);

        replaceShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_R), this);
        replaceShortcut->setContext(Qt::ShortcutContext::WidgetShortcut);
        connect(replaceShortcut, &QShortcut::activated, this, [=](){ this->findOnly(false); });
    }

    ~FindAndReplaceDialog() 
    {
        delete exitShortcut;
        delete replaceShortcut;
    }

    void setTextToFind(const QString& text)
    {
        lineSearch->setText(text);
    }

protected:
    void closeEvent(QCloseEvent *event) override
    {
        clear();
        return QDialog::closeEvent(event);
    }

private:
    void clear()
    {
        QTextCursor cursor = editor->textCursor();
        int cursor_pos = cursor.position();
        cursor.select(QTextCursor::Document);
        cursor.setCharFormat(defaultFormat);
        cursor.clearSelection();
        cursor.setPosition(cursor_pos);
        editor->setTextCursor(cursor);
    }

    void setLabelNbOccurrences()
    {
        labelNbOccurrences->setText(QString::number(nbPrevious) + " of " + QString::number(nbOccurences));
    }

public slots:
    void onSearchEdited(QString text);
    bool next();
    bool previous();
    bool replace();
    void replaceAll();
    void findOnly(bool find_only);
};
