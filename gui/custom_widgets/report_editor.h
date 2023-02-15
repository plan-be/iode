#pragma once

#include "utils.h"
#include "text_editor.h"


class ReportEditor : public TextEditor
{

public: 
    ReportEditor(QWidget *parent = nullptr);
    ~ReportEditor();

public slots:
    void run(const QString& filepath);
};
