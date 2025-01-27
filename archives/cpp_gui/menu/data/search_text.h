#pragma once

#include <QWidget>

#include "ui_search_text.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"


class MenuDataSearchText : public IodeSettingsDialog, public Ui::MenuDataSearchText
{
    Q_OBJECT

    WrapperQLineEdit* wText;
    WrapperQLineEdit* wSaveList;
    WrapperComboBox*  wComboIodeType;
    WrapperCheckBox*  wWholeWord;
    WrapperCheckBox*  wExactCase;
    WrapperCheckBox*  wSearchInNames;
    WrapperCheckBox*  wSearchInFormulas;
    WrapperCheckBox*  wSearchInFreeText;
    WrapperQTextEdit* wResults;

public:
	MenuDataSearchText(QWidget* parent = nullptr);
    ~MenuDataSearchText();

public slots:
    void search();
    void help();
};
