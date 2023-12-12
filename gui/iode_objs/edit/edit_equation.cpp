#include "edit_equation.h"


EditEquationDialog::EditEquationDialog(const QString& equationName, QWidget* parent) : 
	IodeSettingsDialog(parent, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint)
{
	setupUi(this);

	completer = new IodeCompleter(false, false, {I_SCALARS, I_VARIABLES}, textEdit_lec);
	textEdit_lec->setCompleter(completer);
	textEdit_lec->setLineWrapMode(QPlainTextEdit::LineWrapMode::WidgetWidth);

    fullScreenShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_X), this);
    fullScreenShortcut->setContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);

    connect(fullScreenShortcut, &QShortcut::activated, this, &EditEquationDialog::showMaximized);

	est_results = nullptr;

	int i = 0;
	QList<QString> list_methods;
	for (const std::string& method : vEquationMethods) list_methods << QString::fromStdString(method);

	lineName = new WrapperIodeNameEdit(label_name->text(), *lineEdit_name, I_EQUATIONS, REQUIRED_FIELD);
	comboBoxMethod = new WrapperComboBox(label_method->text(), *comboBox_method, OPTIONAL_FIELD, list_methods);
	sampleFrom = new WrapperSampleEdit(label_from->text(), *sampleEdit_from, OPTIONAL_FIELD);
	sampleTo = new WrapperSampleEdit(label_to->text(), *sampleEdit_to, OPTIONAL_FIELD);
	lineLec = new WrapperQPlainTextEdit(label_lec->text(), *textEdit_lec, REQUIRED_FIELD);
	lineComment = new WrapperQLineEdit(label_comment->text(), *lineEdit_comment, OPTIONAL_FIELD);
	lineBlock = new WrapperQLineEdit(label_block->text(), *lineEdit_block, OPTIONAL_FIELD);
	lineInstruments = new WrapperQLineEdit(label_instruments->text(), *lineEdit_instruments, OPTIONAL_FIELD);

	mapFields["Method"] = comboBoxMethod;
	mapFields["From"] = sampleFrom;
	mapFields["To"] = sampleTo;

	std::string equation_name = equationName.toStdString();
	if(!equation_name.empty() && Equations.contains(equation_name))
	{
		// --- existing equation ---
		className = "TAB_EDIT_EQUATION";
		lineEdit_name->setReadOnly(true);
		display_equation(equationName, true);
	}
	else
	{
		// --- new equation ---
		className = "TAB_ADD_EQUATION";
		lineEdit_name->setReadOnly(false);
		loadSettings();
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

	if(est_results) 
		delete est_results;
}

void EditEquationDialog::display_equation(const QString& equationName, const bool update_block_from_equation)
{
	try
	{
		Equation equation(equationName.toStdString());

		lineName->setQValue(equationName);

		// editable values
		comboBoxMethod->setQValue(equation.get_method_as_int());
		Sample sample = equation.get_sample();
		sampleFrom->setQValue(QString::fromStdString(sample.start_period().to_string()));
		sampleTo->setQValue(QString::fromStdString(sample.end_period().to_string()));
		lineLec->setQValue(QString::fromStdString(equation.get_lec()));
		lineComment->setQValue(QString::fromStdString(equation.get_comment()));
		lineInstruments->setQValue(QString::fromStdString(equation.get_instruments()));
		if(update_block_from_equation)
			lineBlock->setQValue(QString::fromStdString(equation.get_block()));

		// read-only values
		std::array<float, EQS_NBTESTS> tests = equation.get_tests();
		lineEdit_tests_r2adj->setText(QString::number(tests[IE_R2ADJ], 'g', 3));
		lineEdit_tests_durbw->setText(QString::number(tests[IE_DW], 'g', 3));
		lineEdit_tests_fstat->setText(QString::number(tests[IE_FSTAT], 'g', 3));
		lineEdit_tests_loglk->setText(QString::number(tests[IE_LOGLIK], 'g', 3));
	}
	catch(const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", "Cannot display equation '" + equationName + "'.\n" + QString(e.what()));
	}
}

