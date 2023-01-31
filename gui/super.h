#pragma once
#include "utils.h"
#include "main_window.h"
#include <QString>
#include <QMessageBox>
#include <QMainWindow>
#include <QStatusBar>


void gui_error_super(int level, char* msg);

void gui_warning_super(char* msg);

void gui_msg_super(char* msg);

int gui_confirm_super(char* msg);

void gui_panic_super();

void gui_assign_super_API();
