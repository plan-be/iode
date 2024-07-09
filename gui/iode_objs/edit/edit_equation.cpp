#include "edit_equation.h"


EditEquationDialog::EditEquationDialog(const QString& equationName, KDBEquations* database, 
	QWidget* parent) : IodeSettingsDialog(parent, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint), 
	database(database)
{
	setupUi(this);

	MainWindowAbstract* main_window = static_cast<MainWindowAbstract*>(get_main_window_ptr());
	main_window->computeHash(true);

	completer = new IodeCompleter(false, false, {SCALARS, VARIABLES}, textEdit_lec);
	textEdit_lec->setCompleter(completer);
	textEdit_lec->setLineWrapMode(QPlainTextEdit::LineWrapMode::WidgetWidth);

    fullScreenShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_X), this);
    fullScreenShortcut->setContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);

    connect(fullScreenShortcut, &QShortcut::activated, this, &EditEquationDialog::showMaximized);

	int i = 0;
	QList<QString> list_methods;
	for (const std::string& method : v_eq_methods) list_methods << QString::fromStdString(method);

	lineName = new WrapperIodeNameEdit(label_name->text(), *lineEdit_name, EQUATIONS, REQUIRED_FIELD);
	comboBoxMethod = new WrapperComboBox(label_method->text(), *comboBox_method, OPTIONAL_FIELD, list_methods);
	sampleFrom = new WrapperSampleEdit(label_from->text(), *sampleEdit_from, OPTIONAL_FIELD);
	sampleTo = new WrapperSampleEdit(label_to->text(), *sampleEdit_to, OPTIONAL_FIELD);
	lineLec = new WrapperQPlainTextEdit(label_lec->text(), *textEdit_lec, REQUIRED_FIELD);
	lineComment = new WrapperQLineEdit(label_comment->text(), *lineEdit_comment, OPTIONAL_FIELD);
	lineBlock = new WrapperQLineEdit(label_block->text(), *lineEdit_block, OPTIONAL_FIELD);
	lineInstruments = new WrapperQLineEdit(label_instruments->text(), *lineEdit_instruments, OPTIONAL_FIELD);

	std::string equation_name = equationName.toStdString();
	if(!equation_name.empty() && Equations.contains(equation_name))
	{
		// --- existing equation ---
		className = "TAB_EDIT_EQUATION";
		lineEdit_name->setReadOnly(true);
		
		Equation eq(equation_name);
		display_equation(eq);

		comboBoxMethod->setQValue(eq.get_method_as_int());
		Sample sample = eq.get_sample();
		sampleFrom->setQValue(QString::fromStdString(sample.start_period().to_string()));
		sampleTo->setQValue(QString::fromStdString(sample.end_period().to_string()));
		lineBlock->setQValue(QString::fromStdString(eq.get_block()));
		lineInstruments->setQValue(QString::fromStdString(eq.get_instruments()));
		
		update_list_equations_to_estimate();
	}
	else
	{
		// --- new equation ---
		className = "TAB_ADD_EQUATION";
		lineEdit_name->setReadOnly(false);
	}
}

EditEquationDialog::~EditEquationDialog() 
{
	delete lineName;
	delete comboBoxMethod;
	delete sampleFrom;
	delete sampleTo;
	delete lineLec;
	delete lineComment;
	delete lineBlock;
	delete lineInstruments;

	delete completer;

	delete fullScreenShortcut;
}

// same as ODE_blk_save_cur() from o_est.c from the old GUI
void EditEquationDialog::save_current_equation()
{
	std::string lec = lineLec->extractAndVerify().toStdString();
	std::string comment = lineComment->extractAndVerify().toStdString();
	edit_est_eqs.update_current_equation(lec, comment);
}

// same as ODE_blk_check() from o_est.c from the old GUI
void EditEquationDialog::update_list_equations_to_estimate()
{
	std::string current_eq_name = lineName->extractAndVerify().toStdString();
	if(current_eq_name.empty())
		throw std::invalid_argument("Empty equation name");

	// update block
	std::string block = lineBlock->extractAndVerify().toStdString();
	if(block.empty()) 
		block = lineName->extractAndVerify().toStdString();
	edit_est_eqs.set_block(block, current_eq_name);

	// the list of equations to estimate (block) may have been changed
	// See EditAndEstimateEquations::set_block()
	block = edit_est_eqs.get_block();
	lineBlock->setQValue(QString::fromStdString(block));
}

