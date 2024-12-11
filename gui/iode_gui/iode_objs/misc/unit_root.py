from PySide6.QtCore import Slot
from PySide6.QtWidgets import QWidget, QMessageBox

from iode_gui.utils import NAN_REP
from iode_gui.settings import MixinSettingsDialog
from .ui_unit_root import Ui_UnitRootDialog

from iode import is_NA, Scalar, dickey_fuller_test


class UnitRootDialog(MixinSettingsDialog):

    def __init__(self, parent: QWidget = None, precision: int = 6):
        super().__init__(parent)
        self.ui = Ui_UnitRootDialog()
        self.ui.setupUi(self)
        self.precision = precision

    def compute_tstat(self, scalar: Scalar) -> str:
        if not is_NA(scalar.value) and not is_NA(scalar.std) and scalar.std > 0.0:
            return f"{(scalar.value / scalar.std):.{self.precision}g}"
        else:
            return NAN_REP

    @Slot()
    def analyse(self):
        try:
            lec = self.ui.lineEdit_series.text()
            order = self.ui.spinBox_order.value()

            # no drift - no trend
            scalars_db = dickey_fuller_test(lec, False, False, 0).copy()
            df: Scalar = scalars_db["df_"]

            # drift - no trend
            scalars_db = dickey_fuller_test(lec, True, False, 0).copy()
            df_drift: Scalar = scalars_db["df_"]
            df_drift_d: Scalar = scalars_db["df_d"]

            # drift - trend
            scalars_db = dickey_fuller_test(lec, True, True, 0).copy()
            df_trend: Scalar = scalars_db["df_"]
            df_trend_d: Scalar = scalars_db["df_d"]
            df_trend_t: Scalar = scalars_db["df_t"]

            # order
            scalars_db = dickey_fuller_test(lec, False, False, order).copy()
            df_order: Scalar = scalars_db["df_"]

            # T-Stats
            res = "\tDF\tDrift\tTrend\tADF\n"
            res += lec + "\t" + self.compute_tstat(df) + "\t"
            res += self.compute_tstat(df_drift) + "\t"
            res += self.compute_tstat(df_trend) + "\t"
            res += self.compute_tstat(df_order) + "\n\n"

            # No drift - no trend
            res += f"ADF({lec}, drift = 0, trend = 0, order = 0) = "
            res += self.compute_tstat(df) + "\n"
            res += "\tValue\tStd Error\tT-Statistic\n"
            res += "ADF\t"
            res += f"{df.value:{self.precision}g}\t"
            res += f"{df.std:{self.precision}g}\t"
            res += self.compute_tstat(df) + "\n\n"

            # Drift - no trend
            res += f"ADF({lec}, drift = 1, trend = 0, order = 0) = "
            res += self.compute_tstat(df_drift) + "\n"
            res += "\tValue\tStd Error\tT-Statistic\n"
            res += "ADF\t"
            res += f"{df_drift.value:{self.precision}g}\t"
            res += f"{df_drift.std:{self.precision}g}\t"
            res += self.compute_tstat(df_drift) + "\n"
            res += "Drift\t"
            res += f"{df_drift_d.value:{self.precision}g}\t"
            res += f"{df_drift_d.std:{self.precision}g}\t"
            res += self.compute_tstat(df_drift_d) + "\n\n"

            # Drift - trend
            res += f"ADF({lec}, drift = 1, trend = 1, order = 0) = "
            res += self.compute_tstat(df_trend) + "\n"
            res += "\tValue\tStd Error\tT-Statistic\n"
            res += "ADF\t"
            res += f"{df_trend.value:{self.precision}g}\t"
            res += f"{df_trend.std:{self.precision}g}\t"
            res += self.compute_tstat(df_trend) + "\n"
            res += "Drift\t"
            res += f"{df_trend_d.value:{self.precision}g}\t"
            res += f"{df_trend_d.std:{self.precision}g}\t"
            res += self.compute_tstat(df_trend_d) + "\n"
            res += "Trend\t"
            res += f"{df_trend_t.value:{self.precision}g}\t"
            res += f"{df_trend_t.std:{self.precision}g}\t"
            res += self.compute_tstat(df_trend_t) + "\n\n"

            # Order
            res += f"ADF({lec}, drift = 0, trend = 0, order = {order}) = "
            res += self.compute_tstat(df_order) + "\n"
            res += "\tValue\tStd Error\tT-Statistic\n"
            res += "ADF\t"
            res += f"{df_order.value:{self.precision}g}\t"
            res += f"{df_order.std:{self.precision}g}\t"
            res += self.compute_tstat(df_order) + "\n"
            for i in range(1, order + 1):
                df_order_x: Scalar = scalars_db[f"df{i}"]
                res += f"order {i}\t"
                res += f"{df_order_x.value:{self.precision}g}\t"
                res += f"{df_order_x.std:{self.precision}g}\t"
                res += self.compute_tstat(df_order_x) + "\n"

            res += "\n"

            self.ui.textEdit_results.setPlainText(res)
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))

