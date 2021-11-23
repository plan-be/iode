#pragma once

#include <QObject>

#include "abstract_table_view.h"
#include "comments_model.h"
#include "text_delegate.h"


class CommentsView : public AbstractTableView<CommentsModel>
{
	Q_OBJECT

public:
	CommentsView(QWidget* parent = nullptr) : AbstractTableView(COMMENTS, new TextDelegate(parent), parent) {};
};
