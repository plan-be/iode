#pragma once

#include <QTableView>
#include "../utils.h"
#include "abstract_delegate.h"

template <class M> class AbstractTableView : public QTableView
{
	EnumIodeType iodeType;
	AbstractDelegate* delegate;

public:
	AbstractTableView(EnumIodeType iodeType, AbstractDelegate* delegate, QWidget* parent = nullptr) : QTableView(parent), iodeType(iodeType), delegate(delegate) {}

	~AbstractTableView() {}

	void setupModel(M* model)
	{
		setModel(model);
		setItemDelegate(delegate);
	}

	void update()
	{
		M* table_model = static_cast<M*>(model());
		table_model->reset();
		table_model->rowCount() > 0 ? show() : hide();
	}
};
