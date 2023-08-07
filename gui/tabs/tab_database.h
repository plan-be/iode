#pragma once

#include "tab_database_abstract.h"

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


/**
 * @brief 
 * 
 * @tparam M 
 * @tparam V 
 * 
 * @note : see https://www.modernescpp.com/index.php/surprise-included-inheritance-and-member-functions-of-class-templates
 *         for accessing members of template base class while the derived class is also a template class
 */
template <class M, class V> class QIodeNumericalObjectWidget: public QIodeObjectWidget<M, V>
{
    QLabel* label_nbDigits;
    QSpinBox* spinBox_nbDigits;

    QShortcut* shortcutNbDecPlus;
    QShortcut* shortcutNbDecMinus;

private:
    QString getGroupName()
    {
        return QString::fromStdString(vIodeTypes[this->iodeType]).toUpper();
    }

public:
    QIodeNumericalObjectWidget(EnumIodeType iodeType, QIodeAbstractTabWidget* parent) 
        : QIodeObjectWidget<M, V>(iodeType, parent)
    {
        QHBoxLayout* digits_layout = new QHBoxLayout();

        // nb decimals spinbox
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);

        label_nbDigits = new QLabel("Nb Significant Digits ");
        label_nbDigits->setObjectName(QString::fromUtf8("label_nbDigits"));
        label_nbDigits->setSizePolicy(sizePolicy);
        digits_layout->addWidget(label_nbDigits, 0, Qt::AlignLeft);
        //this->layout->addWidget(label_nbDigits, 2, 0, Qt::AlignLeft);

        spinBox_nbDigits = new QSpinBox();
        spinBox_nbDigits->setMinimum(1);
        spinBox_nbDigits->setMaximum(MAX_NB_DIGITS_TABLE);
        spinBox_nbDigits->setValue(2);
        spinBox_nbDigits->setObjectName(QString::fromUtf8("spinBox_nbDigits"));
        spinBox_nbDigits->setSizePolicy(sizePolicy);
        spinBox_nbDigits->setFixedWidth(40);
        digits_layout->addWidget(spinBox_nbDigits, 0, Qt::AlignLeft);
        //this->layout->addWidget(spinBox_nbDigits, 2, 1, Qt::AlignLeft);

        this->layout->addLayout(digits_layout, 2, 0);

        // spacers
        QSpacerItem* horizontalSpacer = new QSpacerItem(800, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        // -1 -> span over all rows/columns
        //this->layout->addItem(horizontalSpacer, 2, 2, 1, -1);
        this->layout->addItem(horizontalSpacer, 2, 1, 1, -1);

        // shortcuts
        shortcutNbDecPlus = new QShortcut(QKeySequence(Qt::Key_F4), this);
        shortcutNbDecMinus = new QShortcut(QKeySequence(Qt::SHIFT | Qt::Key_F4), this);

        shortcutNbDecPlus->setContext(Qt::WidgetWithChildrenShortcut);
        shortcutNbDecMinus->setContext(Qt::WidgetWithChildrenShortcut);

        // connect
        QObject::connect(spinBox_nbDigits, &QSpinBox::valueChanged, this, &QIodeNumericalObjectWidget::setNbDigits);
        QObject::connect(shortcutNbDecPlus, &QShortcut::activated, this, [this](){ this->spinBox_nbDigits->stepUp(); });
        QObject::connect(shortcutNbDecMinus, &QShortcut::activated, this, [this](){ this->spinBox_nbDigits->stepDown(); });
        QObject::connect(spinBox_nbDigits, &QSpinBox::valueChanged, this, &QIodeNumericalObjectWidget::saveSettings);

        // reload nb decimals
        loadSetting();
    }

    ~QIodeNumericalObjectWidget()
    {
        delete label_nbDigits;
        delete spinBox_nbDigits;
        delete shortcutNbDecPlus;
        delete shortcutNbDecMinus;
    }

    void setNbDigits(const int value)
    {
        this->objmodel->set_nb_digits(value);
        this->objmodel->reset();
    }

    void setProjectDir(const QDir& projectDir) override
    {
        QIodeObjectWidget<M, V>::setProjectDir(projectDir);
        loadSetting();
    }

    void loadSetting()
    {
        QSettings* project_settings = QIodeProjectSettings::getProjectSettings();
        if(!project_settings)
        {
            spinBox_nbDigits->setValue(2);
            return;
        }
        
        project_settings->beginGroup(getGroupName());
        int nb_digits = project_settings->value("nbDigits", spinBox_nbDigits->value()).toInt();
        spinBox_nbDigits->setValue(nb_digits);
        project_settings->endGroup();
    }

    void saveSettings()
    {
        QSettings* project_settings = QIodeProjectSettings::getProjectSettings();
        if(!project_settings)
            return;
        
        project_settings->beginGroup(getGroupName());
        project_settings->setValue("nbDigits", spinBox_nbDigits->value());
        project_settings->endGroup();
    }
};


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

class QIodeScalarsWidget : public QIodeNumericalObjectWidget<ScalarsModel, ScalarsView>
{
public:
    QIodeScalarsWidget(QIodeAbstractTabWidget* parent) : QIodeNumericalObjectWidget(I_SCALARS, parent) 
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

class QIodeVariablesWidget : public QIodeNumericalObjectWidget<VariablesModel, VariablesView>
{
public:
    QIodeVariablesWidget(QIodeAbstractTabWidget* parent) : QIodeNumericalObjectWidget(I_VARIABLES, parent) 
    {
        connect(objmodel, &VariablesModel::dataChanged, this, &QIodeVariablesWidget::databaseModified);
        connect(objmodel, &VariablesModel::headerDataChanged, this, &QIodeVariablesWidget::databaseModified);
        connect(objmodel, &VariablesModel::rowsInserted, this, &QIodeVariablesWidget::databaseModified);
        connect(objmodel, &VariablesModel::rowsRemoved, this, &QIodeVariablesWidget::databaseModified);
        connect(objmodel, &VariablesModel::databaseModified, this, &QIodeVariablesWidget::databaseModified);
        connect(tableview, &VariablesView::newObjectInserted, this, &QIodeVariablesWidget::databaseModified);
    }
};
