#include "tab_text_abstract.h"


AbstractTextWidget::AbstractTextWidget(const IodeFileType fileType, const QString& filepath, QWidget* parent) 
    : IodeAbstractWidget(fileType, parent), filepath(filepath), editor_(nullptr), editor_2_(nullptr)
{
}

bool AbstractTextWidget::load__(IodeTextEditor* editor, const QString& filepath, const bool forceOverwrite)
{
    // The fileChanged signal from the QFileSystemWatcher member of the IodeAbstractTabWidget class 
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

QString AbstractTextWidget::save_(IodeTextEditor* editor, const QString& filepath)
{
    if (filepath.isEmpty()) 
        return filepath;

    // The fileChanged signal from the QFileSystemWatcher member of the IodeAbstractTabWidget class 
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

QString AbstractTextWidget::save()
{
    return save(filepath);
}

QString AbstractTextWidget::saveAs_()
{
    QString newFilepath = QFileDialog::getSaveFileName(this, "Save As", nullptr, filter);
    if(newFilepath.isEmpty()) 
        return newFilepath;

    return save(newFilepath);
}

void AbstractTextWidget::split(const Qt::Orientation orientation)
{
    if(splitted_)
        return;

    if(splitter_->orientation() != orientation)
        splitter_->setOrientation(orientation);

    editor_2_->show();
    splitted_ = true;
}

void AbstractTextWidget::unsplit()
{
    if(!splitted_)
        return;

    editor_2_->hide();
    splitted_ = false;
}
