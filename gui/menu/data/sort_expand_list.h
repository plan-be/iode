#pragma once

#include <QWidget>

#include "ui_sort_expand_list.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"
#include "custom_widgets/complete_line_edit.h"


class QIodeMenuDataSortExpandList : public QIodeSettings, public Ui::QIodeMenuDataSortExpandList
{
    Q_OBJECT

    WrapperQLineEdit* wLineSort;
    WrapperQLineEdit* wLineSave;

    QIodeCompleter* completer;

public:
	QIodeMenuDataSortExpandList(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~QIodeMenuDataSortExpandList();

public slots:
    void sort();
    void help();
};
