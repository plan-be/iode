#include "tab_text.h"


static const QStringList initialize_text_extensions()
{
    QStringList textExt;
    for(const std::string ext: v_text_ext)
        textExt << "." + QString::fromStdString(ext);
    return textExt;
}

const QStringList QIodeTextWidget::textExtensions = initialize_text_extensions();


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

    editor->document()->setModified(false);
    setModified(false);

    return filepath;
}

QString QIodeAbstractEditor::save()
{
    return save(filepath);
}

QString QIodeAbstractEditor::saveAs_()
{
    QString newFilepath = QFileDialog::getSaveFileName(this, "Save As", nullptr, filter);
    if(newFilepath.isEmpty()) return newFilepath;
    return save(newFilepath);
}
