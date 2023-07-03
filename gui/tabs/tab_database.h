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
#include "custom_widgets/tab_widget_abstract.h"

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
    Q_OBJECT

protected:
    EnumIodeType iodeType;
    QDir projectDir;

    QGridLayout* layout;
    QLineEdit* lineEdit_filter;
    QPushButton* pushButton_filter;
    QPushButton* pushButton_print;
    QPushButton* pushButton_add;

signals:
    void tabDatabaseModified(const EnumIodeType iodeType, const bool modified);

public:
    AbstractIodeObjectWidget(const EnumIodeType iodeType, QIodeAbstractTabWidget* parent) : 
        AbstractTabWidget((EnumIodeFile) iodeType, parent), projectDir(QDir::homePath()), iodeType(iodeType)
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
        QString filepath = getFilepath();
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
            return prefixUnsavedDatabase + " " + QString::fromStdString(vIodeTypes[iodeType]) + " Database";
        else
            return AbstractTabWidget::getTooltip();
    }

    bool updateFilepath(const QString& filepath) override
    {
        if(checkNewFilepath(filepath))
        {
            set_kdb_filename(K_WS[fileType], filepath.toStdString());
            modified = false;
            emit tabDatabaseModified(iodeType, false);
            return true;
        }
        else
            return false;
    }

    void reset()
    {
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
    void setModified(bool modified) override
    {
        this->modified = modified;
        emit tabDatabaseModified(iodeType, modified); 
    }

    void databaseModified()
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
    QIodeObjectWidget(EnumIodeType iodeType, QIodeAbstractTabWidget* parent) 
        : AbstractIodeObjectWidget(iodeType, parent)
    {
        // view table
        tableview = new V(parent);
        tableview->setObjectName(QString::fromUtf8("tableview"));
        tableview->setGeometry(QRect(10, 43, 950, 560));

        // model table
        objmodel = new M(tableview);
        tableview->setModel(objmodel);

        // IODE objects names filter
        tableview->setFilterLineEdit(lineEdit_filter);
        
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

        connect(tableview, &AbstractTableView::showObjsRequest, parent, &QIodeAbstractTabWidget::showObjectsList);

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

    QString getFilepath() const
    {
        return objmodel->getFilepath();
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
