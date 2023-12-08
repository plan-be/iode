/* NOTE FOR THE DEVELOPPERS:
 * 1. We do not use the preprocessor directive #pragma once so that two 
 *    different versions of the class _NUMERICAL_WIDGET_CLASS_NAME_ below can be built.
 *    Depending on if _GSAMPLE_ is defined or not.
 * 
 * 2. Multiple Inheritance Requires QObject to Be First
 *    If you are using multiple inheritance, moc assumes that the first inherited class
 *    is a subclass of QObject. Also, be sure that only the first inherited class is a QObject.
 *    https://doc.qt.io/qt-6/moc.html#multiple-inheritance-requires-qobject-to-be-first
 *    --> That's why we to play with preprocessor directives to inherit from either 
 *        QWidget or TemplateIodeObjectWidget.
 * 
 * 3. Do NOT forget to add safeguards when including the present hearder!
 */

#include <QLabel>
#include <QDialog>
#include <QSpinBox>
#include <QCheckBox>
#include <QShortcut>
#include <QSpacerItem>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>

#ifdef _GSAMPLE_
#define GSAMPLE_NUMERICAL_WIDGET_HEADER

#include "utils.h"
#include "settings.h"
#include "iode_objs/models/gsample_table_model.h"

// reverse #undef _GSAMPLE_ from gsample_table_model.h
#define _GSAMPLE_

#ifndef GSAMPLE_NUMERICAL_VIEW_HEADER
#include "iode_objs/views/numerical_view.h"
#endif

#define _NUMERICAL_WIDGET_CLASS_NAME_ GSampleNumericalDialog

/**
 * @brief Defines a widget containing a GSample table and
 *        a QSpinBox indicating the number of significant 
 *        digits to display.
 */
class _NUMERICAL_WIDGET_CLASS_NAME_ : public QDialog
#else
#define TEMPLATE_NUMERICAL_WIDGET_HEADER

#include "tab_database_abstract.h"

#define _NUMERICAL_WIDGET_CLASS_NAME_ TemplateNumericalWidget

/**
 * @brief Defines a widget containing a Scalar or Variable table 
 *        and a QSpinBox indicating the number of significant 
 *        digits to display.
 */
