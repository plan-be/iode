#pragma once

#include <QWidget>

#include "ui_sort_expand_list.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"


class QIodeMenuDataSortExpandList : public QIodeSettings, public Ui::QIodeMenuDataSortExpandList
{
    Q_OBJECT

    WrapperQLineEdit* wLineSort;
    WrapperQLineEdit* wLineSave;

public:
	QIodeMenuDataSortExpandList(const QString& project_settings_filepath, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~QIodeMenuDataSortExpandList();

public slots:
    void sort();
    void help();
};
