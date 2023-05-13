#pragma once

#include <QWidget>

#include "ui_duplicate.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"
#include "custom_widgets/complete_line_edit.h"


class QIodeMenuDataDuplicateObj : public QIodeSettings, public Ui::QIodeMenuDataDuplicateObj
{
    Q_OBJECT

    WrapperComboBox*    wComboIodeTypes;
    WrapperQLineEdit*   wObjName;
    WrapperQLineEdit*   wDupObjName;

    QIodeCompleter* completer;
    
public:
	QIodeMenuDataDuplicateObj(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~QIodeMenuDataDuplicateObj();

public slots:
    void duplicate();
    void help();
};