// same as ODE_blk_cur() from o_est.c from the old GUI
void EditEquationDialog::display_equation(const Equation& eq)
{
	lineName->setQValue(QString::fromStdString(eq.get_endo()));

	// editable values
	lineLec->setQValue(QString::fromStdString(eq.get_lec()));
	lineComment->setQValue(QString::fromStdString(eq.get_comment()));

	// read-only values
	std::array<float, EQS_NBTESTS> tests = eq.get_tests();
	lineEdit_tests_r2adj->setText(QString::number(tests[EQ_R2ADJ], 'g', 3));
	lineEdit_tests_durbw->setText(QString::number(tests[EQ_DW], 'g', 3));
	lineEdit_tests_fstat->setText(QString::number(tests[EQ_FSTAT], 'g', 3));
	lineEdit_tests_loglk->setText(QString::number(tests[EQ_LOGLIK], 'g', 3));
}

// same as ODE_blk_save_fn() from o_est.c from old GUI
void EditEquationDialog::edit()
{
	try
	{
		save_current_equation();
		update_list_equations_to_estimate();

		if(!edit_est_eqs.is_estimation_done())
		{
			edit_est_eqs.update_scalars();

			std::string instruments = lineInstruments->extractAndVerify().toStdString();
			edit_est_eqs.set_instruments(instruments);
		}

		size_t hashBefore = hash_value(Equations);

		std::string from = sampleFrom->extractAndVerify().toStdString();
		std::string to = sampleTo->extractAndVerify().toStdString();

		std::vector<std::string> v_new_eqs = edit_est_eqs.save(from, to);
		for(const std::string& name: v_new_eqs)
			emit newObjectInserted(QString::fromStdString(name));

		size_t hashAfter = hash_value(Equations);
		if(hashAfter != hashBefore)
			emit databaseModified();
		
		this->accept();
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
	}
}

// same as ODE_blk_coef_fn() from o_est.c from the old GUI
void EditEquationDialog::display_coefs()
{
	try
	{
		save_current_equation();
		update_list_equations_to_estimate();

		edit_est_eqs.update_scalars();
		EstimationCoefsDialog dialog(&edit_est_eqs, this);
		dialog.exec();
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
	}	
}

// same as ODE_blk_est_fn() from o_est.c from the old GUI
void EditEquationDialog::estimate()
{
	try
	{
		save_current_equation();

		// set method
		int i_method = comboBoxMethod->extractAndVerify();
		if(i_method < 0)
			throw std::invalid_argument("Invalid method. Please choose a method in the drop-down list");
		edit_est_eqs.set_method((IodeEquationMethod) i_method);

		// set sample
		std::string from = sampleFrom->extractAndVerify().toStdString();
		if(from.empty())
			throw std::invalid_argument("Starting period for the sample is empty");

		std::string to = sampleTo->extractAndVerify().toStdString();
		if(to.empty())
			throw std::invalid_argument("Ending period for the sample is empty");
		
		edit_est_eqs.set_sample(from, to);

		// set instruments
		std::string instruments = lineInstruments->extractAndVerify().toStdString();
		edit_est_eqs.set_instruments(instruments);

		// update list of equations to estimate 
		update_list_equations_to_estimate();

		// update list of coefficients 
		edit_est_eqs.update_scalars();

		// process estimation
		edit_est_eqs.estimate();

		// copy the resulting values for the tests for all equations
		edit_est_eqs.copy_eq_tests_values();

		// refresh the values for the tests of the current equation 
		Equation current_eq = edit_est_eqs.current_equation();
		display_equation(current_eq);
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
	}
}

// same as ODE_blk_next_fn() from o_est.c from old GUI
void EditEquationDialog::next()
{
	try
	{
		save_current_equation();
		update_list_equations_to_estimate();

		Equation equation = edit_est_eqs.next_equation();
		display_equation(equation);
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
	}
}

void EditEquationDialog::dynamic_adjustment()
{
	QString lec = lineLec->extractAndVerify();
	DynAdjustmentDialog dialog(lec, this);
	dialog.exec();
	lineLec->setQValue(lec);
}

// same as ODE_blk_res_fn() function from o_est.c from the old GUI
void EditEquationDialog::results()
{
	if(edit_est_eqs.is_estimation_done())
	{
		try
		{
			EstimationResultsDialog dialog(&edit_est_eqs, this);
			dialog.exec();
		}
		catch (const std::exception& e)
		{
			QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
		}
	}
	else
	{
		QMessageBox::warning(nullptr, "WARNING", "No estimation has been done yet");
	}
}

void EditEquationDialog::unit_root()
{
	UnitRootDialog dialog(this);
	dialog.exec();
}

void EditEquationDialog::help()
{
	QDesktopServices::openUrl(url_manual);
}