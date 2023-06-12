#pragma once
#include <QPrinter>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QGridLayout>
#include <QPlainTextEdit>
#include <QPrintPreviewDialog>

#include "tab_abstract.h"
#include "custom_widgets/text_editor.h"


class QIodeAbstractEditor: public AbstractTabWidget
{
    Q_OBJECT

protected:
    QString filter;
    QPrinter printer;

public:
    QIodeAbstractEditor(const EnumIodeFile fileType, const QString& filepath, QWidget* parent = nullptr);

    void update() {}
    bool load__(TextEditor* editor, const QString& filepath, const bool forceOverwrite);
    virtual bool load_(const QString& filepath, const bool forceOverwrite) = 0;
    QString save_(TextEditor* editor, const QString& filepath);
    virtual QString save(const QString& filepath) = 0;
    QString save();
    QString saveAs_();
};
