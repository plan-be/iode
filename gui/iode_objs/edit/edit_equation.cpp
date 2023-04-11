#include "edit_equation.h"


QIodeEditEquation::QIodeEditEquation(const QString& equationName, const QString& project_settings_filepath, QWidget* parent, Qt::WindowFlags f) : 
	QIodeSettings(project_settings_filepath, parent, f), project_settings_filepath(project_settings_filepath)
{
	// TODO: if possible, find a way to initialize className inside MixingSettings
	// NOTE FOR DEVELOPPERS: we cannot simply call the line below from the constructor of MixingSettings 
	//                       since in that case this refers to MixingSettings and NOT the derived class
	className = QString::fromStdString(typeid(this).name());

	setupUi(this);

	estimation = nullptr;

	textEdit_lec->setAcceptRichText(false);
	textEdit_lec->setLineWrapMode(QTextEdit::LineWrapMode::WidgetWidth);

	int i = 0;
	QList<QString> list_methods;
	for (const std::string& method : vEquationMethods) list_methods << QString::fromStdString(method);

	lineName = new WrapperIodeNameEdit(label_name->text(), *lineEdit_name, I_EQUATIONS, REQUIRED_FIELD);
	comboBoxMethod = new WrapperComboBox(label_method->text(), *comboBox_method, OPTIONAL_FIELD, list_methods);
	sampleFrom = new WrapperSampleEdit(label_from->text(), *sampleEdit_from, OPTIONAL_FIELD);
	sampleTo = new WrapperSampleEdit(label_to->text(), *sampleEdit_to, OPTIONAL_FIELD);
	lineLec = new WrapperQTextEdit(label_lec->text(), *textEdit_lec, REQUIRED_FIELD);
	lineComment = new WrapperQLineEdit(label_comment->text(), *lineEdit_comment, OPTIONAL_FIELD);
	lineBlock = new WrapperQLineEdit(label_block->text(), *lineEdit_block, OPTIONAL_FIELD);
	lineInstruments = new WrapperQLineEdit(label_instruments->text(), *lineEdit_instruments, OPTIONAL_FIELD);

	mapFields["Method"] = comboBoxMethod;
	mapFields["From"] = sampleFrom;
	mapFields["To"] = sampleTo;

	std::string equation_name = equationName.toStdString();
	if(!equation_name.empty() && kdb_eqs.contains(equation_name))
	{
		// --- existing equation ---
		lineEdit_name->setReadOnly(true);
		NamedEquation equation(equation_name);
		display_equation(equation);
	}
	else
	{
		// --- new equation ---
		className = className.replace("Edit", "New");
		lineEdit_name->setReadOnly(false);
		loadSettings();
	}
}

QIodeEditEquation::~QIodeEditEquation() 
{
	delete lineName;
	delete comboBoxMethod;
	delete sampleFrom;
	delete sampleTo;
	delete lineLec;
	delete lineComment;
	delete lineBlock;
	delete lineInstruments;

	if(estimation) delete estimation;
}

void QIodeEditEquation::set_estimation()
{
	try
	{
		if(estimation)
		{
			delete estimation;
			estimation = nullptr;
		}
		
		std::string from = sampleFrom->extractAndVerify().toStdString();
		std::string to = sampleTo->extractAndVerify().toStdString();
		std::string block = lineBlock->extractAndVerify().toStdString();
		if(block.empty()) block = lineName->extractAndVerify().toStdString();
		estimation = new Estimation(from, to, block);

		NamedEquation equation = estimation->current_equation();
		display_equation(equation);
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), tr("Warning"), tr(e.what()));
	}
}

