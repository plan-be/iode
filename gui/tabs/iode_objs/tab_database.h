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


class CommentsWidget : public TemplateIodeObjectWidget<CommentsModel, CommentsView>
{
public:
    CommentsWidget(IodeAbstractTabWidget* parent) : TemplateIodeObjectWidget(I_COMMENTS, parent) 
    {
        connect(objmodel, &CommentsModel::dataChanged, this, &CommentsWidget::databaseModified);
        connect(objmodel, &CommentsModel::headerDataChanged, this, &CommentsWidget::databaseModified);
        connect(objmodel, &CommentsModel::rowsInserted, this, &CommentsWidget::databaseModified);
        connect(objmodel, &CommentsModel::rowsRemoved, this, &CommentsWidget::databaseModified);
        connect(objmodel, &CommentsModel::databaseModified, this, &CommentsWidget::databaseModified);
        connect(tableview, &CommentsView::newObjectInserted, this, &CommentsWidget::databaseModified);
    }
};

class EquationsWidget : public TemplateIodeObjectWidget<EquationsModel, EquationsView>
{
public:
    EquationsWidget(IodeAbstractTabWidget* parent) : TemplateIodeObjectWidget(I_EQUATIONS, parent) 
    {
        connect(objmodel, &EquationsModel::dataChanged, this, &EquationsWidget::databaseModified);
        connect(objmodel, &EquationsModel::headerDataChanged, this, &EquationsWidget::databaseModified);
        connect(objmodel, &EquationsModel::rowsInserted, this, &EquationsWidget::databaseModified);
        connect(objmodel, &EquationsModel::rowsRemoved, this, &EquationsWidget::databaseModified);
        connect(objmodel, &EquationsModel::databaseModified, this, &EquationsWidget::databaseModified);
        connect(tableview, &EquationsView::newObjectInserted, this, &EquationsWidget::databaseModified);
        connect(tableview, &EquationsView::databaseModified, this, &EquationsWidget::databaseModified);
    }
};

class IdentitiesWidget : public TemplateIodeObjectWidget<IdentitiesModel, IdentitiesView>
{
public:
    IdentitiesWidget(IodeAbstractTabWidget* parent) : TemplateIodeObjectWidget(I_IDENTITIES, parent) 
    {
        connect(objmodel, &IdentitiesModel::dataChanged, this, &IdentitiesWidget::databaseModified);
        connect(objmodel, &IdentitiesModel::headerDataChanged, this, &IdentitiesWidget::databaseModified);
        connect(objmodel, &IdentitiesModel::rowsInserted, this, &IdentitiesWidget::databaseModified);
        connect(objmodel, &IdentitiesModel::rowsRemoved, this, &IdentitiesWidget::databaseModified);
        connect(objmodel, &IdentitiesModel::databaseModified, this, &IdentitiesWidget::databaseModified);
        connect(tableview, &IdentitiesView::newObjectInserted, this, &IdentitiesWidget::databaseModified);
    }
};

class ListsWidget : public TemplateIodeObjectWidget<ListsModel, ListsView>
{
public:
    ListsWidget(IodeAbstractTabWidget* parent) : TemplateIodeObjectWidget(I_LISTS, parent) 
    {
        connect(objmodel, &ListsModel::dataChanged, this, &ListsWidget::databaseModified);
        connect(objmodel, &ListsModel::headerDataChanged, this, &ListsWidget::databaseModified);
        connect(objmodel, &ListsModel::rowsInserted, this, &ListsWidget::databaseModified);
        connect(objmodel, &ListsModel::rowsRemoved, this, &ListsWidget::databaseModified);
        connect(objmodel, &ListsModel::databaseModified, this, &ListsWidget::databaseModified);
        connect(tableview, &ListsView::newObjectInserted, this, &ListsWidget::databaseModified);
    }
};

class ScalarsWidget : public TemplateNumericalWidget<ScalarsModel, ScalarsView>
{
public:
    ScalarsWidget(IodeAbstractTabWidget* parent) : TemplateNumericalWidget(I_SCALARS, parent) 
    {
        addHorizontalSpacer();

        connect(objmodel, &ScalarsModel::dataChanged, this, &ScalarsWidget::databaseModified);
        connect(objmodel, &ScalarsModel::headerDataChanged, this, &ScalarsWidget::databaseModified);
        connect(objmodel, &ScalarsModel::rowsInserted, this, &ScalarsWidget::databaseModified);
        connect(objmodel, &ScalarsModel::rowsRemoved, this, &ScalarsWidget::databaseModified);
        connect(objmodel, &ScalarsModel::databaseModified, this, &ScalarsWidget::databaseModified);
        connect(tableview, &ScalarsView::newObjectInserted, this, &ScalarsWidget::databaseModified);
    }
};

