#include "edit_table.h"


QIodeEditTable::QIodeEditTable(const QString& tableName, QWidget* parent, Qt::WindowFlags f) 
	: QIodeSettings(parent, f), tableName(tableName)
{
	setupUi(this);

	tableView->setupModel(tableName);

	for(const auto& [line_type, _] : mLineType) 
		list_insert_types << QString::fromStdString(line_type);

	wInsertLineType = new WrapperComboBox("line_type", *comboBox_insert_line_type, REQUIRED_FIELD, list_insert_types);
	wInsertWhere = new WrapperComboBox("where", *comboBox_insert_where, REQUIRED_FIELD, list_insert_where);

	mapFields["InsertLineType"]  = wInsertLineType;
	mapFields["InsertWhere"]     = wInsertWhere;

	shortcutDelete = new QShortcut(QKeySequence(Qt::Key_Delete), this);
	shortcutDelete->setContext(Qt::WidgetWithChildrenShortcut);

	connect(shortcutDelete, &QShortcut::activated, this, &QIodeEditTable::delete_line);

	MainWindowPlot* main_window = static_cast<MainWindowPlot*>(get_main_window_ptr());
	connect(this, &QIodeEditTable::newPlot, main_window, &MainWindowPlot::appendPlot);

	// propagate signal
	QIodeEditTableModel* table_model = static_cast<QIodeEditTableModel*>(tableView->model());
	connect(table_model, &QIodeEditTableModel::tableModified, this, &QIodeEditTable::tableModified);

	loadSettings();

	table_model->computeHash(true);
}

QIodeEditTable::~QIodeEditTable()
{
	delete wInsertLineType;
	delete wInsertWhere;

	delete shortcutDelete;
}

void QIodeEditTable::edit()
{
	try
	{
		QIodeEditTableModel* table_model = static_cast<QIodeEditTableModel*>(tableView->model());
		table_model->save();
		table_model->computeHash();

		this->accept();
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), "WARNING", QString(e.what()));
	}
}

void QIodeEditTable::plot()
{
	try
	{
        KDBVariables kdb_var;
        Sample smpl = kdb_var.get_sample();
        QString gsample = QString::fromStdString(smpl.start_period().to_string()) + ":" + QString::number(smpl.nb_periods());
 
        GSampleTable* gSampleTable = new GSampleTable(tableName.toStdString(), gsample.toStdString());

        QIodePlotTableDialog* plotDialog = new QIodePlotTableDialog(gSampleTable);
        plotDialog->plot();
        emit newPlot(plotDialog);
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), "WARNING", QString(e.what()));
	}
}

void QIodeEditTable::insert_line()
{
	try
	{
		QIodeEditTableModel* tables_model = static_cast<QIodeEditTableModel*>(tableView->model());
		QModelIndexList selection = tableView->selectionModel()->selectedRows();
		int position = (selection.count() > 0) ? selection[0].row() : -1;

		int insertLineType = wInsertLineType->extractAndVerify();
		std::string s_lineType = list_insert_types[insertLineType].toStdString();
		EnumLineType lineType = mLineType.at(s_lineType);

		int insertWhere = wInsertWhere->extractAndVerify();

		int new_pos;
		switch(insertWhere)
		{
		case INSERT_AT_THE_END:
			new_pos = tables_model->appendLine(lineType);
			break;
		case INSERT_AFTER_CURRENT:
			if(position < 0)
				return;
			new_pos = tables_model->insertLine(lineType, position, true);
			break;
		case INSERT_BEFORE_CURRENT:
			if(position < 0)
				return;
			new_pos = tables_model->insertLine(lineType, position, false);
			break;
		default:
			break;
		}
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), "WARNING", QString(e.what()));
	}
}

void QIodeEditTable::delete_line()
{
	try
	{
		QIodeEditTableModel* tables_model = static_cast<QIodeEditTableModel*>(tableView->model());
		QModelIndexList selection = tableView->selectionModel()->selectedRows();
		if(selection.count() == 0)
			return;

    	int row = selection[0].row();
		// if diviser line selected -> do nothing
		if(row == 0)
			return;
		
    	tables_model->removeRow(row, QModelIndex());
    	update();
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), "WARNING", QString(e.what()));
	}
}

void QIodeEditTable::help()
{
	QDesktopServices::openUrl(url_manual);
}
