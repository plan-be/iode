#include "edit_table.h"


QIodeEditTable::QIodeEditTable(const QString& tableName, QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	setupUi(this);

	tableView->setupModel(tableName);
}

void QIodeEditTable::edit()
{
	try
	{
		static_cast<QIodeEditTableModel*>(tableView->model())->save();
		this->accept();
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), tr("Warning"), tr(e.what()));
	}
}

void QIodeEditTable::help()
{
	
	QDesktopServices::openUrl(url_manual);
}
