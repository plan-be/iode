#pragma once

#include <QTableView>
#include "../utils.h"
#include "object_name_delegate.h"

template <class M> class AbstractTableView : public QTableView
{
	EnumIodeType iodeType;
	ObjectNameDelegate* objectNameDelegate;

public:
	AbstractTableView(EnumIodeType iodeType, QWidget* parent = nullptr) : QTableView(parent), iodeType(iodeType)
	{
		bool uppercase = !(iodeType == SCALARS);
		objectNameDelegate = new ObjectNameDelegate(uppercase, parent);
	};

	~AbstractTableView()
	{
		delete objectNameDelegate;
	}

	void setupModel(M* model)
	{
		setModel(model);
		setItemDelegateForColumn(0, objectNameDelegate);
	}

	void update()
	{
		M* table_model = static_cast<M*>(model());
		table_model->reset();
		table_model->rowCount() > 0 ? show() : hide();
	}
};
