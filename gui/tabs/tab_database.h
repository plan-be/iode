#pragma once

#include <QLabel>
#include <QObject>
#include <QWidget>
#include <QSpinBox>
#include <QShortcut>
#include <QLineEdit>
#include <QGridLayout>
#include <QPushButton>
#include <QSizePolicy>
#include <QStringList>

#include "tab_abstract.h"

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

const static QString prefixUnsavedDatabase = "Unsaved";


class AbstractIodeObjectWidget: public AbstractTabWidget
{
protected:
    EnumIodeType iodeType;
    QDir projectDir;

    QGridLayout* layout;
    QLineEdit* lineEdit_filter;
    QPushButton* pushButton_filter;
    QPushButton* pushButton_print;
    QPushButton* pushButton_add;

public:
    AbstractIodeObjectWidget(const EnumIodeType iodeType, QWidget* parent = nullptr) : 
        AbstractTabWidget((EnumIodeFile) iodeType, "", parent), projectDir(QDir::homePath())
    {
        this->setObjectName(QString::fromUtf8("widget_iode_obj"));

        // layout
        layout = new QGridLayout(this);
        layout->setObjectName(QString::fromUtf8("layout"));
        layout->setContentsMargins(0, 0, 0, 0);

        // filter 
        lineEdit_filter = new QLineEdit();
        lineEdit_filter->setObjectName(QString::fromUtf8("lineEdit_filter"));
        lineEdit_filter->setPlaceholderText("filter pattern here");
        QSizePolicy sizePolicyFilter(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicyFilter.setHorizontalStretch(0);
        sizePolicyFilter.setVerticalStretch(0);
        sizePolicyFilter.setHeightForWidth(lineEdit_filter->sizePolicy().hasHeightForWidth());
        lineEdit_filter->setSizePolicy(sizePolicyFilter);
        lineEdit_filter->setMinimumSize(QSize(200, 0));
        layout->addWidget(lineEdit_filter, 0, 0, Qt::AlignLeft);

        pushButton_filter = new QPushButton("Filter");
        pushButton_filter->setObjectName(QString::fromUtf8("pushButton_filter"));
        layout->addWidget(pushButton_filter, 0, 1, Qt::AlignLeft);

        // spacer
        QSpacerItem* horizontalSpacer = new QSpacerItem(828, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        layout->addItem(horizontalSpacer, 0, 2);

        // print button
        pushButton_print = new QPushButton("Print");
        pushButton_print->setObjectName(QString::fromUtf8("pushButton_print"));
        layout->addWidget(pushButton_print, 0, 3, Qt::AlignLeft);

        // add button
        pushButton_add = new QPushButton("Add " + QString::fromStdString(vIodeTypes[iodeType]));
        pushButton_add->setObjectName(QString::fromUtf8("pushButton_add"));
        QSizePolicy sizePolicyAdd(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicyAdd.setHorizontalStretch(0);
        sizePolicyAdd.setVerticalStretch(0);
        sizePolicyAdd.setHeightForWidth(pushButton_add->sizePolicy().hasHeightForWidth());
        pushButton_add->setSizePolicy(sizePolicyAdd);
        layout->addWidget(pushButton_add, 0, 4, Qt::AlignRight);
    }

    ~AbstractIodeObjectWidget()
    {
        delete lineEdit_filter;
        delete pushButton_filter;
        delete pushButton_add;
        delete layout;
    }

    /**
     * @brief Return whether or not the tooltip is associated with a tab representing an unsaved KDB.
     * 
     * @param QString Tooltip of the tab. 
     * @return bool
     */
    bool isUnsavedDatabase() const
    {
        return filepath.isEmpty() || filepath == QString(I_DEFAULT_FILENAME);
    }

    QString getTabText() const
    {
        if(isUnsavedDatabase())
        {
            QString ext = QString::fromStdString(v_binary_ext[fileType]);
            // Note: the * is to tell that the content of the KDB has not been saved in file
            return tabPrefix[fileType] + QString(I_DEFAULT_FILENAME) + "." + ext + "*";
        }
        else
            return AbstractTabWidget::getTabText();
    }

    QString getTooltip() const
    {
        if(isUnsavedDatabase())
            return prefixUnsavedDatabase + " " + QString::fromStdString(vIodeTypes[(EnumIodeType) fileType]) + " Database";
        else
            return AbstractTabWidget::getTooltip();
    }

    bool updateFilepath(const QString& filepath) override
    {
        if(AbstractTabWidget::updateFilepath(filepath))
        {
            set_kdb_filename(K_WS[fileType], filepath.toStdString());
            return true;
        }
        else
            return false;
    }

    void reset()
    {
        this->filepath = "";
        clearKDB();
        resetFilter();
    }

    virtual void setProjectDir(const QDir& projectDir)
    {
        this->projectDir = projectDir;
        reset();
    }

    virtual void clearKDB() = 0;
    virtual void resetFilter() = 0;
    virtual QStringList getSelectedObjectsNames() const = 0;
    virtual void computeHash(const bool before=false) = 0;

public slots:
    void KDBModified()
    {
        setModified(true);
    }
};


template <class M, class V> class QIodeObjectWidget: public AbstractIodeObjectWidget
{
protected:
    M* objmodel;
    V* tableview;

    QShortcut* shortcutPrint;
    QShortcut* shortcutAdd;

public:
    QIodeObjectWidget(EnumIodeType iodeType, QWidget* parent = nullptr) : AbstractIodeObjectWidget(iodeType, parent)
    {
        // model table
        QWidget* mainwin = get_main_window_ptr();
        objmodel = new M(mainwin);

        // view table
        tableview = new V(parent);
        tableview->setObjectName(QString::fromUtf8("tableview"));
        tableview->setGeometry(QRect(10, 43, 950, 560));
        tableview->setupView(objmodel, lineEdit_filter);
        
        // shortcuts
        shortcutPrint = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_P), this);
        shortcutAdd = new QShortcut(QKeySequence(Qt::Key_Insert), this);

        // NOTE: By default, shortcuts are defined at the main Window level. 
        //       Thus, a shortcut of a (combination of) key(s) may override the expected behavior 
        //       from another widget dealing with the same (combination of) key(s). 
        //       'setContext(Qt::WidgetWithChildrenShortcut)' makes sure that the shortcut does 
        //       not propagate to other widgets.
        shortcutPrint->setContext(Qt::WidgetWithChildrenShortcut);
        shortcutAdd->setContext(Qt::WidgetWithChildrenShortcut);

        // signals - slots
        connect(lineEdit_filter, &QLineEdit::returnPressed, tableview, &V::filter);
        connect(pushButton_filter, &QPushButton::clicked, tableview, &V::filter);

        connect(pushButton_print, &QPushButton::clicked, tableview, &V::print);
        connect(shortcutPrint, &QShortcut::activated, tableview, &V::print);

        connect(pushButton_add, &QPushButton::clicked, tableview, &V::new_obj);
        connect(shortcutAdd, &QShortcut::activated, tableview, &V::new_obj);

        // insert table to layout
        // -1 -> span over all rows/columns
        layout->addWidget(tableview, 1, 0, 1, -1);
    }

    ~QIodeObjectWidget() 
    {
        delete objmodel;
        delete tableview;
        delete shortcutPrint;
        delete shortcutAdd;
    }

    M* get_model() const 
    {
        return objmodel;
    }

    V* get_view() const
    {
        return tableview;
    }

    void clearKDB()
    {
        objmodel->clearKDB();
        tableview->update();
    }

    void resetFilter()
    {
        lineEdit_filter->setText("");
        tableview->filter();
    }

    void update()
    {
        tableview->update();
    }

    void setup()
    {
        tableview->setup();
    }

    bool load_(const QString& filepath, const bool forceOverwrite)
    {
        return objmodel->load(filepath, forceOverwrite);
    }

    QString save()
    {
        if(isUnsavedDatabase()) 
            return saveAs();
        else
        {
            QString filepath = objmodel->save(projectDir);
            if(!filepath.isEmpty()) setModified(false);
            return filepath;
        }
    }

    QString saveAs_()
    {
        return objmodel->saveAs(projectDir);
    }

    QStringList getSelectedObjectsNames() const 
    {
        return tableview->getSelectedObjectsNames();
    }

    void computeHash(const bool before=false)
    {
        objmodel->computeHash(before);
    }
};


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
    QIodeNumericalObjectWidget(EnumIodeType iodeType, QWidget* parent = nullptr) 
        : QIodeObjectWidget<M, V>(iodeType, parent)
    {
        // make sure iodeType is defined before to call loadSetting() and getGroupName()
        this->iodeType = iodeType;

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
        QObject::connect(spinBox_nbDigits, &QSpinBox::valueChanged, this->objmodel, &M::set_nb_digits);
        QObject::connect(shortcutNbDecPlus, &QShortcut::activated, this->objmodel, &M::nb_dec_plus);
        QObject::connect(shortcutNbDecMinus, &QShortcut::activated, this->objmodel, &M::nb_dec_minus);
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

public slots:
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
    QIodeCommentsWidget(QWidget* parent = nullptr) : QIodeObjectWidget(I_COMMENTS, parent) 
    {
        connect(objmodel, &CommentsModel::dataChanged, this, &QIodeCommentsWidget::KDBModified);
        connect(objmodel, &CommentsModel::headerDataChanged, this, &QIodeCommentsWidget::KDBModified);
        connect(objmodel, &CommentsModel::rowsInserted, this, &QIodeCommentsWidget::KDBModified);
        connect(objmodel, &CommentsModel::rowsRemoved, this, &QIodeCommentsWidget::KDBModified);
        connect(objmodel, &CommentsModel::databaseModified, this, &QIodeCommentsWidget::KDBModified);
        connect(tableview, &CommentsView::newObjectInserted, this, &QIodeCommentsWidget::KDBModified);
    }
};

class QIodeEquationsWidget : public QIodeObjectWidget<EquationsModel, EquationsView>
{
public:
    QIodeEquationsWidget(QWidget* parent = nullptr) : QIodeObjectWidget(I_EQUATIONS, parent) 
    {
        connect(objmodel, &EquationsModel::dataChanged, this, &QIodeEquationsWidget::KDBModified);
        connect(objmodel, &EquationsModel::headerDataChanged, this, &QIodeEquationsWidget::KDBModified);
        connect(objmodel, &EquationsModel::rowsInserted, this, &QIodeEquationsWidget::KDBModified);
        connect(objmodel, &EquationsModel::rowsRemoved, this, &QIodeEquationsWidget::KDBModified);
        connect(objmodel, &EquationsModel::databaseModified, this, &QIodeEquationsWidget::KDBModified);
        connect(tableview, &EquationsView::newObjectInserted, this, &QIodeEquationsWidget::KDBModified);
        connect(tableview, &EquationsView::equationModified, this, &QIodeEquationsWidget::KDBModified);
    }
};

class QIodeIdentitiesWidget : public QIodeObjectWidget<IdentitiesModel, IdentitiesView>
{
public:
    QIodeIdentitiesWidget(QWidget* parent = nullptr) : QIodeObjectWidget(I_IDENTITIES, parent) 
    {
        connect(objmodel, &IdentitiesModel::dataChanged, this, &QIodeIdentitiesWidget::KDBModified);
        connect(objmodel, &IdentitiesModel::headerDataChanged, this, &QIodeIdentitiesWidget::KDBModified);
        connect(objmodel, &IdentitiesModel::rowsInserted, this, &QIodeIdentitiesWidget::KDBModified);
        connect(objmodel, &IdentitiesModel::rowsRemoved, this, &QIodeIdentitiesWidget::KDBModified);
        connect(objmodel, &IdentitiesModel::databaseModified, this, &QIodeIdentitiesWidget::KDBModified);
        connect(tableview, &IdentitiesView::newObjectInserted, this, &QIodeIdentitiesWidget::KDBModified);
    }
};

class QIodeListsWidget : public QIodeObjectWidget<ListsModel, ListsView>
{
public:
    QIodeListsWidget(QWidget* parent = nullptr) : QIodeObjectWidget(I_LISTS, parent) 
    {
        connect(objmodel, &ListsModel::dataChanged, this, &QIodeListsWidget::KDBModified);
        connect(objmodel, &ListsModel::headerDataChanged, this, &QIodeListsWidget::KDBModified);
        connect(objmodel, &ListsModel::rowsInserted, this, &QIodeListsWidget::KDBModified);
        connect(objmodel, &ListsModel::rowsRemoved, this, &QIodeListsWidget::KDBModified);
        connect(objmodel, &ListsModel::databaseModified, this, &QIodeListsWidget::KDBModified);
        connect(tableview, &ListsView::newObjectInserted, this, &QIodeListsWidget::KDBModified);
    }
};

class QIodeScalarsWidget : public QIodeNumericalObjectWidget<ScalarsModel, ScalarsView>
{
public:
    QIodeScalarsWidget(QWidget* parent = nullptr) : QIodeNumericalObjectWidget(I_SCALARS, parent) 
    {
        connect(objmodel, &ScalarsModel::dataChanged, this, &QIodeScalarsWidget::KDBModified);
        connect(objmodel, &ScalarsModel::headerDataChanged, this, &QIodeScalarsWidget::KDBModified);
        connect(objmodel, &ScalarsModel::rowsInserted, this, &QIodeScalarsWidget::KDBModified);
        connect(objmodel, &ScalarsModel::rowsRemoved, this, &QIodeScalarsWidget::KDBModified);
        connect(objmodel, &ScalarsModel::databaseModified, this, &QIodeScalarsWidget::KDBModified);
        connect(tableview, &ScalarsView::newObjectInserted, this, &QIodeScalarsWidget::KDBModified);
    }
};

class QIodeTablesWidget : public QIodeObjectWidget<TablesModel, TablesView>
{
public:
    QIodeTablesWidget(QWidget* parent = nullptr) : QIodeObjectWidget(I_TABLES, parent) 
    {
        connect(objmodel, &TablesModel::dataChanged, this, &QIodeTablesWidget::KDBModified);
        connect(objmodel, &TablesModel::headerDataChanged, this, &QIodeTablesWidget::KDBModified);
        connect(objmodel, &TablesModel::rowsInserted, this, &QIodeTablesWidget::KDBModified);
        connect(objmodel, &TablesModel::rowsRemoved, this, &QIodeTablesWidget::KDBModified);
        connect(objmodel, &TablesModel::databaseModified, this, &QIodeTablesWidget::KDBModified);
        connect(tableview, &TablesView::newObjectInserted, this, &QIodeTablesWidget::KDBModified);
        connect(tableview, &TablesView::tableModified, this, &QIodeTablesWidget::KDBModified);
    }
};

class QIodeVariablesWidget : public QIodeNumericalObjectWidget<VariablesModel, VariablesView>
{
public:
    QIodeVariablesWidget(QWidget* parent = nullptr) : QIodeNumericalObjectWidget(I_VARIABLES, parent) 
    {
        connect(objmodel, &VariablesModel::dataChanged, this, &QIodeVariablesWidget::KDBModified);
        connect(objmodel, &VariablesModel::headerDataChanged, this, &QIodeVariablesWidget::KDBModified);
        connect(objmodel, &VariablesModel::rowsInserted, this, &QIodeVariablesWidget::KDBModified);
        connect(objmodel, &VariablesModel::rowsRemoved, this, &QIodeVariablesWidget::KDBModified);
        connect(objmodel, &VariablesModel::databaseModified, this, &QIodeVariablesWidget::KDBModified);
        connect(tableview, &VariablesView::newObjectInserted, this, &QIodeVariablesWidget::KDBModified);
    }
};
