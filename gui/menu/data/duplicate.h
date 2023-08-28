#pragma once

#include <QWidget>

#include "ui_duplicate.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"
#include "text_edit/complete_line_edit.h"


class MenuDataDuplicateObj : public QIodeSettings, public Ui::MenuDataDuplicateObj
{
    Q_OBJECT

    WrapperComboBox*    wComboIodeTypes;
    WrapperQLineEdit*   wObjName;
    WrapperQLineEdit*   wDupObjName;

    IodeCompleter* completer;
    
public:
	MenuDataDuplicateObj(QWidget* parent = nullptr);
    ~MenuDataDuplicateObj();

public slots:
    void duplicate();
    void help();
};
