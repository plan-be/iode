#pragma once

#include <QObject>
#include <QWidget>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSizePolicy>

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


class AbstractTabWidget: public QWidget
{
protected:
    EnumIodeFile fileType;

public:
    AbstractTabWidget(const EnumIodeFile fileType, QWidget* parent = nullptr) : 
        QWidget(parent), fileType(fileType) 
    {
        this->setGeometry(QRect(10, 11, 951, 26));
    }

    EnumIodeFile getFiletype() const { return fileType; }

    virtual void update() = 0;
    virtual QString save() = 0;
    virtual QString saveAs() = 0;
};


class AbstractIodeObjectWidget: public AbstractTabWidget
{
protected:
    EnumIodeType iodeType;
    QGridLayout* layout;
    QLineEdit* lineEdit_filter;
    QPushButton* pushButton_filter;
    QPushButton* add_iode_obj;

public:
    AbstractIodeObjectWidget(const EnumIodeType iodeType, QWidget* parent = nullptr) : 
        AbstractTabWidget((EnumIodeFile) iodeType, parent)
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

        // spacers
        QSpacerItem* horizontalSpacer = new QSpacerItem(828, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        layout->addItem(horizontalSpacer, 0, 2);

        // add button
        add_iode_obj = new QPushButton("Add " + QString::fromStdString(vIodeTypes[iodeType]));
        add_iode_obj->setObjectName(QString::fromUtf8("add_iode_obj"));
        QSizePolicy sizePolicyAdd(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicyAdd.setHorizontalStretch(0);
        sizePolicyAdd.setVerticalStretch(0);
        sizePolicyAdd.setHeightForWidth(add_iode_obj->sizePolicy().hasHeightForWidth());
        add_iode_obj->setSizePolicy(sizePolicyAdd);
        layout->addWidget(add_iode_obj, 0, 3, Qt::AlignRight);
    }

    ~AbstractIodeObjectWidget()
    {
        delete lineEdit_filter;
        delete pushButton_filter;
        delete add_iode_obj;
        delete layout;
    }

    virtual void clearKDB() = 0;
    virtual void resetFilter() = 0;
    virtual void update() = 0;
    virtual QString save() = 0;
    virtual QString saveAs() = 0;
};


template <class M, class V> class QIodeObjectWidget: public AbstractIodeObjectWidget
{
    M* objmodel;
    V* tableview;
    QDir projectDir;

public:
    QIodeObjectWidget(std::shared_ptr<QString>& settings_filepath, EnumIodeType iodeType, 
        QWidget* parent = nullptr) : AbstractIodeObjectWidget(iodeType, parent), projectDir(QDir::homePath())
    {
        QString stylesheet = "QHeaderView::section { background-color: lightGray; font: bold; border: 0.5px solid }";

        // model table
        QWidget* mainwin = get_main_window_ptr();
        objmodel = new M(mainwin);

        // view table
        tableview = new V(parent);
        tableview->setObjectName(QString::fromUtf8("tableview"));
        tableview->setGeometry(QRect(10, 43, 950, 560));
        tableview->setupView(objmodel, lineEdit_filter, settings_filepath);
        
        // signals - slots
        connect(lineEdit_filter, &QLineEdit::returnPressed, tableview, &V::filter);
        connect(pushButton_filter, &QPushButton::clicked, tableview, &V::filter);
        connect(add_iode_obj, &QPushButton::clicked, tableview, &V::new_obj);

        // size policy
        QSizePolicy sizePolicyTable(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicyTable.setHorizontalStretch(0);
        sizePolicyTable.setVerticalStretch(0);
        sizePolicyTable.setHeightForWidth(tableview->sizePolicy().hasHeightForWidth());
        tableview->setSizePolicy(sizePolicyTable);
        tableview->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
        // sorting
        tableview->setSortingEnabled(true);
        // headers
        tableview->horizontalHeader()->setStretchLastSection(true);
        tableview->verticalHeader()->setVisible(false);
        tableview->verticalHeader()->setStretchLastSection(true);
        // stylesheet
        tableview->setStyleSheet(stylesheet);

        // insert table to layout
        // -1 -> span over all rows/columns
        layout->addWidget(tableview, 1, 0, -1, -1);
    }

    ~QIodeObjectWidget() 
    {
        delete objmodel;
        delete tableview;
    }

    void clearKDB()
    {
        objmodel->clearKDB();
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

    void setProjectDir(const QDir& projectDir)
    {
        this->projectDir = projectDir;
        clearKDB();
    }

    QString save()
    {
        return objmodel->save(projectDir);
    }

    QString saveAs()
    {
        return objmodel->saveAs(projectDir);
    }
};

class QIodeCommentsWidget : public QIodeObjectWidget<CommentsModel, CommentsView>
{
public:
    QIodeCommentsWidget(std::shared_ptr<QString>& settings_filepath, EnumIodeType iodeType, QWidget* parent = nullptr) : 
        QIodeObjectWidget(settings_filepath, iodeType, parent) {}
};

class QIodeEquationsWidget : public QIodeObjectWidget<EquationsModel, EquationsView>
{
public:
    QIodeEquationsWidget(std::shared_ptr<QString>& settings_filepath, EnumIodeType iodeType, QWidget* parent = nullptr) : 
        QIodeObjectWidget(settings_filepath, iodeType, parent) {}
};

class QIodeIdentitiesWidget : public QIodeObjectWidget<IdentitiesModel, IdentitiesView>
{
public:
    QIodeIdentitiesWidget(std::shared_ptr<QString>& settings_filepath, EnumIodeType iodeType, QWidget* parent = nullptr) : 
        QIodeObjectWidget(settings_filepath, iodeType, parent) {}
};

class QIodeListsWidget : public QIodeObjectWidget<ListsModel, ListsView>
{
public:
    QIodeListsWidget(std::shared_ptr<QString>& settings_filepath, EnumIodeType iodeType, QWidget* parent = nullptr) : 
        QIodeObjectWidget(settings_filepath, iodeType, parent) {}
};

class QIodeScalarsWidget : public QIodeObjectWidget<ScalarsModel, ScalarsView>
{
public:
    QIodeScalarsWidget(std::shared_ptr<QString>& settings_filepath, EnumIodeType iodeType, QWidget* parent = nullptr) : 
        QIodeObjectWidget(settings_filepath, iodeType, parent) {}
};

class QIodeTablesWidget : public QIodeObjectWidget<TablesModel, TablesView>
{
public:
    QIodeTablesWidget(std::shared_ptr<QString>& settings_filepath, EnumIodeType iodeType, QWidget* parent = nullptr) : 
        QIodeObjectWidget(settings_filepath, iodeType, parent) {}
};

class QIodeVariablesWidget : public QIodeObjectWidget<VariablesModel, VariablesView>
{
public:
    QIodeVariablesWidget(std::shared_ptr<QString>& settings_filepath, EnumIodeType iodeType, QWidget* parent = nullptr) : 
        QIodeObjectWidget(settings_filepath, iodeType, parent) {}
};