template <class M, class V> class _NUMERICAL_WIDGET_CLASS_NAME_ : public TemplateIodeObjectWidget<M, V>
#endif
{
#ifdef _GSAMPLE_
    QVBoxLayout* vLayout;

    GSampleNumericalTableView* tableview;
    GSampleTableModel* objmodel;
#endif

    static const int COLUMN_SIZE_STEP = 10;

    QLabel* label_precision;
    QSpinBox* spinBox_precision;

    QCheckBox* checkbox_scientific;

    QShortcut* shortcutPrecisionPlus;
    QShortcut* shortcutPrecisionMinus;
    QShortcut* shortcutIncreaseColumnSize;
    QShortcut* shortcutDecreaseColumnSize;
    QShortcut* shortcutResizeToContents;

protected:
    QHBoxLayout* bottomLayout;

protected:
#ifdef _GSAMPLE_
    QString getGroupName()
    {
        return "GSAMPLE_TABLE";
    }

    void closeEvent(QCloseEvent* event) override
    {
        saveSettings();
        event->accept();
    }
#endif

public:
#ifdef _GSAMPLE_
    _NUMERICAL_WIDGET_CLASS_NAME_(const QString& refTable, const QString& gsample, const int nbDecimals, 
        const QString& variables, QWidget* parent = nullptr): 
        QDialog(parent, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint)
#else
    _NUMERICAL_WIDGET_CLASS_NAME_(EnumIodeType iodeType, IodeAbstractTabWidget* parent) 
        : TemplateIodeObjectWidget<M, V>(iodeType, parent)
#endif
    {
#ifdef _GSAMPLE_
        this->setGeometry(QRect(10, 50, 700, 250));

        vLayout = new QVBoxLayout(this);
        vLayout->setContentsMargins(0, 0, 0, 0);
        vLayout->setObjectName("vTabLayout");

        tableview = new GSampleNumericalTableView(this);
        objmodel = new GSampleTableModel(refTable, gsample, nbDecimals, variables, tableview);
        tableview->setModel(objmodel);
        tableview->horizontalHeader()->setStretchLastSection(true);

        QString stylesheet = "QHeaderView::section { background-color: lightGray; font: bold; border: 0.5px solid }";
        tableview->setStyleSheet(stylesheet);

        // -1 -> span over all rows/columns
        vLayout->addWidget(tableview);
#endif
        bottomLayout = new QHBoxLayout(this);
        bottomLayout->setObjectName(QString::fromUtf8("bottom_layout"));

        // nb decimals spinbox
        label_precision = new QLabel("Precision");
        label_precision->setObjectName(QString::fromUtf8("label_precision"));
        QSizePolicy sizePolicyLabel(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicyLabel.setHorizontalStretch(0);
        sizePolicyLabel.setVerticalStretch(0);
        sizePolicyLabel.setHeightForWidth(label_precision->sizePolicy().hasHeightForWidth());
        label_precision->setSizePolicy(sizePolicyLabel);
        bottomLayout->addWidget(label_precision, Qt::AlignLeft);

        spinBox_precision = new QSpinBox();
        spinBox_precision->setMinimum(1);
        spinBox_precision->setMaximum(MAX_PRECISION_NUMBERS);
#ifdef _GSAMPLE_
        spinBox_precision->setValue(nbDecimals);
#else
        spinBox_precision->setValue(4);
#endif
        spinBox_precision->setObjectName(QString::fromUtf8("spinBox_precision"));
        QSizePolicy sizePolicySpinbox(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicySpinbox.setHorizontalStretch(0);
        sizePolicySpinbox.setVerticalStretch(0);
        sizePolicySpinbox.setHeightForWidth(spinBox_precision->sizePolicy().hasHeightForWidth());
        spinBox_precision->setSizePolicy(sizePolicySpinbox);
        spinBox_precision->setMinimumSize(QSize(40, 0));
        bottomLayout->addWidget(spinBox_precision, Qt::AlignLeft);

        // scientific notation checkbox
        checkbox_scientific = new QCheckBox("Scientific", this);
        checkbox_scientific->setChecked(false);
        bottomLayout->addWidget(checkbox_scientific, Qt::AlignLeft);

#ifdef _GSAMPLE_
        addHorizontalSpacer();
        vLayout->addLayout(bottomLayout);
#else
        this->vLayout->addLayout(bottomLayout);
#endif

        // shortcuts
        shortcutPrecisionPlus = new QShortcut(QKeySequence(Qt::Key_F4), this);
        shortcutPrecisionMinus = new QShortcut(QKeySequence(Qt::SHIFT | Qt::Key_F4), this);
        shortcutIncreaseColumnSize = new QShortcut(QKeySequence(Qt::Key_F3), this);
        shortcutDecreaseColumnSize = new QShortcut(QKeySequence(Qt::SHIFT | Qt::Key_F3), this);
        shortcutResizeToContents = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_F3), this);

        shortcutPrecisionPlus->setContext(Qt::WidgetWithChildrenShortcut);
        shortcutPrecisionMinus->setContext(Qt::WidgetWithChildrenShortcut);
        shortcutIncreaseColumnSize->setContext(Qt::WidgetWithChildrenShortcut);
        shortcutDecreaseColumnSize->setContext(Qt::WidgetWithChildrenShortcut);
        shortcutResizeToContents->setContext(Qt::WidgetWithChildrenShortcut);

        // connect
        QObject::connect(spinBox_precision, &QSpinBox::valueChanged, this, &_NUMERICAL_WIDGET_CLASS_NAME_::updatePrecision);
        QObject::connect(checkbox_scientific, &QCheckBox::stateChanged, this, &_NUMERICAL_WIDGET_CLASS_NAME_::formatNumber);
        QObject::connect(shortcutPrecisionPlus, &QShortcut::activated, this, [this](){ this->spinBox_precision->stepUp(); });
        QObject::connect(shortcutPrecisionMinus, &QShortcut::activated, this, [this](){ this->spinBox_precision->stepDown(); });
        QObject::connect(shortcutIncreaseColumnSize, &QShortcut::activated, this, &_NUMERICAL_WIDGET_CLASS_NAME_::increaseColumnSize);
        QObject::connect(shortcutDecreaseColumnSize, &QShortcut::activated, this, &_NUMERICAL_WIDGET_CLASS_NAME_::decreaseColumnSize);
        QObject::connect(shortcutResizeToContents, &QShortcut::activated, this, [this](){ this->tableview->resizeColumnsToContents(); });

#ifdef _GSAMPLE_
        loadSettings();

        this->setModal(true);
        if(variables.isEmpty())
            this->setWindowTitle("Table " + refTable);
        else
            this->setWindowTitle("Table of series");
#endif
    }

    ~_NUMERICAL_WIDGET_CLASS_NAME_()
    {
        delete label_precision;
        delete spinBox_precision;
        delete checkbox_scientific;
        delete shortcutPrecisionPlus;
        delete shortcutPrecisionMinus;
        delete shortcutIncreaseColumnSize;
        delete shortcutDecreaseColumnSize;
        delete shortcutResizeToContents;
#ifdef _GSAMPLE_
        delete objmodel;
        delete tableview;
#endif
    }

    void addHorizontalSpacer() 
    {
        QSpacerItem* horizontalSpacer = new QSpacerItem(800, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        // -1 -> span over all rows/columns
        bottomLayout->addSpacerItem(horizontalSpacer);
    }

    void increaseColumnSize()
    {
        QHeaderView* columns = this->tableview->horizontalHeader();
        const int max_size = columns->maximumSectionSize();
        for(int j=0; j < columns->count(); j++)
            columns->resizeSection(j, qMin(columns->sectionSize(j) + COLUMN_SIZE_STEP, max_size));
    }

    void decreaseColumnSize()
    {
        QHeaderView* columns = this->tableview->horizontalHeader();
        const int min_size = columns->minimumSectionSize();
        for(int j=0; j < columns->count(); j++)
            columns->resizeSection(j, qMax(columns->sectionSize(j) - COLUMN_SIZE_STEP, min_size));
    }

    void updatePrecision(const int value)
    {
        this->objmodel->set_precision(value);
        this->objmodel->reset();
    }

    void formatNumber(int state)
    {
        char format = (state == Qt::Checked) ? 'e' : 'f'; 
        this->objmodel->set_format(format);
        spinBox_precision->setMinimum(this->objmodel->get_min_precision());
        this->objmodel->reset();
    }

    void loadNumericSettings(const QSettings* project_settings)
    {
        if(!project_settings)
            return;
        
        bool checked = project_settings->value("scientific", false).toBool();
        checkbox_scientific->setChecked(checked);
        formatNumber(checkbox_scientific->checkState());
        int nb_digits = project_settings->value("precision", spinBox_precision->value()).toInt();
        spinBox_precision->setValue(nb_digits);
    }

    void saveNumericSettings(QSettings* project_settings)
    {
        if(!project_settings)
            return;
        
        project_settings->setValue("precision", spinBox_precision->value());
        project_settings->setValue("scientific", checkbox_scientific->isChecked());
    }

#ifdef _GSAMPLE_
public slots:
    void loadSettings()
    {
        QSettings* project_settings = ProjectSettings::getProjectSettings();
        if(!project_settings)
            return;

        project_settings->beginGroup(getGroupName());
        loadNumericSettings(project_settings);
        project_settings->endGroup();
    }

    void saveSettings()
    {
        QSettings* project_settings = ProjectSettings::getProjectSettings();
        if(!project_settings)
            return;

        project_settings->beginGroup(getGroupName());
        saveNumericSettings(project_settings);
        project_settings->endGroup();
    }
#endif
};
