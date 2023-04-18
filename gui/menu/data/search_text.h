#pragma once

#include <QWidget>

#include "ui_search_text.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"


class QIodeMenuDataSearchText : public QIodeSettings, public Ui::QIodeMenuDataSearchText
{
    Q_OBJECT

    WrapperQLineEdit* wText;
    WrapperQLineEdit* wSaveList;
    WrapperComboBox*  wComboIodeTypes;
    WrapperCheckBox*  wWholeWord;
    WrapperCheckBox*  wExactCase;
    WrapperCheckBox*  wSearchInNames;
    WrapperCheckBox*  wSearchInFormulas;
    WrapperCheckBox*  wSearchInFreeText;
    WrapperQTextEdit* wResults;

public:
	QIodeMenuDataSearchText(const QString& project_settings_filepath, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~QIodeMenuDataSearchText();

public slots:
    void search();
    void help();
};
