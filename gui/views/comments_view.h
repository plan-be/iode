#pragma once

#include <QObject>
#include <QTableView>

#include "abstract_qtableview.h"
#include "../models/comments_model.h"


class CommentsView : public AbstractTableView<CommentsModel>
{
	Q_OBJECT

public:
	CommentsView(QWidget* parent = nullptr) : AbstractTableView(parent) {};
};
