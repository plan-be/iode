#pragma once

#include <QLabel>
#include <QObject>
#include <QWidget>
#include <QSpinBox>
#include <QShortcut>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSizePolicy>
#include <QStringList>

#include "tabs/tab_abstract.h"
#include "main_widgets/tab_widget/tab_widget_abstract.h"


const static QString prefixUnsavedDatabase = "Unsaved";


class AbstractIodeObjectWidget: public IodeAbstractWidget
{
    Q_OBJECT

protected:
    IodeType iodeType;
    QDir projectDir;

    QVBoxLayout* vLayout;
    QLineEdit* lineEdit_filter;
    QPushButton* pushButton_filter;
    QPushButton* pushButton_print;
    QPushButton* pushButton_add;

signals:
    void tabDatabaseModified(const IodeType iodeType, const bool modified);

public:
    AbstractIodeObjectWidget(const IodeType iodeType, QWidget* parent) : 
        IodeAbstractWidget((IodeFileType) iodeType, parent), projectDir(QDir::homePath()), iodeType(iodeType)
    {
        this->setObjectName(QString::fromUtf8("widget_iode_obj"));

        /* NOTE FOR THE DEVELOPERS:
         * I first tried to use a QGridLayout but some widgets were misaligned in the tabs. 
         * I could,'t find out why.
         * So I finally decided to mix QVBoxLayout and QHBoxLayout which solved the problem.
         */

        // layout
        vLayout = new QVBoxLayout(this);
        vLayout->setObjectName(QString::fromUtf8("vTabLayout"));
        vLayout->setContentsMargins(0, 0, 0, 0);

        // top horizontal layout
        QHBoxLayout* topHLayout = new QHBoxLayout(this);

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
        topHLayout->addWidget(lineEdit_filter, Qt::AlignLeft);

        pushButton_filter = new QPushButton("Filter");
        pushButton_filter->setObjectName(QString::fromUtf8("pushButton_filter"));
        topHLayout->addWidget(pushButton_filter, Qt::AlignLeft);

        // spacer
        QSpacerItem* horizontalSpacer = new QSpacerItem(828, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        topHLayout->addSpacerItem(horizontalSpacer);

        // print button
        pushButton_print = new QPushButton("Print");
        pushButton_print->setObjectName(QString::fromUtf8("pushButton_print"));
        topHLayout->addWidget(pushButton_print, Qt::AlignLeft);

        // add button
        pushButton_add = new QPushButton("Add " + QString::fromStdString(v_iode_types[iodeType]));
        pushButton_add->setObjectName(QString::fromUtf8("pushButton_add"));
        QSizePolicy sizePolicyAdd(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicyAdd.setHorizontalStretch(0);
        sizePolicyAdd.setVerticalStretch(0);
        sizePolicyAdd.setHeightForWidth(pushButton_add->sizePolicy().hasHeightForWidth());
        pushButton_add->setSizePolicy(sizePolicyAdd);
        topHLayout->addWidget(pushButton_add, Qt::AlignRight);

        vLayout->addLayout(topHLayout);
    }

    ~AbstractIodeObjectWidget()
    {
        delete lineEdit_filter;
        delete pushButton_filter;
        delete pushButton_add;
        if(vLayout) delete vLayout;
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

    bool updateFilepath(const QString& filepath) override
    {
        if(checkNewFilepath(filepath))
        {
            set_kdb_filename(K_WS[fileType], filepath.toLocal8Bit().toStdString());
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
    }

    virtual void setProjectDir(const QDir& projectDir)
    {
        this->projectDir = projectDir;
        QSettings* project_settings = ProjectSettings::getProjectSettings();
        loadSettings(project_settings);
        reset();
    }

    virtual void clearKDB() = 0;
    virtual void filter(const bool silent = false) = 0;
    virtual void filter(const QString& pattern, const bool silent = false) = 0;
    virtual void resetFilter() = 0;
    virtual QStringList getSelectedObjectsNames() const = 0;
    virtual void openAddDialog() = 0;
    virtual void openEditDialog() = 0;
    virtual void computeHash(const bool before=false) = 0;
    virtual int preferredHeight() = 0;
    virtual void resetModel() = 0;

public slots:
    void setModified(bool modified) override
    {
        this->modified = modified;
        if(modified)
        {
            // we need to recompute the subset database in the IodeTemplateTableModel class if the filter is active 
            // -> executing a command line or an IODE report may have renamed, added or removed IODE objects and 
            //    then made the subset database invalid (remember that the subset database in the 
            //    IodeTemplateTableModel class is a shallow copy of a subset of a global IODE database)  
		    filter(true);
            resetModel(); 
        }
        emit tabDatabaseModified(iodeType, modified);
    }

    void databaseModified()
    {
        setModified(true);
    }
};


template <class M, class V> class TemplateIodeObjectWidget: public AbstractIodeObjectWidget
{
protected:
    M* objmodel;
    V* tableview;
    V* tableview_2;

    QSplitter* splitter;

    QShortcut* shortcutPrint;
    QShortcut* shortcutAdd;

public:
    TemplateIodeObjectWidget(IodeType iodeType, QWidget* parent) : AbstractIodeObjectWidget(iodeType, parent)
    {
        // prepare splitter
        splitter = new QSplitter(parent);
        splitter->setObjectName("splitter");
        splitter->move(10, 43);
        splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        
        splitter->setOrientation(Qt::Horizontal);
        splitter_ = splitter;

        // view table
        tableview = new V(splitter);
        tableview->setObjectName(QString::fromUtf8("tableview"));
        
        tableview_2 = new V(splitter);
        tableview_2->setObjectName(QString::fromUtf8("tableview_2"));

        // model table
        objmodel = new M(tableview);
        tableview->setModel(objmodel);
        tableview_2->setModel(objmodel);

        // IODE objects names filter
        tableview->setFilterLineEdit(lineEdit_filter);
        tableview_2->setFilterLineEdit(lineEdit_filter);
        
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
        // NOTE: no need to connect lineEdit_filter and pushButton_filter to tableview_2 since 
        //       both views share the same model
        connect(lineEdit_filter, &QLineEdit::returnPressed, tableview, &V::filter_slot);
        connect(pushButton_filter, &QPushButton::clicked, tableview, &V::filter_slot);

        connect(pushButton_print, &QPushButton::clicked, tableview, &V::print);
        connect(shortcutPrint, &QShortcut::activated, tableview, &V::print);

        connect(pushButton_add, &QPushButton::clicked, tableview, &V::new_obj);
        connect(shortcutAdd, &QShortcut::activated, tableview, &V::new_obj);

        IodeAbstractTabWidget* tabWidget = static_cast<IodeAbstractTabWidget*>(get_tabs_widget_ptr());
        connect(tableview, &IodeAbstractTableView::showObjsRequest, tabWidget, &IodeAbstractTabWidget::showObjectsList);
        connect(tableview_2, &IodeAbstractTableView::showObjsRequest, tabWidget, &IodeAbstractTabWidget::showObjectsList);

        // update filter pattern when an object is removed
        connect(objmodel, &M::objectRemoved, tableview, &V::objectRemoved);

        splitter->addWidget(tableview);
        splitter->addWidget(tableview_2);
        tableview_2->hide();

        // insert table to layout
        // -1 -> span over all rows/columns
        vLayout->addWidget(splitter);
    }

    ~TemplateIodeObjectWidget() 
    {
        delete objmodel;
        delete tableview;
        delete shortcutPrint;
        delete shortcutAdd;
    }

    /**
     * @brief get the preferred height
     */
    int preferredHeight()
    {
        int h = (vLayout->count() - 1) * 30; 
        return h + tableview->preferredHeight();
    }

    QString getTabText() const
    {
        if(isUnsavedDatabase())
        {
            QString ext = QString::fromStdString(v_file_types[fileType].v_ext[0]);
            // Note: the * is to tell that the content of the KDB has not been saved in file
            return tabPrefix[fileType] + QString(I_DEFAULT_FILENAME) + ext + " [0]*";
        }
        else
            return IodeAbstractWidget::getTabText() + " [" + QString::number(objmodel->getNbObjects()) + "]";
    }

    QString getTooltip() const
    {
        if(isUnsavedDatabase())
            return prefixUnsavedDatabase + " " + QString::fromStdString(v_iode_types[iodeType]) + " Database [0]";
        else
            return IodeAbstractWidget::getTooltip() + " [" + QString::number(objmodel->getNbObjects()) + "]";
    }

    void resetModel() override
    {
        objmodel->reset();
    }

    void openAddDialog() override
    {
        tableview->new_obj();
    }
    
    void openEditDialog() override
    {
        tableview->edit_obj();
    }

    QString getFilepath() const
    {
        return objmodel->getFilepath();
    }

    void clearKDB()
    {
        objmodel->clearKDB();
        resetFilter();
    }

    void filter(const bool silent = false)
    {
        tableview->filter(silent);
    }

    void filter(const QString& pattern, const bool silent = false)
    {
        lineEdit_filter->setText(pattern);
        tableview->filter(silent);
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
        tableview_2->setup();
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
        QStringList list;
        list = tableview->getSelectedObjectsNames();
        if(tableview_2->isVisible())
            list += tableview_2->getSelectedObjectsNames();
        return list;
    }

    void computeHash(const bool before=false)
    {
        objmodel->computeHash(before);
    }

    void split(const Qt::Orientation orientation) override
    {
        if(splitted_)
            return;

        if(splitter->orientation() != orientation)
            splitter->setOrientation(orientation);

        tableview_2->show();
        splitted_ = true;
    }

    void unsplit() override
    {
        if(!splitted_)
            return;

        tableview_2->hide();
        splitted_ = false;
    }
};
