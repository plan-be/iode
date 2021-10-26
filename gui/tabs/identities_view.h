#pragma once

#include <QObject>

#include "abstract_table_view.h"
#include "identities_model.h"


class IdentitiesView : public AbstractTableView<IdentitiesModel>
{
	Q_OBJECT

public:
	IdentitiesView(QWidget* parent = nullptr) : AbstractTableView(IDENTITIES, parent) {};
};
