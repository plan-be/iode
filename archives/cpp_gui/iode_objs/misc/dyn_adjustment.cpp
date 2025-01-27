#include "dyn_adjustment.h"


DynAdjustmentDialog::DynAdjustmentDialog(QString& lec, QWidget* parent) : 
    IodeSettingsDialog(parent), lec(lec)
{
	setupUi(this);

    textEdit_equation->setText(lec);

    QList<QString> list_methods;
    for(const std::string& method : v_adjustment_method) list_methods.append(QString::fromStdString(method));
    comboMethod = new WrapperComboBox(label_method->text(), *comboBox_method, REQUIRED_FIELD, list_methods);
    lineCoef1 = new WrapperQLineEdit(label_coefs->text(), *lineEdit_coef1, REQUIRED_FIELD);
    lineCoef2 = new WrapperQLineEdit(label_coefs->text(), *lineEdit_coef2, REQUIRED_FIELD);

    mapFields["Method"] = comboMethod;
    mapFields["Coef1"] = lineCoef1;
    mapFields["Coef2"] = lineCoef2;

	className = "EQUATION_DYNAMIC_ADJUSTMENT";
    loadSettings();
}

DynAdjustmentDialog::~DynAdjustmentDialog()
{
    delete comboMethod;
    delete lineCoef1;
    delete lineCoef2;
}

void DynAdjustmentDialog::adjust()
{
    try
    {
        std::string std_lec = textEdit_equation->toPlainText().toStdString();
        IodeAdjustmentMethod method = (IodeAdjustmentMethod) comboMethod->extractAndVerify();
        std::string coef1 = lineCoef1->extractAndVerify().toStdString();
        std::string coef2 = lineCoef2->extractAndVerify().toStdString();

        std::string adjusted_lec = dynamic_adjustment(method, std_lec, coef1, coef2);
        lec = QString::fromStdString(adjusted_lec);

        this->accept();
    }
    catch(const std::exception& e)
    {
        QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
    }
}

void DynAdjustmentDialog::help()
{
	QDesktopServices::openUrl(url_manual);
}
