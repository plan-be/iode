#include "add_table.h"

QIodeAddTable::QIodeAddTable(QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    setupUi(this);

    lineName = new WrapperIodeNameEdit(label_name->text(), *lineEdit_name, I_TABLES, REQUIRED_FIELD);
    spinNbColumns = new WrapperSpinBox(label_nb_columns->text(), *spinBox_nb_columns, REQUIRED_FIELD);
    lineTitle = new WrapperQLineEdit(label_table_title->text(), *lineEdit_table_title, REQUIRED_FIELD);
    textLEC = new WrapperQTextEdit(label_lec->text(), *textEdit_lec, REQUIRED_FIELD);
    checkBoxMode = new WrapperCheckBox("Mode", *checkBox_mode, REQUIRED_FIELD);
    checkBoxFiles = new WrapperCheckBox("Files", *checkBox_files, REQUIRED_FIELD);
    checkBoxDate = new WrapperCheckBox("Date", *checkBox_date, REQUIRED_FIELD);
}

QIodeAddTable::~QIodeAddTable()
{
    delete lineName;
    delete spinNbColumns;
    delete lineTitle;
    delete textLEC;
    delete checkBoxMode;
    delete checkBoxFiles;
    delete checkBoxDate;
}

void QIodeAddTable::add()
{
    try
    {
        KDBTables kdb;

        std::string name = lineName->extractAndVerify().toStdString();
        int nb_columns = spinNbColumns->extractAndVerify();
        std::string title = lineTitle->extractAndVerify().toStdString();
        std::string lecs = textLEC->extractAndVerify().toStdString();
        bool mode = checkBoxMode->extractAndVerify();
        bool files = checkBoxFiles->extractAndVerify();
        bool date = checkBoxDate->extractAndVerify();

        kdb.add(name, nb_columns, title, lecs, mode, files, date);

        this->accept();
    }
    catch (const std::runtime_error& e)
    {
        QMessageBox::warning(static_cast<QWidget*>(parent()), tr("Warning"), tr(e.what()));
    }
}

void QIodeAddTable::help()
{
    QUrl url = get_url_iode_manual();
    QDesktopServices::openUrl(url);
}
