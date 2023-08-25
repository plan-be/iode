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
 *        QWidget or QIodeObjectWidget.
 * 
 * 3. Do NOT forget to add safeguards when including the present hearder!
 */

#include <QLabel>
#include <QDialog>
#include <QSpinBox>
#include <QShortcut>
#include <QSpacerItem>
#include <QGridLayout>
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
template <class M, class V> class _NUMERICAL_WIDGET_CLASS_NAME_ : public QIodeObjectWidget<M, V>
#endif
{
#ifdef _GSAMPLE_
    int row;
    QGridLayout* gridLayout;

    GSampleNumericalTableView* tableview;
    GSampleTableModel* objmodel;
#endif

    QLabel* label_nbDigits;
    QSpinBox* spinBox_nbDigits;

    QShortcut* shortcutNbDecPlus;
    QShortcut* shortcutNbDecMinus;

private:
    QString getGroupName()
    {
#ifdef _GSAMPLE_
        return "GSampleTable";
#else
        return QString::fromStdString(vIodeTypes[this->iodeType]).toUpper();
#endif
    }

public:
#ifdef _GSAMPLE_
    _NUMERICAL_WIDGET_CLASS_NAME_(const QString& refTable, const QString& gsample, const int nbDecimals, 
        const QString& variables, QWidget* parent = nullptr): 
        QDialog(parent, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint)
#else
    _NUMERICAL_WIDGET_CLASS_NAME_(EnumIodeType iodeType, QIodeAbstractTabWidget* parent) 
        : QIodeObjectWidget<M, V>(iodeType, parent)
#endif
    {
#ifdef _GSAMPLE_
        row = 0;

        this->setGeometry(QRect(10, 50, 700, 250));

        gridLayout = new QGridLayout(this);
        gridLayout->setObjectName("gridLayoutGSampleTable");

        tableview = new GSampleNumericalTableView(this);
        objmodel = new GSampleTableModel(refTable, gsample, nbDecimals, variables, tableview);
        tableview->setModel(objmodel);
        tableview->horizontalHeader()->setStretchLastSection(true);

        QString stylesheet = "QHeaderView::section { background-color: lightGray; font: bold; border: 0.5px solid }";
        tableview->setStyleSheet(stylesheet);

        // -1 -> span over all rows/columns
        gridLayout->addWidget(tableview, row, 0, 1, -1);
        row++;

#define _GRIDLAYOUT_ gridLayout
#else
#define _GRIDLAYOUT_ this->gridLayout
#endif

        // nb decimals spinbox
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);

        label_nbDigits = new QLabel("Nb Significant Digits ");
        label_nbDigits->setObjectName(QString::fromUtf8("label_nbDigits"));
        label_nbDigits->setSizePolicy(sizePolicy);
        _GRIDLAYOUT_->addWidget(label_nbDigits, this->row, 0, Qt::AlignLeft);

        spinBox_nbDigits = new QSpinBox();
        spinBox_nbDigits->setMinimum(1);
        spinBox_nbDigits->setMaximum(MAX_NB_DIGITS_TABLE);
#ifdef _GSAMPLE_
        spinBox_nbDigits->setValue(nbDecimals);
#else
        spinBox_nbDigits->setValue(4);
#endif
        spinBox_nbDigits->setObjectName(QString::fromUtf8("spinBox_nbDigits"));
        spinBox_nbDigits->setSizePolicy(sizePolicy);
        spinBox_nbDigits->setFixedWidth(40);
        _GRIDLAYOUT_->addWidget(spinBox_nbDigits, this->row, 1, Qt::AlignLeft);

        // spacers
        QSpacerItem* horizontalSpacer = new QSpacerItem(600, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        // -1 -> span over all rows/columns
        _GRIDLAYOUT_->addItem(horizontalSpacer, this->row, 2, 1, -1);

#undef _GRIDLAYOUT_

        // shortcuts
        shortcutNbDecPlus = new QShortcut(QKeySequence(Qt::Key_F4), this);
        shortcutNbDecMinus = new QShortcut(QKeySequence(Qt::SHIFT | Qt::Key_F4), this);

        shortcutNbDecPlus->setContext(Qt::WidgetWithChildrenShortcut);
        shortcutNbDecMinus->setContext(Qt::WidgetWithChildrenShortcut);

        // connect
        QObject::connect(spinBox_nbDigits, &QSpinBox::valueChanged, this, &_NUMERICAL_WIDGET_CLASS_NAME_::setNbDigits);
        QObject::connect(shortcutNbDecPlus, &QShortcut::activated, this, [this](){ this->spinBox_nbDigits->stepUp(); });
        QObject::connect(shortcutNbDecMinus, &QShortcut::activated, this, [this](){ this->spinBox_nbDigits->stepDown(); });
        QObject::connect(spinBox_nbDigits, &QSpinBox::valueChanged, this, &_NUMERICAL_WIDGET_CLASS_NAME_::saveSettings);

        // reload nb decimals
        loadSetting();

#ifdef _GSAMPLE_
        this->setModal(true);
        if(variables.isEmpty())
            this->setWindowTitle(refTable);
        else
            this->setWindowTitle("Table of series");
#endif
    }

    ~_NUMERICAL_WIDGET_CLASS_NAME_()
    {
        delete label_nbDigits;
        delete spinBox_nbDigits;
        delete shortcutNbDecPlus;
        delete shortcutNbDecMinus;
#ifdef _GSAMPLE_
        delete objmodel;
        delete tableview;
#endif
    }

    void setNbDigits(const int value)
    {
        this->objmodel->set_nb_digits(value);
        this->objmodel->reset();
    }

#ifndef _GSAMPLE_
    void setProjectDir(const QDir& projectDir) override
    {
        QIodeObjectWidget<M, V>::setProjectDir(projectDir);
        loadSetting();
    }
#endif

    void loadSetting()
    {
        QSettings* project_settings = QIodeProjectSettings::getProjectSettings();
        if(!project_settings)
        {
            spinBox_nbDigits->setValue(4);
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
