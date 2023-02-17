#include "report_editor.h"


ReportEditor::ReportEditor(QWidget *parent) : TextEditor(parent)
{
    highlighter = new QIodeHighlighter(this->document());
}

ReportEditor::~ReportEditor()
{
    delete highlighter;
}

void ReportEditor::run(const QString& filepath)
{
    int success = B_ReportExec(filepath.toStdString().data());
    if(success != 0) B_display_last_error();
}