#pragma once

#include <QObject>

#include "abstract_table_view.h"
#include "comments_model.h"


class CommentsView : public AbstractTableView<CommentsModel>
{
	Q_OBJECT

public:
	CommentsView(QWidget* parent = nullptr) : AbstractTableView(parent) {};
};
