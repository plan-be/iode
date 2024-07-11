#include "unit_root.h"


UnitRootDialog::UnitRootDialog(QWidget* parent) : IodeSettingsDialog(parent), precision(6)
{
	setupUi(this);

    lineSeries = new WrapperQLineEdit(label_series->text(), *lineEdit_series, REQUIRED_FIELD);
    spinBoxOrder = new WrapperSpinBox(label_order->text(), *spinBox_order, REQUIRED_FIELD);

    mapFields["Series"] = lineSeries;
    mapFields["Order"] = spinBoxOrder;

	className = "EQUATION_UNIT_ROOT";
    loadSettings();
}

UnitRootDialog::~UnitRootDialog()
{
    delete lineSeries;
    delete spinBoxOrder;
}

QString UnitRootDialog::get_tstat(Scalar* scalar)
{
    if (IODE_IS_A_NUMBER(scalar->val) && IODE_IS_A_NUMBER(scalar->std) && !IODE_IS_0(scalar->std))
        return QString::number(scalar->val / scalar->std, 'g', precision);
    else
        return NAN_REP;
}

// QUESTION FOR JMP : what means ADF in results of Unit Root ?
void UnitRootDialog::analyse()
{
    KDBScalars* kdb_scl;
    QString res;

    try
    {
        QString lec = lineSeries->extractAndVerify();
        int order = spinBoxOrder->extractAndVerify();

        // no drift - no trend
        kdb_scl = dickey_fuller_test(lec.toStdString(), false, false, 0);
        Scalar* df = kdb_scl->get("df_");

        // drift - no trend
        kdb_scl = dickey_fuller_test(lec.toStdString(), true, false, 0);
        Scalar* df_drift = kdb_scl->get("df_");
        Scalar* df_drift_d = kdb_scl->get("df_d");

        // drift - trend
        kdb_scl = dickey_fuller_test(lec.toStdString(), true, true, 0);
        Scalar* df_trend = kdb_scl->get("df_");
        Scalar* df_trend_d = kdb_scl->get("df_d");
        Scalar* df_trend_t = kdb_scl->get("df_t");

        // order
        kdb_scl = dickey_fuller_test(lec.toStdString(), false, false, order);
        Scalar* df_order = kdb_scl->get("df_");

        // T-Stats
        res = "\tDF\tDrift\tTrend\tADF\n";
        res += lec + "\t" + get_tstat(df) + "\t";
        res += get_tstat(df_drift) + "\t";
        res += get_tstat(df_trend) + "\t";
        res += get_tstat(df_order);
        res += "\n\n";

        // no drift - no trend
        res += "ADF(" + lec + "drift = 0, trend = 0, order = 0) = ";
        res += get_tstat(df) + "\n";
        res += "\tValue\tStd Error\tT-Statistic\n";
        res += "ADF\t";
        res += QString::number(df->val, 'g', precision) + "\t";
        res += QString::number(df->std, 'g', precision) + "\t";
        res += get_tstat(df);
        res += "\n\n";

        // drift - no trend
        res += "ADF(" + lec + "drift = 1, trend = 0, order = 0) = ";
        res += get_tstat(df_drift) + "\n";
        res += "\tValue\tStd Error\tT-Statistic\n";
        res += "ADF\t";
        res += QString::number(df_drift->val, 'g', precision) + "\t";
        res += QString::number(df_drift->std, 'g', precision) + "\t";
        res += get_tstat(df_drift) + "\n";
        res += "Drift\t";
        res += QString::number(df_drift_d->val, 'g', precision) + "\t";
        res += QString::number(df_drift_d->std, 'g', precision) + "\t";
        res += get_tstat(df_drift_d);
        res += "\n\n";

        // drift - trend
        res += "ADF(" + lec + "drift = 1, trend = 1, order = 0) = ";
        res += get_tstat(df_trend) + "\n";
        res += "\tValue\tStd Error\tT-Statistic\n";
        res += "ADF\t";
        res += QString::number(df_trend->val, 'g', precision) + "\t";
        res += QString::number(df_trend->std, 'g', precision) + "\t";
        res += get_tstat(df_trend) + "\n";
        res += "Drift\t";
        res += QString::number(df_trend_d->val, 'g', precision) + "\t";
        res += QString::number(df_trend_d->std, 'g', precision) + "\t";
        res += get_tstat(df_trend_d) + "\n";
        res += "Trend\t";
        res += QString::number(df_trend_t->val, 'g', precision) + "\t";
        res += QString::number(df_trend_t->std, 'g', precision) + "\t";
        res += get_tstat(df_trend_t);
        res += "\n\n";

        // order
        res += "ADF(" + lec + "drift = 0, trend = 0, order = " + QString::number(order) + ") = ";
        res += get_tstat(df_order) + "\n";
        res += "\tValue\tStd Error\tT-Statistic\n";
        res += "ADF\t";
        res += QString::number(df_order->val, 'g', precision) + "\t";
        res += QString::number(df_order->std, 'g', precision) + "\t";
        res += get_tstat(df_order) + "\n";
        for (int i=1; i<=order; i++)
        {
            Scalar* df_order_x = kdb_scl->get("df" + std::to_string(i));
            res += "order " + QString::number(i) + "\t";
            res += QString::number(df_order_x->val, 'g', precision) + "\t";
            res += QString::number(df_order_x->std, 'g', precision) + "\t";
            res += get_tstat(df_order_x) + "\n";
        }
        res += "\n";

        textEdit_results->setText(res);
    }
    catch(const std::exception& e)
    {
        QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
    }
}

void UnitRootDialog::help()
{
	QDesktopServices::openUrl(url_manual);
}
