#pragma once

#include "utils.h"
#include "qiode_highlighter.h"
#include "text_editor.h"


class ReportEditor : public TextEditor
{
    Q_OBJECT

    QIodeHighlighter *highlighter;

public: 
    ReportEditor(QWidget *parent = nullptr);
    ~ReportEditor();

public slots:
    void run(const QString& filepath);
};
