#pragma once

#include <QObject>
#include <QWidget>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSizePolicy>

#include "qiode_tab_abstract.h"

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

    QGridLayout* layout;
    QLineEdit* lineEdit_filter;
    QPushButton* pushButton_filter;
    QPushButton* add_iode_obj;

public:
    AbstractIodeObjectWidget(const EnumIodeType iodeType, QWidget* parent = nullptr) : 
        AbstractTabWidget((EnumIodeFile) iodeType, "", parent)
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
            QString ext = QString::fromStdString(vFileExtensions[fileType].ext);
            // Note: the * is to tell that the content of the KDB has not been saved in file
            return tabPrefix[fileType] + QString(I_DEFAULT_FILENAME) + ext + "*";
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

    virtual void clearKDB() = 0;
    virtual void resetFilter() = 0;
};


template <class M, class V> class QIodeObjectWidget: public AbstractIodeObjectWidget
{
    M* objmodel;
    V* tableview;
    QDir projectDir;

public:
    QIodeObjectWidget(EnumIodeType iodeType, QWidget* parent = nullptr) : 
        AbstractIodeObjectWidget(iodeType, parent), projectDir(QDir::homePath())
    {
        // model table
        QWidget* mainwin = get_main_window_ptr();
        objmodel = new M(mainwin);

        // view table
        tableview = new V(parent);
        tableview->setObjectName(QString::fromUtf8("tableview"));
        tableview->setGeometry(QRect(10, 43, 950, 560));
        tableview->setupView(objmodel, lineEdit_filter);
        
        // signals - slots
        connect(lineEdit_filter, &QLineEdit::returnPressed, tableview, &V::filter);
        connect(pushButton_filter, &QPushButton::clicked, tableview, &V::filter);
        connect(add_iode_obj, &QPushButton::clicked, tableview, &V::new_obj);

        // insert table to layout
        // -1 -> span over all rows/columns
        layout->addWidget(tableview, 1, 0, -1, -1);
    }

    ~QIodeObjectWidget() 
    {
        delete objmodel;
        delete tableview;
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

    void setProjectDir(const QDir& projectDir)
    {
        this->projectDir = projectDir;
        
        clearKDB();
    }

    void setProjectSettingsFilepath(std::shared_ptr<QString>& settings_filepath)
    {
        tableview->setProjectSettingsFilepath(settings_filepath);
    }

    bool load_(const QString& filepath, const bool forceOverwrite)
    {
        return objmodel->load(filepath, forceOverwrite);
    }

    QString save()
    {
        return objmodel->save(projectDir);
    }

    QString saveAs_()
    {
        return objmodel->saveAs(projectDir);
    }
};

class QIodeCommentsWidget : public QIodeObjectWidget<CommentsModel, CommentsView>
{
public:
    QIodeCommentsWidget(EnumIodeType iodeType, QWidget* parent = nullptr) : QIodeObjectWidget(iodeType, parent) {}
};

class QIodeEquationsWidget : public QIodeObjectWidget<EquationsModel, EquationsView>
{
public:
    QIodeEquationsWidget(EnumIodeType iodeType, QWidget* parent = nullptr) : QIodeObjectWidget(iodeType, parent) {}
};

class QIodeIdentitiesWidget : public QIodeObjectWidget<IdentitiesModel, IdentitiesView>
{
public:
    QIodeIdentitiesWidget(EnumIodeType iodeType, QWidget* parent = nullptr) : QIodeObjectWidget(iodeType, parent) {}
};

class QIodeListsWidget : public QIodeObjectWidget<ListsModel, ListsView>
{
public:
    QIodeListsWidget(EnumIodeType iodeType, QWidget* parent = nullptr) : QIodeObjectWidget(iodeType, parent) {}
};

class QIodeScalarsWidget : public QIodeObjectWidget<ScalarsModel, ScalarsView>
{
public:
    QIodeScalarsWidget(EnumIodeType iodeType, QWidget* parent = nullptr) : QIodeObjectWidget(iodeType, parent) {}
};

class QIodeTablesWidget : public QIodeObjectWidget<TablesModel, TablesView>
{
public:
    QIodeTablesWidget(EnumIodeType iodeType, QWidget* parent = nullptr) : QIodeObjectWidget(iodeType, parent) {}
};

class QIodeVariablesWidget : public QIodeObjectWidget<VariablesModel, VariablesView>
{
public:
    QIodeVariablesWidget(EnumIodeType iodeType, QWidget* parent = nullptr) : QIodeObjectWidget(iodeType, parent) {}
};
