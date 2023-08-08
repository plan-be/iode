#include "main_window_abstract.h"


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