void EditEquationDialog::edit()
{
	QString equationName = lineName->extractAndVerify();

	try
	{
		std::string equation_name = equationName.toStdString();

		// TODO : remove extra \n
		std::string lec = lineLec->extractAndVerify().toStdString();
		int i_method = comboBoxMethod->extractAndVerify();
		std::string method = (i_method >= 0) ? vEquationMethods[i_method] : "";
		std::string from = sampleFrom->extractAndVerify().toStdString();
		std::string to = sampleTo->extractAndVerify().toStdString();
		std::string comment = lineComment->extractAndVerify().toStdString();
		std::string block = lineBlock->extractAndVerify().toStdString();
		std::string instruments = lineInstruments->extractAndVerify().toStdString();
		
		std::array<float, EQS_NBTESTS> tests = { 0.0 };

		// update equation
		if (Equations.contains(equation_name))
		{
			Equation eq = Equations.get(equation_name);
			computeHash(eq, true);

			Equations.update(equation_name, lec, method, from, to, comment, instruments, block);
		
			Equation updated_eq = Equations.get(equation_name);
			computeHash(updated_eq);
		}
		// new equation
		else
		{
			Equations.add(equation_name, lec, method, from, to, comment, instruments, block);
			emit databaseModified();
		}

		if(est_results) 
			est_results->save();
		
		this->accept();
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", "Cannot edit equation '" + equationName + "'.\n" + QString(e.what()));
	}
}

void EditEquationDialog::display_coefs()
{
	if(est_results)
	{
		try
		{
			EstimationCoefsDialog dialog(est_results, this);
			dialog.exec();
		}
		catch (const std::exception& e)
		{
			QMessageBox::warning(nullptr, "WARNING", "Cannot display coefficients.\n" + QString(e.what()));
		}	
	}
	else
	{
		QMessageBox::warning(nullptr, "WARNING", "No estimation has been done yet");
	}
}

void EditEquationDialog::estimate()
{
	// block
	QString qBlock = lineBlock->extractAndVerify();
	if(qBlock.isEmpty()) 
		qBlock = lineName->extractAndVerify();

	try
	{
		if(est_results)
		{
			delete est_results;
			est_results = nullptr;
		}

		// sample
		std::string from = sampleFrom->extractAndVerify().toStdString();
		std::string to = sampleTo->extractAndVerify().toStdString();
		Sample sample(from, to);

		// block
		std::string block = qBlock.toStdString();
		
		// check method
		int i_method = comboBoxMethod->extractAndVerify();
		if(i_method < 0)
			throw std::invalid_argument("Please choose a method in the drop-down list");

		est_results = equations_estimate(block, from, to);
	}
	catch (const std::exception& e)
	{
		est_results = nullptr;
		QMessageBox::warning(nullptr, "WARNING", "Cannot estimate block or equation '" + qBlock + "'.\n" + QString(e.what()));
		return;
	}

	// reload content of the current equation.
	// Specifically, reload the values for the estimation tests
	QString equationName = lineName->extractAndVerify();
	display_equation(equationName);
}

void EditEquationDialog::next()
{
	try
	{	
		// get current equation name
		QString equationName = lineName->extractAndVerify();

		QString block = lineEdit_block->text();
		// if content of the 'block' field is empty, set block = current equation name 
		if(block.isEmpty())
		{
			block = equationName;
			eqs_list.clear();
			eqs_list << equationName;
			next_block = equationName;
			return;
		}
		else
		{
			// check if block changed since last call to next()
			// If changed, update 'eqs_list' and 'next_block'
			if(next_block != block)
			{
				eqs_list.clear();
				for(const std::string& name: Equations.get_names(block.toStdString()))
					eqs_list << QString::fromStdString(name);

				// check if 'block' was a valid pattern for the get_names() method
				if(eqs_list.size() > 0)
					next_block = block;
			}

			// check if the present equation is in the 'block' of equations to estimate:
			// - If yes -> get next equation and display it
			// - If not -> add it to the block and return
			qsizetype current_index = eqs_list.indexOf(equationName);
			if(current_index >= 0)
			{
				// check if last element of the list:
				// yes -> take first equation
				// no -> take next equation
				equationName = (current_index == eqs_list.size() - 1) ? eqs_list.first() : eqs_list[current_index + 1];
				display_equation(equationName);
			}
			else
			{
				block += ";" + equationName;
				lineBlock->setQValue(block);
			}
		}
	}
	catch(const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", "Cannot find next equation.\n" + QString(e.what()));
	}	
}

void EditEquationDialog::dynamic_adjustment()
{
	try
	{
		QString lec = lineLec->extractAndVerify();
		DynAdjustmentDialog dialog(lec, this);
		dialog.exec();
		lineLec->setQValue(lec);
	}
	catch(const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", "Cannot compute dynamic adjustment.\n" +  QString(e.what()));
	}	
}

void EditEquationDialog::results()
{
	if(est_results)
	{
		try
		{
			QString equationName = lineName->extractAndVerify();
			EstimationResultsDialog dialog(est_results, equationName, this);
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
	try
	{		
		UnitRootDialog dialog(this);
		dialog.exec();
	}
	catch(const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", "Cannot compute Dickey Fuller test.\n" +  QString(e.what()));
	}
}

void EditEquationDialog::help()
{
	QDesktopServices::openUrl(url_manual);
}