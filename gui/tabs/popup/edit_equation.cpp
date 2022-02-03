#include "edit_equation.h"


QIodeEditEquation::QIodeEditEquation(const QString& equationName, QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	setupUi(this);

	textEdit_lec->setAcceptRichText(false);
	textEdit_lec->setLineWrapMode(QTextEdit::LineWrapMode::WidgetWidth);

	int i = 0;
	QList<QString> list_methods;
	for (const std::string& method : vEquationMethods) list_methods << QString::fromStdString(method);

	comboBoxMethod = new WrapperComboBox(label_method->text(), *comboBox_method, REQUIRED_FIELD, list_methods);
	sampleFrom = new WrapperSampleEdit(label_from->text(), *sampleEdit_from, REQUIRED_FIELD);
	sampleTo = new WrapperSampleEdit(label_to->text(), *sampleEdit_to, REQUIRED_FIELD);
	lineLec = new WrapperQTextEdit(label_lec->text(), *textEdit_lec, OPTIONAL_FIELD);
	lineComment = new WrapperQLineEdit(label_comment->text(), *lineEdit_comment, OPTIONAL_FIELD);
	lineBlock = new WrapperQLineEdit(label_block->text(), *lineEdit_block, OPTIONAL_FIELD);
	lineInstruments = new WrapperQLineEdit(label_instruments->text(), *lineEdit_instruments, OPTIONAL_FIELD);

	std::string std_equation_name = equationName.toStdString();

	// editable values
	comboBoxMethod->setQValue(kdb.getMethodAsInt(std_equation_name));
	Sample sample = kdb.getSample(std_equation_name);
	sampleFrom->setQValue(QString::fromStdString(sample.start_period().to_string()));
	sampleTo->setQValue(QString::fromStdString(sample.end_period().to_string()));
	lineLec->setQValue(QString::fromStdString(kdb.getLec(std_equation_name)));
	lineComment->setQValue(QString::fromStdString(kdb.getComment(std_equation_name)));
	lineBlock->setQValue(QString::fromStdString(kdb.getBlock(std_equation_name)));
	lineInstruments->setQValue(QString::fromStdString(kdb.getInstruments(std_equation_name)));

	// read-only values
	lineEdit_name->setText(equationName);
	std::array<float, EQS_NBTESTS> tests = kdb.getTests(std_equation_name);
	lineEdit_tests_r2adj->setText(QString::number(tests[KE_R2ADJ - 10], 'g', 3));
	lineEdit_tests_durbw->setText(QString::number(tests[KE_DW - 10], 'g', 3));
	lineEdit_tests_fstat->setText(QString::number(tests[KE_FSTAT - 10], 'g', 3));
	lineEdit_tests_loglk->setText(QString::number(tests[KE_LOGLIK - 10], 'g', 3));
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
		std::string equation_name = lineEdit_name->text().toStdString();

		// update equation
		int method = comboBoxMethod->extractAndVerify();
		std::string from = sampleFrom->extractAndVerify().toStdString();
		std::string to = sampleTo->extractAndVerify().toStdString();
		Sample sample(from, to);
		// TODO : remove extra \n
		std::string lec = lineLec->extractAndVerify().toStdString();
		std::string comment = lineComment->extractAndVerify().toStdString();
		std::string block = lineBlock->extractAndVerify().toStdString();
		std::string instruments = lineInstruments->extractAndVerify().toStdString();

		QString equation_values("equation name: %1\nlec: %2\ncomment: %3\nmethod: %4\nsample: %5\ninstruments: %6\nblock: %7");

		kdb.setEquation(equation_name, lec, comment, method, &sample, instruments, block, NULL, 0);

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