#pragma once

#include <QObject>

#include "abstract_table_view.h"
#include "identities_model.h"
#include "identities_delegate.h"
#include "popup/add_identity.h"


class IdentitiesView : public AbstractTableView<IdentitiesModel>
{
	Q_OBJECT

public:
	IdentitiesView(QWidget* parent = nullptr) : AbstractTableView(I_IDENTITIES, new IdentitiesDelegate(parent), parent) {};

public slots:
	void new_identity();
};
