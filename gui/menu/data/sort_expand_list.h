#pragma once

#include <QWidget>

#include "ui_sort_expand_list.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"
#include "text_edit/complete_line_edit.h"


class MenuDataSortExpandList : public IodeSettingsDialog, public Ui::MenuDataSortExpandList
{
    Q_OBJECT

    WrapperQLineEdit* wLineSort;
    WrapperQLineEdit* wLineSave;

    IodeCompleter* completer;

public:
	MenuDataSortExpandList(QWidget* parent = nullptr);
    ~MenuDataSortExpandList();

public slots:
    void sort();
    void help();
};
