#include "edit_table.h"


QIodeEditTable::QIodeEditTable(const QString& equationName, QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f) 
{
}

QIodeEditTable::~QIodeEditTable()
{
	for (auto& wrapper : map_items.values()) delete wrapper;
}

void QIodeEditTable::edit()
{
	try
	{
		this->accept();
	}
	catch (const std::runtime_error& e)
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), tr("Warning"), tr(e.what()));
	}
}

void QIodeEditTable::help()
{
	QUrl url = get_url_iode_manual();
	QDesktopServices::openUrl(url);
}