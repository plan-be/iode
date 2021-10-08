#pragma once

#include <QMainWindow>

#include "ui_main_window.h"

#include "utils.h"

#include "tabs/comments_model.h"
#include "tabs/equations_model.h"
#include "tabs/identities_model.h"
#include "tabs/lists_model.h"
#include "tabs/scalars_model.h"
#include "tabs/tables_model.h"
#include "tabs/variables_model.h"


class MainWindow : public QMainWindow, public Ui::MainWindow
{
	Q_OBJECT

private:
	CommentsModel* commentsModel;
	EquationsModel* equationsModel;
	IdentitiesModel* identitiesModel;
	ListsModel* listsModel;
	ScalarsModel* scalarsModel;
	TablesModel* tablesModel;
	VariablesModel* variablesModel;

public:
	MainWindow(QWidget *parent = Q_NULLPTR);
	~MainWindow();

private:
	void updateCurrentTab(int index = -1);
};
