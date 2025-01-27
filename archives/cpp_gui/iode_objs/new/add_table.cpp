#include "add_table.h"

AddTableDialog::AddTableDialog(KDBTables* database, QWidget* parent) : IodeSettingsDialog(parent), database(database)
{
    setupUi(this);

	completer = new IodeCompleter(false, false, {SCALARS, VARIABLES}, textEdit_lec);
	textEdit_lec->setCompleter(completer);

    lineName = new WrapperIodeNameEdit(label_name->text(), *lineEdit_name, TABLES, REQUIRED_FIELD);
    spinNbColumns = new WrapperSpinBox(label_nb_columns->text(), *spinBox_nb_columns, REQUIRED_FIELD);
    lineTitle = new WrapperQLineEdit(label_table_title->text(), *lineEdit_table_title, REQUIRED_FIELD);
    textLEC = new WrapperQPlainTextEdit(label_lec->text(), *textEdit_lec, REQUIRED_FIELD);
    checkBoxMode = new WrapperCheckBox("Mode", *checkBox_mode, REQUIRED_FIELD);
    checkBoxFiles = new WrapperCheckBox("Files", *checkBox_files, REQUIRED_FIELD);
    checkBoxDate = new WrapperCheckBox("Date", *checkBox_date, REQUIRED_FIELD);

    mapFields["NbColumns"] = spinNbColumns;
    mapFields["Mode"] = checkBoxMode;
    mapFields["Files"] = checkBoxFiles;
    mapFields["Date"] = checkBoxDate;

    className = "TAB_ADD_TABLE";
    loadSettings();
}

AddTableDialog::~AddTableDialog()
{
    delete lineName;
    delete spinNbColumns;
    delete lineTitle;
    delete textLEC;
    delete checkBoxMode;
    delete checkBoxFiles;
    delete checkBoxDate;

    delete completer;
}

void AddTableDialog::add()
{
    try
    {
        std::string name = lineName->extractAndVerify().toStdString();
        int nb_columns = spinNbColumns->extractAndVerify();
        std::string title = lineTitle->extractAndVerify().toStdString();
        std::string lecs = textLEC->extractAndVerify().toStdString();
        bool mode = checkBoxMode->extractAndVerify();
        bool files = checkBoxFiles->extractAndVerify();
        bool date = checkBoxDate->extractAndVerify();
		// check if already exists in the global IODE database since the variable 'database' may represents only a subset
		if(Tables.contains(name))
		{
			int res = QMessageBox::question(nullptr, "WARNING", QString::fromStdString(name) + " already exists. Replace it ?");
			if(res != QMessageBox::StandardButton::Yes)
			{
				this->reject();
				return;
			}
            Table table(nb_columns, title, lecs, mode, files, date);
            Tables.update(name, table);
		}
        else
        {
            emit newObjectInserted(QString::fromStdString(name));
            database->add(name, nb_columns, title, lecs, mode, files, date);
        }
        
        this->accept();
    }
    catch (const std::exception& e)
    {
        QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
    }
}

void AddTableDialog::help()
{
    
    QDesktopServices::openUrl(url_manual);
}
