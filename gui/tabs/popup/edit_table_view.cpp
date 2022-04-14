#include "edit_table_view.h"


QIodeEditTableView::QIodeEditTableView(QWidget* parent): QTableView(parent) 
{
	// see: - https://doc.qt.io/qt-5/stylesheet-reference.html
	//      - https://doc.qt.io/qt-5/stylesheet-examples.html
	//      - https://doc.qt.io/qt-5/qcolor.html
	horizontalHeader()->setStyleSheet("font: bold; border: 0.5px solid");
	horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	verticalHeader()->setStyleSheet("border: none");
	setStyleSheet("QHeaderView::section { background-color: lightGray }");
}

void QIodeEditTableView::setupModel(const QString& tableName)
{
	QIodeEditTableModel* model = new QIodeEditTableModel(tableName, static_cast<QWidget*>(parent()));
	setModel(model);

	setItemDelegate(new EditTableDelegate());

	connect(model, &QIodeEditTableModel::displayData, this, &QIodeEditTableView::updateRow);
}

void QIodeEditTableView::updateRow(const int row, const EnumLineType line_type, const int nb_columns)
{
	if (line_type != IT_CELL) setSpan(row, 0, 1, nb_columns);
	if (line_type == IT_LINE) setRowHeight(row, 3);
}
