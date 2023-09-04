#include "edit_table.h"


EditTableDialog::EditTableDialog(const QString& tableName, QWidget* parent) 
	: IodeSettingsDialog(parent, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint), tableName(tableName)
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
    fullScreenShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_X), this);

	shortcutDelete->setContext(Qt::WidgetWithChildrenShortcut);
    fullScreenShortcut->setContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);

	connect(shortcutDelete, &QShortcut::activated, this, &EditTableDialog::delete_line);
    connect(fullScreenShortcut, &QShortcut::activated, this, &EditTableDialog::showMaximized);

	MainWindowAbstract* main_window = static_cast<MainWindowAbstract*>(get_main_window_ptr());
	connect(this, &EditTableDialog::newPlot, main_window, &MainWindowAbstract::appendDialog);

	// propagate signal
	EditTableModel* table_model = static_cast<EditTableModel*>(tableView->model());
	connect(table_model, &EditTableModel::tableModified, this, &EditTableDialog::tableModified);

	loadSettings();

	table_model->computeHash(true);
}

EditTableDialog::~EditTableDialog()
{
	delete wInsertLineType;
	delete wInsertWhere;

	delete shortcutDelete;
	delete fullScreenShortcut;
}

void EditTableDialog::edit()
{
	try
	{
		EditTableModel* table_model = static_cast<EditTableModel*>(tableView->model());
		table_model->save();
		table_model->computeHash();

		this->accept();
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), "WARNING", QString(e.what()));
	}
}

void EditTableDialog::plot()
{
	try
	{
        KDBVariables kdb_var;
        Sample smpl = kdb_var.get_sample();
        QString gsample = QString::fromStdString(smpl.start_period().to_string()) + ":" + QString::number(smpl.nb_periods());
 
        GSampleGraph* gSampleGraph = new GSampleGraph(tableName.toStdString(), gsample.toStdString());

        PlotTableDialog* plotDialog = new PlotTableDialog(gSampleGraph);
        plotDialog->plot();
        emit newPlot(plotDialog);
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), "WARNING", QString(e.what()));
	}
}

void EditTableDialog::insert_line()
{
	try
	{
		EditTableModel* tables_model = static_cast<EditTableModel*>(tableView->model());
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

void EditTableDialog::delete_line()
{
	try
	{
		EditTableModel* tables_model = static_cast<EditTableModel*>(tableView->model());
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

void EditTableDialog::help()
{
	QDesktopServices::openUrl(url_manual);
}
