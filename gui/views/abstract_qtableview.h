#pragma once

#include <QTableView>

template <class M> class AbstractTableView : public QTableView
{
public:
	AbstractTableView(QWidget* parent = nullptr) : QTableView(parent) {};

	void setupModel(M* model)
	{
		setModel(model);
	}

	void update()
	{
		M* table_model = static_cast<M*>(model());
		table_model->reset();
		table_model->rowCount() > 0 ? show() : hide();
	}
};
