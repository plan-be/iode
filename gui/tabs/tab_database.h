#pragma once

#include "tab_database_abstract.h"

#ifndef TEMPLATE_NUMERICAL_WIDGET_HEADER
#include "tab_numerical_values.h"
#endif

#include "iode_objs/models/comments_model.h"
#include "iode_objs/models/equations_model.h"
#include "iode_objs/models/identities_model.h"
#include "iode_objs/models/lists_model.h"
#include "iode_objs/models/scalars_model.h"
#include "iode_objs/models/tables_model.h"
#include "iode_objs/models/variables_model.h"

#include "iode_objs/views/comments_view.h"
#include "iode_objs/views/equations_view.h"
#include "iode_objs/views/identities_view.h"
#include "iode_objs/views/lists_view.h"
#include "iode_objs/views/scalars_view.h"
#include "iode_objs/views/tables_view.h"
#include "iode_objs/views/variables_view.h"


class QIodeCommentsWidget : public QIodeObjectWidget<CommentsModel, CommentsView>
{
public:
    QIodeCommentsWidget(QIodeAbstractTabWidget* parent) : QIodeObjectWidget(I_COMMENTS, parent) 
    {
        connect(objmodel, &CommentsModel::dataChanged, this, &QIodeCommentsWidget::databaseModified);
        connect(objmodel, &CommentsModel::headerDataChanged, this, &QIodeCommentsWidget::databaseModified);
        connect(objmodel, &CommentsModel::rowsInserted, this, &QIodeCommentsWidget::databaseModified);
        connect(objmodel, &CommentsModel::rowsRemoved, this, &QIodeCommentsWidget::databaseModified);
        connect(objmodel, &CommentsModel::databaseModified, this, &QIodeCommentsWidget::databaseModified);
        connect(tableview, &CommentsView::newObjectInserted, this, &QIodeCommentsWidget::databaseModified);
    }
};

class QIodeEquationsWidget : public QIodeObjectWidget<EquationsModel, EquationsView>
{
public:
    QIodeEquationsWidget(QIodeAbstractTabWidget* parent) : QIodeObjectWidget(I_EQUATIONS, parent) 
    {
        connect(objmodel, &EquationsModel::dataChanged, this, &QIodeEquationsWidget::databaseModified);
        connect(objmodel, &EquationsModel::headerDataChanged, this, &QIodeEquationsWidget::databaseModified);
        connect(objmodel, &EquationsModel::rowsInserted, this, &QIodeEquationsWidget::databaseModified);
        connect(objmodel, &EquationsModel::rowsRemoved, this, &QIodeEquationsWidget::databaseModified);
        connect(objmodel, &EquationsModel::databaseModified, this, &QIodeEquationsWidget::databaseModified);
        connect(tableview, &EquationsView::newObjectInserted, this, &QIodeEquationsWidget::databaseModified);
        connect(tableview, &EquationsView::equationModified, this, &QIodeEquationsWidget::databaseModified);
    }
};

class QIodeIdentitiesWidget : public QIodeObjectWidget<IdentitiesModel, IdentitiesView>
{
public:
    QIodeIdentitiesWidget(QIodeAbstractTabWidget* parent) : QIodeObjectWidget(I_IDENTITIES, parent) 
    {
        connect(objmodel, &IdentitiesModel::dataChanged, this, &QIodeIdentitiesWidget::databaseModified);
        connect(objmodel, &IdentitiesModel::headerDataChanged, this, &QIodeIdentitiesWidget::databaseModified);
        connect(objmodel, &IdentitiesModel::rowsInserted, this, &QIodeIdentitiesWidget::databaseModified);
        connect(objmodel, &IdentitiesModel::rowsRemoved, this, &QIodeIdentitiesWidget::databaseModified);
        connect(objmodel, &IdentitiesModel::databaseModified, this, &QIodeIdentitiesWidget::databaseModified);
        connect(tableview, &IdentitiesView::newObjectInserted, this, &QIodeIdentitiesWidget::databaseModified);
    }
};

