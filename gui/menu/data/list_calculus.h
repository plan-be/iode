#pragma once

#include <QWidget>

#include "ui_list_calculus.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"
#include "text_edit/complete_line_edit.h"


/**
 * @brief 
 *        Operators:
 *         + = union
 *         - = difference
 *         * = intersection
 *         x = product: ["A", "B"] x ["C", "D"] => ["AB", "AC", "BC", "BD"]
 * 
 */
class MenuDataListCalculus : public IodeSettingsDialog, public Ui::MenuDataListCalculus
{
    Q_OBJECT

    WrapperQLineEdit* wList1;
    WrapperQLineEdit* wList2;
    WrapperQLineEdit* wListRes;
    WrapperComboBox*  wComboOperator;
    WrapperQTextEdit* wResults;

    IodeCompleter* completer;

public:
	MenuDataListCalculus(QWidget* parent = nullptr);
    ~MenuDataListCalculus();

public slots:
    void calculus();
    void help();
};