void QIodeEditEquation::display_equation(const NamedEquation& equation)
{
	lineName->setQValue(QString::fromStdString(equation.name));
	Equation eq = equation.eq;

	// editable values
	comboBoxMethod->setQValue(eq.get_method_as_int());
	Sample sample = eq.get_sample();
	sampleFrom->setQValue(QString::fromStdString(sample.start_period().to_string()));
	sampleTo->setQValue(QString::fromStdString(sample.end_period().to_string()));
	lineLec->setQValue(QString::fromStdString(eq.get_lec()));
	lineComment->setQValue(QString::fromStdString(eq.get_comment()));
	lineBlock->setQValue(QString::fromStdString(eq.get_block()));
	lineInstruments->setQValue(QString::fromStdString(eq.get_instruments()));

	// read-only values
	std::array<float, EQS_NBTESTS> tests = eq.get_tests();
	lineEdit_tests_r2adj->setText(QString::number(tests[IE_R2ADJ], 'g', 3));
	lineEdit_tests_durbw->setText(QString::number(tests[IE_DW], 'g', 3));
	lineEdit_tests_fstat->setText(QString::number(tests[IE_FSTAT], 'g', 3));
	lineEdit_tests_loglk->setText(QString::number(tests[IE_LOGLIK], 'g', 3));
}

void QIodeEditEquation::edit()
{
	try
	{
		std::string equation_name = lineName->extractAndVerify().toStdString();

		// TODO : remove extra \n
		std::string lec = lineLec->extractAndVerify().toStdString();
		int i_method = comboBoxMethod->extractAndVerify();
		std::string method = (i_method > 0) ? vEquationMethods[i_method] : "";
		std::string from = sampleFrom->extractAndVerify().toStdString();
		std::string to = sampleTo->extractAndVerify().toStdString();
		std::string comment = lineComment->extractAndVerify().toStdString();
		std::string block = lineBlock->extractAndVerify().toStdString();
		std::string instruments = lineInstruments->extractAndVerify().toStdString();


		Sample* sample = nullptr; 
		if(!(from.empty() || to.empty()))
			sample = new Sample(from, to);
		
		std::array<float, EQS_NBTESTS> tests = { 0.0 };

		// update equation
		if (kdb_eqs.contains(equation_name))
			kdb_eqs.update(equation_name, lec, comment, method, sample, instruments, block, &tests);
		// new equation
		else
			kdb_eqs.add(equation_name, lec, comment, method, sample, instruments, block, tests, true);

		if(estimation) 
			estimation->save();

		if(sample) 
			delete sample;
		
		this->accept();
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), tr("Warning"), tr(e.what()));
	}
}

void QIodeEditEquation::display_coefs()
{
	if(estimation)
	{
		try
		{
			KDBScalars* kdb_scl = estimation->get_coefficients();
			QIodeEstimationCoefs dialog(kdb_scl, this);
			dialog.exec();
		}
		catch (const std::exception& e)
		{
			QMessageBox::warning(static_cast<QWidget*>(parent()), tr("Warning"), tr(e.what()));
		}	
	}
	else
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), tr("Warning"), "No estimation has been done yet");
	}
}

void QIodeEditEquation::estimate()
{
	set_estimation();
	try
	{
		estimation->equations_estimate();
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), tr("Warning"), tr(e.what()));
	}
}

void QIodeEditEquation::next()
{
	if(estimation)
	{
		try
		{
			NamedEquation equation = estimation->next_equation();
			display_equation(equation);
		}
		catch (const std::exception& e)
		{
			QMessageBox::warning(static_cast<QWidget*>(parent()), tr("Warning"), tr(e.what()));
		}
	}
}

void QIodeEditEquation::dynamic_adjustment()
{
	QString lec = lineLec->extractAndVerify();
	QIodeDynAdjustment dialog(lec, project_settings_filepath, this);
	dialog.exec();
	lineLec->setQValue(lec);
}

void QIodeEditEquation::results()
{
	if(estimation)
	{
		try
		{
			QIodeEstimationResults dialog(estimation, this);
			dialog.exec();
		}
		catch (const std::exception& e)
		{
			QMessageBox::warning(static_cast<QWidget*>(parent()), tr("Warning"), tr(e.what()));
		}
	}
	else
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), tr("Warning"), "No estimation has been done yet");
	}
}

void QIodeEditEquation::unit_root()
{
	QIodeUnitRoot dialog(project_settings_filepath, this);
	dialog.exec();
}

void QIodeEditEquation::help()
{
	QDesktopServices::openUrl(url_manual);
}