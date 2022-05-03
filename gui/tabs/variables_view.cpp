#include "variables_view.h"


void VariablesView::new_variable()
{
	// check variables sample and ask to set it if not already defined
	KDBVariables kdb;
	if (kdb.get_nb_periods() == 0)
	{
		QWidget* p = static_cast<QWidget*>(parent());
		QMessageBox::StandardButton reply = QMessageBox::question(p, "Sample", "Sample undefined. Set it?");
		if (reply == QMessageBox::Yes)
		{
			QIodeEditVarsSample dialog(this);
			dialog.exec();
		}
		else
		{
			return;
		}
	}

	QIodeAddVariable dialog(this);
	dialog.exec();
	update();
}
