#pragma once

#include <QObject>
#include <QTableView>

#include "abstract_qtableview.h"
#include "../models/equations_model.h"


class EquationsView : public AbstractTableView<EquationsModel>
{
	Q_OBJECT

public:
	EquationsView(QWidget* parent = nullptr) : AbstractTableView(parent) {};
};