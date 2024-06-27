#pragma once

#include <QObject>

#include "abstract_table_view.h"
#include "iode_objs/models/comments_model.h"
#include "iode_objs/delegates/comments_delegate.h"
#include "iode_objs/new/add_comment.h"
#include "iode_objs/edit/edit_comment.h"


class CommentsView : public IodeAbstractTableView, public TableViewEditObj<CommentsModel, EditCommentDialog>, public TableViewAddObj<CommentsModel, AddCommentDialog>
{
	Q_OBJECT

public:
	CommentsView(QWidget* parent = nullptr) : 
		IodeAbstractTableView(COMMENTS, new CommentsDelegate(parent), parent), 
		TableViewEditObj(this), TableViewAddObj(this) {};

public slots:
	void new_obj()  { openAddDialog(); }
	void edit_obj() { openEditDialog(); }
};