class TablesWidget : public TemplateIodeObjectWidget<TablesModel, TablesView>
{
public:
    TablesWidget(IodeAbstractTabWidget* parent) : TemplateIodeObjectWidget(I_TABLES, parent) 
    {
        connect(objmodel, &TablesModel::dataChanged, this, &TablesWidget::databaseModified);
        connect(objmodel, &TablesModel::headerDataChanged, this, &TablesWidget::databaseModified);
        connect(objmodel, &TablesModel::rowsInserted, this, &TablesWidget::databaseModified);
        connect(objmodel, &TablesModel::rowsRemoved, this, &TablesWidget::databaseModified);
        connect(objmodel, &TablesModel::databaseModified, this, &TablesWidget::databaseModified);
        connect(tableview, &TablesView::newObjectInserted, this, &TablesWidget::databaseModified);
        connect(tableview, &TablesView::tableModified, this, &TablesWidget::databaseModified);
    }
};

class VariablesWidget : public TemplateNumericalWidget<VariablesModel, VariablesView>
{
    Q_OBJECT

    QComboBox* comboMode;
    QShortcut* shortcutModePlus;
    QShortcut* shortcutModeMinus;

public:
    VariablesWidget(IodeAbstractTabWidget* parent) : TemplateNumericalWidget(I_VARIABLES, parent) 
    {
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);

        QLabel* label_mode = new QLabel("mode ");
        label_mode->setObjectName(QString::fromUtf8("label_mode"));
        label_mode->setSizePolicy(sizePolicy);
        bottomLayout->addWidget(label_mode, Qt::AlignLeft);

        QStringList q_var_modes;
        for(const std::string& s_mode: v_var_modes) q_var_modes << QString::fromStdString(s_mode);

        comboMode = new QComboBox();
        comboMode->setObjectName("combobox_mode");
        comboMode->addItems(q_var_modes);
        comboMode->setSizePolicy(sizePolicy);
        bottomLayout->addWidget(comboMode, Qt::AlignLeft);

        addHorizontalSpacer();

        // shortcuts
        shortcutModePlus = new QShortcut(QKeySequence(Qt::Key_F5), this);
        shortcutModeMinus = new QShortcut(QKeySequence(Qt::SHIFT | Qt::Key_F5), this);

        shortcutModePlus->setContext(Qt::WidgetWithChildrenShortcut);
        shortcutModeMinus->setContext(Qt::WidgetWithChildrenShortcut);

        // connect
        connect(comboMode, &QComboBox::currentIndexChanged, this, &VariablesWidget::updateMode);
        connect(shortcutModePlus, &QShortcut::activated, this, &VariablesWidget::nextMode);
        connect(shortcutModeMinus, &QShortcut::activated, this, &VariablesWidget::previousMode);

        connect(objmodel, &VariablesModel::dataChanged, this, &VariablesWidget::databaseModified);
        connect(objmodel, &VariablesModel::headerDataChanged, this, &VariablesWidget::databaseModified);
        connect(objmodel, &VariablesModel::rowsInserted, this, &VariablesWidget::databaseModified);
        connect(objmodel, &VariablesModel::rowsRemoved, this, &VariablesWidget::databaseModified);
        connect(objmodel, &VariablesModel::databaseModified, this, &VariablesWidget::databaseModified);
        connect(tableview, &VariablesView::newObjectInserted, this, &VariablesWidget::databaseModified);
    
        // reload mode
        loadModeFromSettings();
    }

    ~VariablesWidget()
    {
        delete shortcutModePlus;
        delete shortcutModeMinus;
    }

    void setProjectDir(const QDir& projectDir) override
    {
        TemplateNumericalWidget::setProjectDir(projectDir);
        loadModeFromSettings();
    }

    void loadModeFromSettings()
    {
        QSettings* project_settings = ProjectSettings::getProjectSettings();
        if(!project_settings)
        {
            comboMode->setCurrentIndex(0);
            return;
        }
        
        project_settings->beginGroup(getGroupName());
        int mode = project_settings->value("Mode", 0).toInt();
        comboMode->setCurrentIndex(mode);
        project_settings->endGroup();
    }

    void saveModeToSettings()
    {
        QSettings* project_settings = ProjectSettings::getProjectSettings();
        if(!project_settings)
            return;
        
        project_settings->beginGroup(getGroupName());
        project_settings->setValue("Mode", comboMode->currentIndex());
        project_settings->endGroup();
    }

public slots:
    void updateMode(const int index)
    {
        objmodel->setMode((EnumIodeVarMode) index);
        saveModeToSettings();
    }

    void nextMode()
    {
        int index = comboMode->currentIndex();
        index = (index == comboMode->count() - 1) ? 0 : index + 1;
        comboMode->setCurrentIndex(index);
    }

    void previousMode()
    {
        int index = comboMode->currentIndex();
        index = (index == 0) ? comboMode->count() - 1 : index - 1;
        comboMode->setCurrentIndex(index);
    }

};
