#pragma once
#pragma once

#include <QObject>
#include <QLineEdit>
#include <QStyledItemDelegate>

#include "cpp_api/iode_cpp_api.h"
#include "utils.h"
#include "edit_table_model.h"


class EditTableDelegate : public QStyledItemDelegate
{
	Q_OBJECT

private:
	bool is_editable(const QModelIndex& index) const;

public:
	EditTableDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

	void setEditorData(QWidget* editor, const QModelIndex& index) const override;

	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
};
