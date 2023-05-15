#include "add_scalar.h"


QIodeAddScalar::QIodeAddScalar(QWidget* parent, Qt::WindowFlags f) : QIodeSettings( parent, f)
{
	setupUi(this);

	lineName = new WrapperIodeNameEdit(label_name->text(), *lineEdit_name, I_SCALARS, REQUIRED_FIELD);
	lineValue = new WrapperQLineEdit(label_value->text(), *lineEdit_value, REQUIRED_FIELD);
	spinBoxRelax = new WrapperDoubleSpinBox(label_relax->text(), *doubleSpinBox_relax, REQUIRED_FIELD);

	mapFields["Relax"] = spinBoxRelax;

	lineEdit_value->setValidator(new QDoubleValidator(parent));

	className = "TAB_ADD_SCALAR";
	loadSettings();
}

QIodeAddScalar::~QIodeAddScalar()
{
	delete lineName;
	delete lineValue;
	delete spinBoxRelax;
}

void QIodeAddScalar::add()
{
	try
	{
		KDBScalars kdb;
		std::string name = lineName->extractAndVerify().toStdString();
		IODE_REAL value = (IODE_REAL) lineValue->extractAndVerify().toDouble();
		IODE_REAL relax = (IODE_REAL) spinBoxRelax->extractAndVerify();
		kdb.add(name, value, relax);
		this->accept();
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), tr("WARNING"), tr(e.what()));
	}
}

void QIodeAddScalar::help()
{
	
	QDesktopServices::openUrl(url_manual);
}



