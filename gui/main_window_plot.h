#pragma once
#include <QWidget>
#include <QMainWindow>

#include "utils.h"
#include "plot/plot.h"


class MainWindowPlot: public QMainWindow
{
	Q_OBJECT

	QVector<QIodePlotDialog*> plots;

public:
    MainWindowPlot(QWidget *parent = Q_NULLPTR) : QMainWindow(parent) {}
    ~MainWindowPlot();

public slots:
	void appendPlot(QIodePlotDialog* plotDialog);
	virtual void open_graphs_variables_dialog_from_vars_view(
		const QList<QString>& variableNames, const QString& from, const QString& to) = 0;
	virtual void open_compute_identities_dialog() = 0;
};
