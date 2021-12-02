#pragma once

#include <QObject>

#include "abstract_table_view.h"
#include "tables_model.h"
#include "tables_delegate.h"


class TablesView : public AbstractTableView<TablesModel>
{
	Q_OBJECT

public:
	TablesView(QWidget* parent = nullptr) : AbstractTableView(TABLES, new TablesDelegate(parent), parent) {};
};
