#include "main_window_plot.h"


void MainWindowPlot::appendModalDialog(QDialog* dialog)
{
    dialog->open();
    dialogs.append(dialog);
}