class QIodeListsWidget : public QIodeObjectWidget<ListsModel, ListsView>
{
public:
    QIodeListsWidget(QIodeAbstractTabWidget* parent) : QIodeObjectWidget(I_LISTS, parent) 
    {
        connect(objmodel, &ListsModel::dataChanged, this, &QIodeListsWidget::databaseModified);
        connect(objmodel, &ListsModel::headerDataChanged, this, &QIodeListsWidget::databaseModified);
        connect(objmodel, &ListsModel::rowsInserted, this, &QIodeListsWidget::databaseModified);
        connect(objmodel, &ListsModel::rowsRemoved, this, &QIodeListsWidget::databaseModified);
        connect(objmodel, &ListsModel::databaseModified, this, &QIodeListsWidget::databaseModified);
        connect(tableview, &ListsView::newObjectInserted, this, &QIodeListsWidget::databaseModified);
    }
};

class QIodeScalarsWidget : public TemplateNumericalWidget<ScalarsModel, ScalarsView>
{
public:
    QIodeScalarsWidget(QIodeAbstractTabWidget* parent) : TemplateNumericalWidget(I_SCALARS, parent) 
    {
        connect(objmodel, &ScalarsModel::dataChanged, this, &QIodeScalarsWidget::databaseModified);
        connect(objmodel, &ScalarsModel::headerDataChanged, this, &QIodeScalarsWidget::databaseModified);
        connect(objmodel, &ScalarsModel::rowsInserted, this, &QIodeScalarsWidget::databaseModified);
        connect(objmodel, &ScalarsModel::rowsRemoved, this, &QIodeScalarsWidget::databaseModified);
        connect(objmodel, &ScalarsModel::databaseModified, this, &QIodeScalarsWidget::databaseModified);
        connect(tableview, &ScalarsView::newObjectInserted, this, &QIodeScalarsWidget::databaseModified);
    }
};

class QIodeTablesWidget : public QIodeObjectWidget<TablesModel, TablesView>
{
public:
    QIodeTablesWidget(QIodeAbstractTabWidget* parent) : QIodeObjectWidget(I_TABLES, parent) 
    {
        connect(objmodel, &TablesModel::dataChanged, this, &QIodeTablesWidget::databaseModified);
        connect(objmodel, &TablesModel::headerDataChanged, this, &QIodeTablesWidget::databaseModified);
        connect(objmodel, &TablesModel::rowsInserted, this, &QIodeTablesWidget::databaseModified);
        connect(objmodel, &TablesModel::rowsRemoved, this, &QIodeTablesWidget::databaseModified);
        connect(objmodel, &TablesModel::databaseModified, this, &QIodeTablesWidget::databaseModified);
        connect(tableview, &TablesView::newObjectInserted, this, &QIodeTablesWidget::databaseModified);
        connect(tableview, &TablesView::tableModified, this, &QIodeTablesWidget::databaseModified);
    }
};

class QIodeVariablesWidget : public TemplateNumericalWidget<VariablesModel, VariablesView>
{
public:
    QIodeVariablesWidget(QIodeAbstractTabWidget* parent) : TemplateNumericalWidget(I_VARIABLES, parent) 
    {
        connect(objmodel, &VariablesModel::dataChanged, this, &QIodeVariablesWidget::databaseModified);
        connect(objmodel, &VariablesModel::headerDataChanged, this, &QIodeVariablesWidget::databaseModified);
        connect(objmodel, &VariablesModel::rowsInserted, this, &QIodeVariablesWidget::databaseModified);
        connect(objmodel, &VariablesModel::rowsRemoved, this, &QIodeVariablesWidget::databaseModified);
        connect(objmodel, &VariablesModel::databaseModified, this, &QIodeVariablesWidget::databaseModified);
        connect(tableview, &VariablesView::newObjectInserted, this, &QIodeVariablesWidget::databaseModified);
    }
};
