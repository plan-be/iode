#pragma once

#include <QObject>

#include "abstract_table_view.h"
#include "iode_objs/models/comments_model.h"
#include "iode_objs/delegates/comments_delegate.h"
#include "iode_objs/new/add_comment.h"
#include "iode_objs/edit/edit_iode_obj.h"


class CommentsView : public IodeTemplateTableView<CommentsModel, EditCommentDialog>
{
	Q_OBJECT

public:
	CommentsView(QWidget* parent = nullptr) : IodeTemplateTableView(I_COMMENTS, new CommentsDelegate(parent), parent) {};

public slots:
	void new_obj();
	void edit_obj() { openEditDialog(); }
};
