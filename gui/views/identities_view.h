#pragma once

#include <QObject>
#include <QTableView>

#include "abstract_qtableview.h"
#include "../models/identities_model.h"


class IdentitiesView : public AbstractTableView<IdentitiesModel>
{
	Q_OBJECT

public:
	IdentitiesView(QWidget* parent = nullptr) : AbstractTableView(parent) {};
};
