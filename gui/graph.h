#pragma once

#include <QWidget>
#include <QDialog>
#include <QVector>

#include "utils.h"
#include "wrapper_classes.h"
#include "ui_graph.h"


class IODEGraphDialog : public MixinSettings
{
    Q_OBJECT

private:
    Ui::IODEGraph* ui;

public:
    IODEGraphDialog(QSettings& settings, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~IODEGraphDialog();

private:
    void plot();
};
