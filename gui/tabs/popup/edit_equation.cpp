#include "edit_equation.h"


QIodeEditEquation::QIodeEditEquation(const QString& equationName, QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	setupUi(this);

	textEdit_lec->setAcceptRichText(false);
	textEdit_lec->setLineWrapMode(QTextEdit::LineWrapMode::WidgetWidth);

	int i = 0;
	QList<QString> list_methods;
	for (const std::string& method : vEquationMethods) list_methods << QString::fromStdString(method);

	lineName = new WrapperIodeNameEdit(label_name->text(), *lineEdit_name, I_EQUATIONS, REQUIRED_FIELD);
	comboBoxMethod = new WrapperComboBox(label_method->text(), *comboBox_method, REQUIRED_FIELD, list_methods);
	sampleFrom = new WrapperSampleEdit(label_from->text(), *sampleEdit_from, REQUIRED_FIELD);
	sampleTo = new WrapperSampleEdit(label_to->text(), *sampleEdit_to, REQUIRED_FIELD);
	lineLec = new WrapperQTextEdit(label_lec->text(), *textEdit_lec, OPTIONAL_FIELD);
	lineComment = new WrapperQLineEdit(label_comment->text(), *lineEdit_comment, OPTIONAL_FIELD);
	lineBlock = new WrapperQLineEdit(label_block->text(), *lineEdit_block, OPTIONAL_FIELD);
	lineInstruments = new WrapperQLineEdit(label_instruments->text(), *lineEdit_instruments, OPTIONAL_FIELD);

	std::string std_equation_name = equationName.toStdString();

	if(!std_equation_name.empty() && kdb.contains(std_equation_name))
	{
		// --- existing equation ---
		// editable values
		Equation eq = kdb.get(std_equation_name);
		comboBoxMethod->setQValue(eq.get_method_as_int());
		Sample sample = eq.get_sample();
		sampleFrom->setQValue(QString::fromStdString(sample.start_period().to_string()));
		sampleTo->setQValue(QString::fromStdString(sample.end_period().to_string()));
		lineLec->setQValue(QString::fromStdString(eq.get_lec()));
		lineComment->setQValue(QString::fromStdString(eq.get_comment()));
		lineBlock->setQValue(QString::fromStdString(eq.get_block()));
		lineInstruments->setQValue(QString::fromStdString(eq.get_instruments()));

		// read-only values
		lineEdit_name->setText(equationName);
		std::array<float, EQS_NBTESTS> tests = eq.get_tests();
		lineEdit_tests_r2adj->setText(QString::number(tests[KE_R2ADJ - 10], 'g', 3));
		lineEdit_tests_durbw->setText(QString::number(tests[KE_DW - 10], 'g', 3));
		lineEdit_tests_fstat->setText(QString::number(tests[KE_FSTAT - 10], 'g', 3));
		lineEdit_tests_loglk->setText(QString::number(tests[KE_LOGLIK - 10], 'g', 3));
	}
	else
	{
		// --- new equation ---
		KDBVariables kdb_vars;
		Sample sample = kdb_vars.get_sample();
		if (sample.nb_periods() > 0)
		{
			sampleFrom->setQValue(QString::fromStdString(sample.start_period().to_string()));
			sampleTo->setQValue(QString::fromStdString(sample.end_period().to_string()));
		}
	}
}

QIodeEditEquation::~QIodeEditEquation() 
{
	delete comboBoxMethod;
	delete sampleFrom;
	delete sampleTo;
	delete lineLec;
	delete lineBlock;
	delete lineInstruments;
}

void QIodeEditEquation::edit()
{
	try
	{
		std::string equation_name = lineName->extractAndVerify().toStdString();

		// TODO : remove extra \n
		std::string lec = lineLec->extractAndVerify().toStdString();
		int i_method = comboBoxMethod->extractAndVerify();
		std::string method = vEquationMethods[i_method];
		std::string from = sampleFrom->extractAndVerify().toStdString();
		std::string to = sampleTo->extractAndVerify().toStdString();
		std::string comment = lineComment->extractAndVerify().toStdString();
		std::string block = lineBlock->extractAndVerify().toStdString();
		std::string instruments = lineInstruments->extractAndVerify().toStdString();

		// update equation
		if (kdb.contains(equation_name))
		{
			Equation eq(equation_name);

			eq.set_lec(lec, equation_name);
			eq.set_method(method);
			eq.set_sample(from, to);
			eq.set_comment(comment);
			eq.set_block(block);
			eq.set_instruments(instruments);

			kdb.update(equation_name, eq);
		}
		// new equation
		else
		{
			Sample sample(from, to);
			std::array<float, EQS_NBTESTS> tests = { 0.0 };
			kdb.add(equation_name, lec, comment, method, sample, instruments, block, tests, true);
		}
		this->accept();
	}
	catch (const std::runtime_error& e)
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), tr("Warning"), tr(e.what()));
	}
}

void QIodeEditEquation::help()
{
	QUrl url = get_url_iode_manual();
	QDesktopServices::openUrl(url);
}