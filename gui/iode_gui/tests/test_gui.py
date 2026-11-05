import pytest
import pytestqt

from qtpy.QtCore import Qt, QItemSelection, QItemSelectionModel
from qtpy.QtWidgets import QMessageBox, QApplication

from iode import variables, VarsMode
from iode_gui.iode_objs.models.table_model import VariablesModel
from iode_gui.iode_objs.views.table_view import VariablesView


@pytest.fixture
def variables_view(qtbot):
    # small, known Variables workspace
    variables.clear()
    variables.sample = "2000Y1:2005Y1"
    variables["A"] = 0.0
    variables["B"] = 0.0

    view = VariablesView()
    qtbot.addWidget(view)
    model = VariablesModel(precision=2, format='f', mode=VarsMode.LEVEL, parent=view)
    view.setModel(model)

    yield view

    variables.clear()


def test_variables_view_paste_special(variables_view, monkeypatch):
    view: VariablesView = variables_view

    # record warnings instead of opening a blocking message box
    warnings = []
    monkeypatch.setattr(QMessageBox, "warning", lambda *args, **kwargs: warnings.append(args))

    # the sample dialog must not be opened when the columns represent periods
    def fail_ask_sample():
        raise AssertionError("_ask_sample() should not have been called")
    monkeypatch.setattr(view, "_ask_sample", fail_ask_sample)

    # ---- columns = contiguous periods ----
    view._paste_special(["A", "B"], ["2001Y1", "2002Y1", "2003Y1"], [[1.0, 2.0, 3.0], [4.0, 5.0, 6.0]])
    assert [variables["A", p] for p in ["2000Y1", "2001Y1", "2002Y1", "2003Y1", "2004Y1"]] == [0.0, 1.0, 2.0, 3.0, 0.0]
    assert [variables["B", p] for p in ["2000Y1", "2001Y1", "2002Y1", "2003Y1", "2004Y1"]] == [0.0, 4.0, 5.0, 6.0, 0.0]

    # ---- columns = integers (yearly periods) + surrounding spaces ----
    view._paste_special(["A", "B"], [" 2004", "2005 "], [[7.0, 8.0], [9.0, 10.0]])
    assert [variables["A", p] for p in ["2004Y1", "2005Y1"]] == [7.0, 8.0]
    assert [variables["B", p] for p in ["2004Y1", "2005Y1"]] == [9.0, 10.0]

    # ---- columns = non-contiguous periods ----
    view._paste_special(["A", "B"], ["2000", "2002Y1"], [[11.0, 12.0], [13.0, 14.0]])
    assert [variables["A", p] for p in ["2000Y1", "2001Y1", "2002Y1"]] == [11.0, 1.0, 12.0]
    assert [variables["B", p] for p in ["2000Y1", "2001Y1", "2002Y1"]] == [13.0, 4.0, 14.0]
    assert not warnings

    # ---- columns are not periods -> the user is asked for the sample ----
    monkeypatch.setattr(view, "_ask_sample", lambda: ("2001Y1", "2003Y1"))
    view._paste_special(["A", "B"], ["1.5", "2.5", "3.5"], [[21.0, 22.0, 23.0], [24.0, 25.0, 26.0]])
    assert [variables["A", p] for p in ["2001Y1", "2002Y1", "2003Y1"]] == [21.0, 22.0, 23.0]
    assert [variables["B", p] for p in ["2001Y1", "2002Y1", "2003Y1"]] == [24.0, 25.0, 26.0]
    assert not warnings

    # ---- columns are not periods + the user cancels the sample dialog -> nothing pasted ----
    monkeypatch.setattr(view, "_ask_sample", lambda: None)
    view._paste_special(["A"], ["1.5", "2.5", "3.5"], [[31.0, 32.0, 33.0]])
    assert [variables["A", p] for p in ["2001Y1", "2002Y1", "2003Y1"]] == [21.0, 22.0, 23.0]
    assert not warnings

    # ---- columns are not periods + sample size != number of values -> warning, nothing pasted ----
    monkeypatch.setattr(view, "_ask_sample", lambda: ("2001Y1", "2002Y1"))
    view._paste_special(["A"], ["1.5", "2.5", "3.5"], [[41.0, 42.0, 43.0]])
    assert len(warnings) == 1
    assert "2001Y1:2002Y1" in warnings[0][2]
    assert [variables["A", p] for p in ["2001Y1", "2002Y1", "2003Y1"]] == [21.0, 22.0, 23.0]


def test_variables_view_paste(variables_view, qtbot, monkeypatch):
    view: VariablesView = variables_view
    model: VariablesModel = view.model()

    warnings = []
    monkeypatch.setattr(QMessageBox, "warning", lambda *args, **kwargs: warnings.append(args))

    # select the block A[2001Y1:2002Y1] - B[2001Y1:2002Y1] (paste starts from the top-left cell)
    selection = QItemSelection(model.index(0, 1), model.index(1, 2))
    view.selectionModel().select(selection, QItemSelectionModel.SelectionFlag.Select)

    # ---- values copied from Excel (tab-separated, '\r\n' line endings) ----
    # NOTE: the first value is equal to the current value (0) -> the paste must not stop
    QApplication.clipboard().setText("0\t2\r\n3\t4\r\n")
    with qtbot.waitSignal(model.dataChanged, timeout=1000):
        view.paste()
    assert not warnings
    assert [variables["A", p] for p in ["2000Y1", "2001Y1", "2002Y1", "2003Y1"]] == [0.0, 0.0, 2.0, 0.0]
    assert [variables["B", p] for p in ["2000Y1", "2001Y1", "2002Y1", "2003Y1"]] == [0.0, 3.0, 4.0, 0.0]
    assert model.data(model.index(1, 2), Qt.ItemDataRole.DisplayRole) == "4.00"

    # ---- more values than available columns -> warning ----
    QApplication.clipboard().setText("\t".join(["1"] * 10))
    view.paste()
    assert len(warnings) == 1
    assert "Invalid index" in warnings[0][2]
