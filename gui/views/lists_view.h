#pragma once

#include <QObject>
#include <QTableView>

#include "abstract_qtableview.h"
#include "../models/lists_model.h"


class ListsView : public AbstractTableView<ListsModel>
{
	Q_OBJECT

public:
	ListsView(QWidget* parent = nullptr) : AbstractTableView(parent) {};
};
