#include "tab_text_abstract.h"


QIodeAbstractEditor::QIodeAbstractEditor(const EnumIodeFile fileType, const QString& filepath, QWidget* parent) 
    : AbstractTabWidget(fileType, filepath, parent) 
{ 
}

bool QIodeAbstractEditor::load__(TextEditor* editor, const QString& filepath, const bool forceOverwrite)
{
    // The fileChanged signal from the QFileSystemWatcher member of the QIodeAbstractTabWidget class 
    // is sent when the content of a tab is saved to file. The reloadFile() method of the same class 
    // resets the cursor to the beginning and resets the stack of undos and redos.
    // We don't want that when saving the content of the tab to the corresponding file.
    if(savingFile)
    {
        savingFile = false;
        return false;
    }

    try
    {
        QFile file(filepath);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::critical(nullptr, "ERROR", file.errorString());
            return false;
        }

        QTextStream in(&file);
        QString fileContent = in.readAll();
        if(editor->toPlainText() == fileContent)
            return false;
        
        editor->setPlainText(fileContent);

        // updates modified status -> automatically calls setModified()
        editor->document()->setModified(false);

        return true;
    }
    catch(const std::exception& e)
    {
        QMessageBox::warning(nullptr, "WARNING", "Could not load file " + filepath + "\n\n" + QString(e.what()));
        return false;
    }
}

QString QIodeAbstractEditor::save_(TextEditor* editor, const QString& filepath)
{
    if (filepath.isEmpty()) 
        return filepath;

    // The fileChanged signal from the QFileSystemWatcher member of the QIodeAbstractTabWidget class 
    // is sent when the content of a tab is saved to file. The reloadFile() method of the same class 
    // resets the cursor to the beginning and resets the stack of undos and redos.
    savingFile = true;

    try
    {
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
    catch(const std::exception& e)
    {
        savingFile = false;
        QMessageBox::warning(nullptr, "WARNING", "Could not save tab content to file " + filepath + 
            "\n\n" + QString(e.what()));
        return "";
    }
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
