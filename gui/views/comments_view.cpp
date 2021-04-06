#include "comments_view.h"

CommentsView::CommentsView(QWidget* parent) : QTableView(parent)
{
}

CommentsView::~CommentsView()
{
}

void CommentsView::setupModel(CommentsModel* model)
{
	setModel(model);
}
