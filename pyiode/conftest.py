import pytest
try:
    import larray as la
except ImportError:
    la = None

deselect_items = [
    "iode.computed_table.computed_table.ComputedTable.plotting_series_values",
    "iode.computed_table.computed_table.ComputedTable.to_array",
    "variables_database.Variables.__add__",
    "variables_database.Variables.__iadd__",
    "variables_database.Variables.__imul__",
    "variables_database.Variables.__isub__",
    "variables_database.Variables.__itruediv__",
    "variables_database.Variables.__mul__",
    "variables_database.Variables.__pow__",
    "variables_database.Variables.__radd__",
    "variables_database.Variables.__rmul__",
    "variables_database.Variables.__rsub__",
    "variables_database.Variables.__rtruediv__",
    "variables_database.Variables.__setitem__",
    "variables_database.Variables.__sub__",
    "variables_database.Variables.__truediv__",
    "variables_database.Variables.from_array",
    "variables_database.Variables.from_numpy",
    "variables_database.Variables.to_array",
    ]


def pytest_collection_modifyitems(config, items):
    skip = pytest.mark.skip(reason="larray is not installed")
    for item in items:
        if la is None and item.name in deselect_items:
            item.add_marker(skip)
