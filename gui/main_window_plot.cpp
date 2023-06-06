#include "main_window_plot.h"


MainWindowPlot::~MainWindowPlot()
{
    foreach(QDialog* dialog, dialogs) dialog->close();
    dialogs.clear();
}

void MainWindowPlot::appendModalDialog(QDialog* dialog)
{
    dialog->open();
    dialogs.append(dialog);
}
