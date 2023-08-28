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
#include "custom_widgets/tab_widget/tab_widget_abstract.h"


const static QString prefixUnsavedDatabase = "Unsaved";


class AbstractIodeObjectWidget: public AbstractTabWidget
{
    Q_OBJECT

protected:
    EnumIodeType iodeType;
    QDir projectDir;

    int row;
    QGridLayout* gridLayout;
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
        row = 0;
        gridLayout = new QGridLayout(this);
        gridLayout->setObjectName(QString::fromUtf8("layout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);

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
        gridLayout->addWidget(lineEdit_filter, row, 0, Qt::AlignLeft);

        pushButton_filter = new QPushButton("Filter");
        pushButton_filter->setObjectName(QString::fromUtf8("pushButton_filter"));
        gridLayout->addWidget(pushButton_filter, row, 1, Qt::AlignLeft);

        // spacer
        QSpacerItem* horizontalSpacer = new QSpacerItem(828, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        gridLayout->addItem(horizontalSpacer, row, 2);

        // print button
        pushButton_print = new QPushButton("Print");
        pushButton_print->setObjectName(QString::fromUtf8("pushButton_print"));
        gridLayout->addWidget(pushButton_print, row, 3, Qt::AlignLeft);

        // add button
        pushButton_add = new QPushButton("Add " + QString::fromStdString(vIodeTypes[iodeType]));
        pushButton_add->setObjectName(QString::fromUtf8("pushButton_add"));
        QSizePolicy sizePolicyAdd(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicyAdd.setHorizontalStretch(0);
        sizePolicyAdd.setVerticalStretch(0);
        sizePolicyAdd.setHeightForWidth(pushButton_add->sizePolicy().hasHeightForWidth());
        pushButton_add->setSizePolicy(sizePolicyAdd);
        gridLayout->addWidget(pushButton_add, row, 4, Qt::AlignRight);

        row++;
    }

    ~AbstractIodeObjectWidget()
    {
        delete lineEdit_filter;
        delete pushButton_filter;
        delete pushButton_add;
        if(gridLayout) delete gridLayout;
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
    V* tableview_2;

    QSplitter* splitter;

    QShortcut* shortcutPrint;
    QShortcut* shortcutAdd;

public:
    QIodeObjectWidget(EnumIodeType iodeType, QIodeAbstractTabWidget* parent) 
        : AbstractIodeObjectWidget(iodeType, parent)
    {
        // prepare splitter
        splitter = new QSplitter(parent);
        splitter->setObjectName("splitter");
        splitter->setGeometry(QRect(10, 43, 950, 560));
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
        connect(lineEdit_filter, &QLineEdit::returnPressed, tableview, &V::filter);
        connect(lineEdit_filter, &QLineEdit::returnPressed, tableview_2, &V::filter);
        connect(pushButton_filter, &QPushButton::clicked, tableview, &V::filter);
        connect(pushButton_filter, &QPushButton::clicked, tableview_2, &V::filter);

        connect(pushButton_print, &QPushButton::clicked, tableview, &V::print);
        connect(shortcutPrint, &QShortcut::activated, tableview, &V::print);

        connect(pushButton_add, &QPushButton::clicked, tableview, &V::new_obj);
        connect(shortcutAdd, &QShortcut::activated, tableview, &V::new_obj);

        connect(tableview, &AbstractTableView::showObjsRequest, parent, &QIodeAbstractTabWidget::showObjectsList);
        connect(tableview_2, &AbstractTableView::showObjsRequest, parent, &QIodeAbstractTabWidget::showObjectsList);

        splitter->addWidget(tableview);
        splitter->addWidget(tableview_2);
        tableview_2->hide();

        // insert table to layout
        // -1 -> span over all rows/columns
        gridLayout->addWidget(splitter, row, 0, 1, -1);
        row++;
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
