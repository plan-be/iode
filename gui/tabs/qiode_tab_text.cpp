#include "qiode_tab_text.h"


QIodeAbstractEditor::QIodeAbstractEditor(const EnumIodeFile fileType, const QString& filepath, QWidget* parent) 
    : AbstractTabWidget(fileType, filepath, parent) 
{   
    // layout
    layout = new QGridLayout(this);
    layout->setObjectName(QString::fromUtf8("layout"));
    layout->setContentsMargins(0, 0, 0, 0);
}

QIodeAbstractEditor::~QIodeAbstractEditor()
{
    delete editor;
}

void QIodeAbstractEditor::addEditorToLayout(int row)
{
    editor->setGeometry(QRect(0, 50, 800, 500));
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(editor->sizePolicy().hasHeightForWidth());
    editor->setSizePolicy(sizePolicy);
    editor->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);

    // -1 -> span over all rows/columns
    layout->addWidget(editor, row, 0, -1, -1);

    if(!filepath.isEmpty()) 
        load(filepath, true);
}

bool QIodeAbstractEditor::load_(const QString& filepath, const bool forceOverwrite)
{
    QFile file(filepath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(nullptr, "Error", file.errorString());
        return false;
    }

    QTextStream in(&file);
    editor->setPlainText(in.readAll());

    return true;
}

QString QIodeAbstractEditor::save(const QString& filepath)
{
    if (filepath.isEmpty()) return filepath;

    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(nullptr, "Error", file.errorString());
        return "";
    }

    QTextStream out(&file);
    out << editor->toPlainText() << "\n";
    file.close();

    return filepath;
}

QString QIodeAbstractEditor::save()
{
    return save(filepath);
}

QString QIodeAbstractEditor::saveAs_()
{
    QString newFilepath = QFileDialog::getSaveFileName(this, "Save As", nullptr, "Text files (*.txt, *.rtf, *.a2m)");
    if(newFilepath.isEmpty()) return newFilepath;
    return save(newFilepath);
}
