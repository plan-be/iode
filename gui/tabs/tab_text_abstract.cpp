#include "tab_text_abstract.h"


QIodeAbstractEditor::QIodeAbstractEditor(const EnumIodeFile fileType, const QString& filepath, QWidget* parent) 
    : AbstractTabWidget(fileType, filepath, parent) 
{ 
}

bool QIodeAbstractEditor::load__(TextEditor* editor, const QString& filepath, const bool forceOverwrite)
{
    QFile file(filepath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(nullptr, "ERROR", file.errorString());
        return false;
    }

    QTextStream in(&file);
    editor->setPlainText(in.readAll());

    // updates modified status -> automatically calls setModified()
    editor->document()->setModified(false);

    return true;
}

QString QIodeAbstractEditor::save_(TextEditor* editor, const QString& filepath)
{
    if (filepath.isEmpty()) return filepath;

    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(nullptr, "ERROR", file.errorString());
        return "";
    }

    QTextStream out(&file);
    out << editor->toPlainText() << "\n";
    file.close();

    // updates modified status -> automatically calls setModified()
    editor->document()->setModified(false);

    return filepath;
}

QString QIodeAbstractEditor::save()
{
    return save(filepath);
}

QString QIodeAbstractEditor::saveAs_()
{
    QString newFilepath = QFileDialog::getSaveFileName(this, "Save As", nullptr, filter);
    if(newFilepath.isEmpty()) 
        return newFilepath;

    return save(newFilepath);
}
