#pragma once

#include <QObject>

#include "abstract_table_view.h"
#include "iode_objs/models/comments_model.h"
#include "iode_objs/delegates/comments_delegate.h"
#include "iode_objs/new/add_comment.h"


class CommentsView : public IodeAbstractTableView
{
	Q_OBJECT

public:
	CommentsView(QWidget* parent = nullptr) : IodeAbstractTableView(I_COMMENTS, new CommentsDelegate(parent), parent) {};

public slots:
	void new_obj();
};
