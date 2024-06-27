#pragma once

#include "tab_database_abstract.h"
#include "tab_numerical_values.h"

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
    CommentsWidget(QWidget* parent) : TemplateIodeObjectWidget(COMMENTS, parent) 
    {
        connect(objmodel, &CommentsModel::dataChanged, this, &CommentsWidget::databaseModified);
        connect(objmodel, &CommentsModel::headerDataChanged, this, &CommentsWidget::databaseModified);
        connect(objmodel, &CommentsModel::rowsInserted, this, &CommentsWidget::databaseModified);
        connect(objmodel, &CommentsModel::rowsRemoved, this, &CommentsWidget::databaseModified);
        connect(objmodel, &CommentsModel::databaseModified, this, &CommentsWidget::databaseModified);
        connect(tableview, &CommentsView::databaseModified, this, &CommentsWidget::databaseModified);
    }
};

class EquationsWidget : public TemplateIodeObjectWidget<EquationsModel, EquationsView>
{
public:
    EquationsWidget(QWidget* parent) : TemplateIodeObjectWidget(EQUATIONS, parent) 
    {
        connect(objmodel, &EquationsModel::dataChanged, this, &EquationsWidget::databaseModified);
        connect(objmodel, &EquationsModel::headerDataChanged, this, &EquationsWidget::databaseModified);
        connect(objmodel, &EquationsModel::rowsInserted, this, &EquationsWidget::databaseModified);
        connect(objmodel, &EquationsModel::rowsRemoved, this, &EquationsWidget::databaseModified);
        connect(objmodel, &EquationsModel::databaseModified, this, &EquationsWidget::databaseModified);
        connect(tableview, &EquationsView::databaseModified, this, &EquationsWidget::databaseModified);
    }
};

class IdentitiesWidget : public TemplateIodeObjectWidget<IdentitiesModel, IdentitiesView>
{
public:
    IdentitiesWidget(QWidget* parent) : TemplateIodeObjectWidget(IDENTITIES, parent) 
    {
        connect(objmodel, &IdentitiesModel::dataChanged, this, &IdentitiesWidget::databaseModified);
        connect(objmodel, &IdentitiesModel::headerDataChanged, this, &IdentitiesWidget::databaseModified);
        connect(objmodel, &IdentitiesModel::rowsInserted, this, &IdentitiesWidget::databaseModified);
        connect(objmodel, &IdentitiesModel::rowsRemoved, this, &IdentitiesWidget::databaseModified);
        connect(objmodel, &IdentitiesModel::databaseModified, this, &IdentitiesWidget::databaseModified);
        connect(tableview, &IdentitiesView::databaseModified, this, &IdentitiesWidget::databaseModified);
    }
};

class ListsWidget : public TemplateIodeObjectWidget<ListsModel, ListsView>
{
public:
    ListsWidget(QWidget* parent) : TemplateIodeObjectWidget(LISTS, parent) 
    {
        connect(objmodel, &ListsModel::dataChanged, this, &ListsWidget::databaseModified);
        connect(objmodel, &ListsModel::headerDataChanged, this, &ListsWidget::databaseModified);
        connect(objmodel, &ListsModel::rowsInserted, this, &ListsWidget::databaseModified);
        connect(objmodel, &ListsModel::rowsRemoved, this, &ListsWidget::databaseModified);
        connect(objmodel, &ListsModel::databaseModified, this, &ListsWidget::databaseModified);
        connect(tableview, &ListsView::databaseModified, this, &ListsWidget::databaseModified);
    }
};

class ScalarsWidget : public TemplateIodeObjectWidget<ScalarsModel, ScalarsView>
{
    NumericalWidget numeric; 

public:
    ScalarsWidget(QWidget* parent) : TemplateIodeObjectWidget(SCALARS, parent) 
    {
        QHBoxLayout* bottomLayout = numeric.setup(this, objmodel, tableview);

        QSpacerItem* horizontalSpacer = new QSpacerItem(800, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        // -1 -> span over all rows/columns
        bottomLayout->addSpacerItem(horizontalSpacer);

        vLayout->addLayout(bottomLayout);

        connect(objmodel, &ScalarsModel::dataChanged, this, &ScalarsWidget::databaseModified);
        connect(objmodel, &ScalarsModel::headerDataChanged, this, &ScalarsWidget::databaseModified);
        connect(objmodel, &ScalarsModel::rowsInserted, this, &ScalarsWidget::databaseModified);
        connect(objmodel, &ScalarsModel::rowsRemoved, this, &ScalarsWidget::databaseModified);
        connect(objmodel, &ScalarsModel::databaseModified, this, &ScalarsWidget::databaseModified);
        connect(tableview, &ScalarsView::databaseModified, this, &ScalarsWidget::databaseModified);
    }

    void loadSettings(const QSettings* project_settings) override
    {
        numeric.loadNumericSettings(project_settings);
    }

    void saveSettings(QSettings* project_settings) override
    {
        numeric.saveNumericSettings(project_settings);
    }
};

class TablesWidget : public TemplateIodeObjectWidget<TablesModel, TablesView>
{
public:
    TablesWidget(QWidget* parent) : TemplateIodeObjectWidget(TABLES, parent) 
    {
        connect(objmodel, &TablesModel::dataChanged, this, &TablesWidget::databaseModified);
        connect(objmodel, &TablesModel::headerDataChanged, this, &TablesWidget::databaseModified);
        connect(objmodel, &TablesModel::rowsInserted, this, &TablesWidget::databaseModified);
        connect(objmodel, &TablesModel::rowsRemoved, this, &TablesWidget::databaseModified);
        connect(objmodel, &TablesModel::databaseModified, this, &TablesWidget::databaseModified);
        connect(tableview, &TablesView::databaseModified, this, &TablesWidget::databaseModified);
    }
};

class VariablesWidget : public TemplateIodeObjectWidget<VariablesModel, VariablesView>
{
    Q_OBJECT

    NumericalWidget numeric; 

    QComboBox* comboMode;
    QShortcut* shortcutModePlus;
    QShortcut* shortcutModeMinus;

public:
    VariablesWidget(QWidget* parent) : TemplateIodeObjectWidget(VARIABLES, parent) 
    {
        QHBoxLayout* bottomLayout = numeric.setup(this, objmodel, tableview);

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
        comboMode->setCurrentIndex(0);
        bottomLayout->addWidget(comboMode, Qt::AlignLeft);

        QSpacerItem* horizontalSpacer = new QSpacerItem(800, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        // -1 -> span over all rows/columns
        bottomLayout->addSpacerItem(horizontalSpacer);

        vLayout->addLayout(bottomLayout);

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
        connect(tableview, &VariablesView::databaseModified, this, &VariablesWidget::databaseModified);
    }

    ~VariablesWidget()
    {
        delete shortcutModePlus;
        delete shortcutModeMinus;
    }

    void loadSettings(const QSettings* project_settings) override
    {
        if(!project_settings)
            return;

        numeric.loadNumericSettings(project_settings);
        int mode = project_settings->value("Mode", 0).toInt();
        comboMode->setCurrentIndex(mode);
    }

    void saveSettings(QSettings* project_settings) override
    {
        if(!project_settings)
            return;
        
        numeric.saveNumericSettings(project_settings);
        project_settings->setValue("Mode", comboMode->currentIndex());
    }

public slots:
    void updateMode(const int index)
    {
        objmodel->setMode((IodeVarMode) index);
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
