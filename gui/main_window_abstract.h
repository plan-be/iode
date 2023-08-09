#pragma once
#include <QWidget>
#include <QMainWindow>

#include "utils.h"
#include "plot/plot.h"
#include "custom_widgets/completer.h"


class MainWindowAbstract: public QMainWindow
{
	Q_OBJECT

protected:
	QIodeCompleter* completer;

	QVector<QDialog*> dialogs;

public:
    MainWindowAbstract(QWidget *parent = Q_NULLPTR) : QMainWindow(parent) {}

	QIodeCompleter* getCompleter()
	{
		return completer;
	}

	virtual void computeHash(const bool value) = 0;

	virtual void update_tab_and_completer(const int iodeType = -1) = 0;

public slots:
	void appendDialog(QDialog* dialog);
	void appendPlot(QIodePlotDialog* dialog);
	virtual void open_graphs_variables_dialog_from_vars_view(
		const QList<QString>& variableNames, const QString& from, const QString& to) = 0;
	virtual void open_compute_identities_dialog() = 0;
};
