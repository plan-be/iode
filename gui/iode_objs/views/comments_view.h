#pragma once

#include <QObject>

#include "abstract_table_view.h"
#include "iode_objs/models/comments_model.h"
#include "iode_objs/delegates/comments_delegate.h"
#include "iode_objs/new/add_comment.h"


class CommentsView : public AbstractTableView<CommentsModel>
{
	Q_OBJECT

public:
	CommentsView(QWidget* parent = nullptr) : AbstractTableView(I_COMMENTS, new CommentsDelegate(parent), parent) {};

public slots:
	void filter() { filter_and_update(); }
	void new_comment();
};
