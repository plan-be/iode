#include "add_scalar.h"


AddScalarDialog::AddScalarDialog(KDBScalars* database, QWidget* parent) : IodeSettingsDialog(parent), database(database)
{
	setupUi(this);

	lineName = new WrapperIodeNameEdit(label_name->text(), *lineEdit_name, SCALARS, REQUIRED_FIELD);
	lineValue = new WrapperQLineEdit(label_value->text(), *lineEdit_value, REQUIRED_FIELD);
	spinBoxRelax = new WrapperDoubleSpinBox(label_relax->text(), *doubleSpinBox_relax, REQUIRED_FIELD);

	mapFields["Relax"] = spinBoxRelax;

	lineEdit_value->setValidator(new QDoubleValidator(parent));

	className = "TAB_ADD_SCALAR";
	loadSettings();
}

AddScalarDialog::~AddScalarDialog()
{
	delete lineName;
	delete lineValue;
	delete spinBoxRelax;
}

void AddScalarDialog::add()
{
	try
	{
		std::string name = lineName->extractAndVerify().toStdString();
		double value = (double) lineValue->extractAndVerify().toDouble();
		double relax = (double) spinBoxRelax->extractAndVerify();
		// check if already exists in the global IODE database since the variable 'database' may represents only a subset
		if(Scalars.contains(name))
		{
			int res = QMessageBox::question(nullptr, "WARNING", QString::fromStdString(name) + " already exists. Replace it ?");
			if(res != QMessageBox::StandardButton::Yes)
			{
				this->reject();
				return;
			}
			Scalars.update(name, value, relax);
		}
		else
		{
			emit newObjectInserted(QString::fromStdString(name));
			database->add(name, value, relax);
		}
		
		this->accept();
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
	}
}

void AddScalarDialog::help()
{
	
	QDesktopServices::openUrl(url_manual);
}



