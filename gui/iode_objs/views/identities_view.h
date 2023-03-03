#pragma once

#include <QObject>

#include "abstract_table_view.h"
#include "iode_objs/models/identities_model.h"
#include "iode_objs/delegates/identities_delegate.h"
#include "iode_objs/new/add_identity.h"


class IdentitiesView : public TemplateTableView<IdentitiesModel>
{
	Q_OBJECT

public:
	IdentitiesView(QWidget* parent = nullptr) : TemplateTableView(I_IDENTITIES, new IdentitiesDelegate(parent), parent) {};

public slots:
	void filter() { filter_and_update(); }
	void new_obj();
};
