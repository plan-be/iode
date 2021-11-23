#pragma once

#include <QObject>

#include "abstract_table_view.h"
#include "equations_model.h"
#include "text_delegate.h"


class EquationsView : public AbstractTableView<EquationsModel>
{
	Q_OBJECT

public:
	EquationsView(QWidget* parent = nullptr) : AbstractTableView(EQUATIONS, new TextDelegate(parent), parent) {};
};
