#include "main_window_plot.h"


void MainWindowPlot::appendDialog(QDialog* dialog)
{
    dialog->open();
    dialogs.append(dialog);
}

void MainWindowPlot::appendPlot(QIodePlotDialog* dialog)
{
    dialog->plot();
    appendDialog(dialog);
}
