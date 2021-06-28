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

void CommentsView::update()
{
	CommentsModel* commentsModel = static_cast<CommentsModel*>(model());
    commentsModel->reset();
    get_nb_elements_WS(Comments) > 0 ? show() : hide();
}
