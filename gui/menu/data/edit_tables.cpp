#include "edit_tables.h"


QIodeMenuDataEditTables::QIodeMenuDataEditTables(QWidget* parent)
    : QIodeSettings(parent)
{
    setupUi(this);

    completer = new QIodeCompleter(false, false, I_TABLES, textEdit_table_names);
    textEdit_table_names->setCompleter(completer);

    wTablesNames = new WrapperQPlainTextEdit(label_table_names->text(), *textEdit_table_names, REQUIRED_FIELD);
    mapFields["TablesNames"] = wTablesNames;

    className = "MENU_DATA_EDIT_TABLES";
    loadSettings();
}

QIodeMenuDataEditTables::~QIodeMenuDataEditTables()
{
    clear_all_reference_kdbs();
    foreach(QIodeGSampleTableView* view, tableViews) view->close();
    tableViews.clear();

    delete wTablesNames;
    delete completer;
}


void QIodeMenuDataEditTables::display()
{
    try
    {
        KDBVariables kdb_var;
        Sample smpl = kdb_var.get_sample();
        QString gsample = QString::fromStdString(smpl.start_period().to_string()) + ":" + QString::number(smpl.nb_periods());

        QString tableNames = wTablesNames->extractAndVerify();

        // Parses a string and replaces @filename and $listname by their contents
        char** c_table_names = B_ainit_chk(tableNames.toStdString().data(), NULL, 0);
        int nb_names = SCR_tbl_size((unsigned char**) c_table_names);
        for(int i=0; i < nb_names; i++)
        {
            QString tableName(c_table_names[i]); 
            QIodeGSampleTableView* view = new QIodeGSampleTableView(tableName, gsample, 3, "", this);
            tableViews.append(view);
            view->open();
        }
    }
    catch(const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
}

void QIodeMenuDataEditTables::help()
{
	QDesktopServices::openUrl(url_manual);
}
