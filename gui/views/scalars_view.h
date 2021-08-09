#pragma once

#include <QObject>
#include <QTableView>

#include "abstract_qtableview.h"
#include "../models/scalars_model.h"


class ScalarsView : public AbstractTableView<ScalarsModel>
{
	Q_OBJECT

public:
	ScalarsView(QWidget* parent = nullptr) : AbstractTableView(parent) {};
};
