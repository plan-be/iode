#include "main_window_abstract.h"


void MainWindowAbstract::appendDialog(QDialog* dialog)
{
    dialog->open();
    dialogs.append(dialog);
}

void MainWindowAbstract::appendPlot(PlotDialog* dialog)
{
    dialog->plot();
    appendDialog(dialog);
}
