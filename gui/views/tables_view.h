#pragma once

#include <QObject>
#include <QTableView>

#include "abstract_qtableview.h"
#include "../models/tables_model.h"


class TablesView : public AbstractTableView<TablesModel>
{
	Q_OBJECT

public:
	TablesView(QWidget* parent = nullptr) : AbstractTableView(parent) {};
};
