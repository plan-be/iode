#pragma once

#include <QObject>

#include "abstract_table_view.h"
#include "comments_model.h"
#include "comments_delegate.h"
#include "popup/add_comment.h"


class CommentsView : public AbstractTableView<CommentsModel>
{
	Q_OBJECT

public:
	CommentsView(QWidget* parent = nullptr) : AbstractTableView(I_COMMENTS, new CommentsDelegate(parent), parent) {};

public slots:
	void new_comment();
};
