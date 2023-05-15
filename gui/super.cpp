#pragma once
#include "super.h"


void gui_error_super(int level, char* msg)
{
	QMessageBox::critical(nullptr, "ERROR", QString(msg));
	if (level > 0)
	{
		QMainWindow* main_window = static_cast<QMainWindow*>(get_main_window_ptr());
		main_window->close();
		exit(level);
	}
}

void gui_warning_super(char* msg)
{
	QMessageBox::warning(nullptr, "WARNING", QString(msg));
}

void gui_msg_super(char* msg)
{
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	main_window->display_output(QString(msg));
}

int gui_confirm_super(char* msg)
{
	QMessageBox::StandardButton answer = QMessageBox::question(nullptr, "CONFIRM", QString(msg));
	if (answer == QMessageBox::Ok) return 1;
	else return 0;
}

void gui_panic_super()
{
	QString msg("Attempting to save Data saved in ws.*files\n");
	msg += "To restart iode with the last data, type :\n";
	msg += "    iode -ws";
	QMessageBox::critical(nullptr, "SWAP PANIC", msg);
	K_end_ws(1);
	QMainWindow* main_window = static_cast<QMainWindow*>(get_main_window_ptr());
	main_window->close();
	exit(2);
}

void gui_assign_super_API()
{
	IODE_assign_super_API();
	kerror_super = gui_error_super;
	kwarning_super = gui_warning_super;
	kmsg_super = gui_msg_super;
	kconfirm_super = gui_confirm_super;
	kpanic_super = gui_panic_super;
}
