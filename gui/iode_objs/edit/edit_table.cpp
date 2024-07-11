#include "edit_table.h"


EditTableDialog::EditTableDialog(const QString& name, KDBTables* database, QWidget* parent) 
	: IodeSettingsDialog(parent, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint), 
	  name(name.toStdString()), database(database), table(database->get(name.toStdString()))
{
	setupUi(this);

	tableView->setupModel(name);

	lineEdit_name->setText(name);

	for(const auto& [line_type, _] : m_line_type) 
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

	loadSettings();
}

EditTableDialog::~EditTableDialog()
{
	if(table)
		delete table;

	delete wInsertLineType;
	delete wInsertWhere;

	delete shortcutDelete;
	delete fullScreenShortcut;
}

void EditTableDialog::edit()
{
	try
	{
		std::string name_ = lineEdit_name->text().toStdString();
		EditTableModel* edit_table_model = static_cast<EditTableModel*>(tableView->model());
		Table* table_ = edit_table_model->getIodeTable();
		if((!table) || (!table_))
			throw std::runtime_error("Cannot edit Table with name '" + name_ + "' not found");

		if(name_ == name)
		{
			if(*table_ == *table)
			{
				this->accept();
				return;
			}

			database->update(name_, table_);
		}
		// check in / update the global IODE database since the variable 'database' may represents only a subset
		else if(Tables.contains(name_))
		{
			int res = QMessageBox::question(nullptr, "WARNING", QString::fromStdString(name_) + " already exists. Replace it ?");
			if(res != QMessageBox::StandardButton::Yes)
			{
				this->reject();
				return;
			}

			Tables.update(name_, table_);
		}
		else
			database->add(name_, table_);

		emit databaseModified();
		this->accept();
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
	}
}

void EditTableDialog::plot()
{
	try
	{
        Sample* smpl = Variables.get_sample();
        QString gsample = QString::fromStdString(smpl->start_period().to_string()) + ":" + QString::number(smpl->nb_periods());
 
        ComputedTableGraph* gSampleGraph = new ComputedTableGraph(name, gsample.toStdString());

        PlotTableDialog* plotDialog = new PlotTableDialog(gSampleGraph);
        plotDialog->plot();
        emit newPlot(plotDialog);
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
	}
}

void EditTableDialog::insert_line()
{
	try
	{
		EditTableModel* edit_table_model = static_cast<EditTableModel*>(tableView->model());
		QModelIndexList selection = tableView->selectionModel()->selectedRows();
		int position = (selection.count() > 0) ? selection[0].row() : -1;

		int insert_lineType = wInsertLineType->extractAndVerify();
		std::string s_lineType = list_insert_types[insert_lineType].toStdString();
		TableLineType lineType = m_line_type.at(s_lineType);

		int insertWhere = wInsertWhere->extractAndVerify();

		switch(insertWhere)
		{
		case INSERT_AT_THE_END:
			edit_table_model->appendLine(lineType);
			break;
		case INSERT_AFTER_CURRENT:
			if(position < 0)
				return;
			edit_table_model->insert_line(lineType, position, true);
			break;
		case INSERT_BEFORE_CURRENT:
			if(position < 0)
				return;
			edit_table_model->insert_line(lineType, position, false);
			break;
		default:
			break;
		}
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
	}
}

void EditTableDialog::delete_line()
{
	try
	{
		EditTableModel* edit_table_model = static_cast<EditTableModel*>(tableView->model());
		QModelIndexList selection = tableView->selectionModel()->selectedRows();
		if(selection.count() == 0)
			return;

    	int row = selection[0].row();
		// if diviser line selected -> do nothing
		if(row == 0)
			return;
		
    	edit_table_model->removeRow(row, QModelIndex());
    	update();
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
	}
}

void EditTableDialog::help()
{
	QDesktopServices::openUrl(url_manual);
}
