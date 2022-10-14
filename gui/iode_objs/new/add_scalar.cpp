#include "add_scalar.h"


QIodeAddScalar::QIodeAddScalar(const QString& project_settings_filepath, QWidget* parent, Qt::WindowFlags f) : QIodeSettings(project_settings_filepath, parent, f)
{
	// TODO: if possible, find a way to initialize className inside MixingSettings
	// NOTE FOR DEVELOPPERS: we cannot simply call the line below from the constructor of MixingSettings 
	//                       since in that case this refers to MixingSettings and NOT the derived class
	className = QString::fromStdString(typeid(this).name());

	setupUi(this);

	lineName = new WrapperIodeNameEdit(label_name->text(), *lineEdit_name, I_SCALARS, REQUIRED_FIELD);
	lineValue = new WrapperQLineEdit(label_value->text(), *lineEdit_value, REQUIRED_FIELD);
	spinBoxRelax = new WrapperDoubleSpinBox(label_relax->text(), *doubleSpinBox_relax, REQUIRED_FIELD);

	mapFields["Relax"] = spinBoxRelax;

	lineEdit_value->setValidator(new QDoubleValidator(parent));

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
		QMessageBox::warning(static_cast<QWidget*>(parent()), tr("Warning"), tr(e.what()));
	}
}

void QIodeAddScalar::help()
{
	QUrl url = get_url_iode_manual();
	QDesktopServices::openUrl(url);
}



