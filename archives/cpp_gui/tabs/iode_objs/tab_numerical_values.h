#pragma once

#include <QLabel>
#include <QObject>
#include <QSpinBox>
#include <QCheckBox>
#include <QShortcut>
#include <QSpacerItem>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QAbstractTableModel>

#include "utils.h"
#include "settings.h"
#include "iode_objs/models/numerical_table_model.h"


/**
 * @brief Defines a widget containing a Scalar or Variable table 
 *        and a QSpinBox indicating the number of significant 
 *        digits to display.
 */
class NumericalWidget : public QObject
{
    Q_OBJECT

    static const int COLUMN_SIZE_STEP = 10;

    NumericalTableModel* tableModel;
    QTableView* tableView;

    QLabel*    label_precision;
    QSpinBox*  spinBox_precision;
    QCheckBox* checkbox_scientific;

    QShortcut* shortcutPrecisionPlus;
    QShortcut* shortcutPrecisionMinus;
    QShortcut* shortcutIncreaseColumnSize;
    QShortcut* shortcutDecreaseColumnSize;
    QShortcut* shortcutResizeToContents;

    QHBoxLayout* bottomLayout;

public:
    NumericalWidget() {}

    ~NumericalWidget()
    {
        delete label_precision;
        delete spinBox_precision;
        delete checkbox_scientific;
        delete shortcutPrecisionPlus;
        delete shortcutPrecisionMinus;
        delete shortcutIncreaseColumnSize;
        delete shortcutDecreaseColumnSize;
        delete shortcutResizeToContents;
    }

    QHBoxLayout* setup(QWidget* owner, NumericalTableModel* tableModel, QTableView* tableView)
    {
        this->tableModel = tableModel;
        this->tableView  = tableView;

        bottomLayout = new QHBoxLayout(owner);
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
        spinBox_precision->setValue(4);
        spinBox_precision->setObjectName(QString::fromUtf8("spinBox_precision"));
        QSizePolicy sizePolicySpinbox(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicySpinbox.setHorizontalStretch(0);
        sizePolicySpinbox.setVerticalStretch(0);
        sizePolicySpinbox.setHeightForWidth(spinBox_precision->sizePolicy().hasHeightForWidth());
        spinBox_precision->setSizePolicy(sizePolicySpinbox);
        spinBox_precision->setMinimumSize(QSize(40, 0));
        bottomLayout->addWidget(spinBox_precision, Qt::AlignLeft);

        // scientific notation checkbox
        checkbox_scientific = new QCheckBox("Scientific");
        checkbox_scientific->setChecked(false);
        bottomLayout->addWidget(checkbox_scientific, Qt::AlignLeft);

        // shortcuts
        shortcutPrecisionPlus = new QShortcut(QKeySequence(Qt::Key_F4), owner);
        shortcutPrecisionMinus = new QShortcut(QKeySequence(Qt::SHIFT | Qt::Key_F4), owner);
        shortcutIncreaseColumnSize = new QShortcut(QKeySequence(Qt::Key_F3), owner);
        shortcutDecreaseColumnSize = new QShortcut(QKeySequence(Qt::SHIFT | Qt::Key_F3), owner);
        shortcutResizeToContents = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_F3), owner);

        shortcutPrecisionPlus->setContext(Qt::WidgetWithChildrenShortcut);
        shortcutPrecisionMinus->setContext(Qt::WidgetWithChildrenShortcut);
        shortcutIncreaseColumnSize->setContext(Qt::WidgetWithChildrenShortcut);
        shortcutDecreaseColumnSize->setContext(Qt::WidgetWithChildrenShortcut);
        shortcutResizeToContents->setContext(Qt::WidgetWithChildrenShortcut);

        // connect
        connect(spinBox_precision, &QSpinBox::valueChanged, this, &NumericalWidget::updatePrecision);
        connect(checkbox_scientific, &QCheckBox::stateChanged, this, &NumericalWidget::formatNumber);
        connect(shortcutPrecisionPlus, &QShortcut::activated, this, [this](){ spinBox_precision->stepUp(); });
        connect(shortcutPrecisionMinus, &QShortcut::activated, this, [this](){ spinBox_precision->stepDown(); });
        connect(shortcutIncreaseColumnSize, &QShortcut::activated, this, &NumericalWidget::increaseColumnSize);
        connect(shortcutDecreaseColumnSize, &QShortcut::activated, this, &NumericalWidget::decreaseColumnSize);
        connect(shortcutResizeToContents, &QShortcut::activated, this, [tableView](){ tableView->resizeColumnsToContents(); });

        return bottomLayout;
    }

    void increaseColumnSize()
    {
        QHeaderView* columns = tableView->horizontalHeader();
        const int max_size = columns->maximumSectionSize();
        for(int j=0; j < columns->count(); j++)
            columns->resizeSection(j, qMin(columns->sectionSize(j) + COLUMN_SIZE_STEP, max_size));
    }

    void decreaseColumnSize()
    {
        QHeaderView* columns = tableView->horizontalHeader();
        const int min_size = columns->minimumSectionSize();
        for(int j=0; j < columns->count(); j++)
            columns->resizeSection(j, qMax(columns->sectionSize(j) - COLUMN_SIZE_STEP, min_size));
    }

    void updatePrecision(const int value)
    {
        tableModel->set_precision(value);
    }

    void formatNumber(int state)
    {
        char format = (state == Qt::Checked) ? 'e' : 'f'; 
        tableModel->set_format(format);
        spinBox_precision->setMinimum(tableModel->get_min_precision());
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
};
