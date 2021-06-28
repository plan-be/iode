#pragma once

#include <QObject>
#include <QTableView>

#include "../models/comments_model.h"


class CommentsView : public QTableView
{
	Q_OBJECT

public:
	CommentsView(QWidget* parent = nullptr);
	~CommentsView();

	void setupModel(CommentsModel* model);
	void update();
};
