#include "report_editor.h"

ReportEditor::ReportEditor(QWidget *parent) : TextEditor(parent)
{

}

ReportEditor::~ReportEditor()
{

}

void ReportEditor::run(const QString& filepath)
{
    int success = B_ReportExec(filepath.toStdString().data());
    if(success != 0) B_display_last_error();
}