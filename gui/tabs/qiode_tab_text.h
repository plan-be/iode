#pragma once

#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QTextStream>
#include <QGridLayout>

#include "custom_widgets/text_editor.h"
#include "qiode_tab_abstract.h"


class QIodeAbstractEditor: public AbstractTabWidget
{
    Q_OBJECT

protected:
    QGridLayout* layout;
    TextEditor* editor;

protected:
    void addEditorToLayout(int row);

public:
    QIodeAbstractEditor(const EnumIodeFile fileType, const QString& filepath, QWidget* parent = nullptr);
    ~QIodeAbstractEditor();

    void update() {}
    bool load_(const QString& filepath, const bool forceOverwrite);
    QString save(const QString& filepath);
    QString save();
    QString saveAs_();
};


class QIodeTextWidget : public QIodeAbstractEditor
{
public:
    QIodeTextWidget(const EnumIodeFile fileType, const QString& filepath, QWidget* parent = nullptr) 
        : QIodeAbstractEditor(fileType, filepath, parent) 
    {
        editor = new TextEditor();
        addEditorToLayout(0);
    }
};
