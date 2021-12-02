#pragma once

#include <QObject>

#include "abstract_table_view.h"
#include "comments_model.h"
#include "comments_delegate.h"


class CommentsView : public AbstractTableView<CommentsModel>
{
	Q_OBJECT

public:
	CommentsView(QWidget* parent = nullptr) : AbstractTableView(COMMENTS, new CommentsDelegate(parent), parent) {};
};
