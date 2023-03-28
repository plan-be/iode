#include "main_window_plot.h"


MainWindowPlot::~MainWindowPlot()
{
    foreach(QIodePlotDialog* plotDialog, plots) plotDialog->close();
    plots.clear();
}

void MainWindowPlot::appendPlot(QIodePlotDialog* plotDialog)
{
    plotDialog->open();
    plots.append(plotDialog);
}
