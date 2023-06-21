#pragma once

#include <QWidget>

#include "ui_list_calculus.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"
#include "custom_widgets/complete_line_edit.h"


/**
 * @brief 
 *        Operators:
 *         + = union
 *         - = difference
 *         * = intersection
 *         x = product: ["A", "B"] x ["C", "D"] => ["AB", "AC", "BC", "BD"]
 * 
 */
class QIodeMenuDataListCalculus : public QIodeSettings, public Ui::QIodeMenuDataListCalculus
{
    Q_OBJECT

    WrapperQLineEdit* wList1;
    WrapperQLineEdit* wList2;
    WrapperQLineEdit* wListRes;
    WrapperComboBox*  wComboOperator;
    WrapperQTextEdit* wResults;

    QIodeCompleter* completer;

public:
	QIodeMenuDataListCalculus(QWidget* parent = nullptr);
    ~QIodeMenuDataListCalculus();

public slots:
    void calculus();
    void help();
};