# iode-gui - Graphical User Interface for IODE

The `iode-gui` package provides a graphical user interface for the `IODE` tool. `IODE` is the French 
acronym for `Intégrateur d'Outils de Développements Économétriques` (Integrated Tools for Econometric Developments).

Originally designed at the `Federal Planning Bureau` in Brussels by econometricians and mathematicians, this software integrates all the 
functionalities required for the construction and the handling of econometric models and for the management of statistical series.

## Installation

To install `iode-gui`, you can use `pip`:
```bash
pip install iode-gui
```

## Dependencies

`iode-gui` depends on the following packages:
  - `numpy`: A library for numerical computing in Python, which is used by `iode` for mathematical operations.
  - `pandas`: A library for data manipulation and analysis in Python, which is used by `iode` for handling statistical series.
  - `matplotlib`: A library for creating static, animated, and interactive visualizations in Python, which is used by `iode` for plotting graphs and charts.
  - `iode`: The core package of IODE, which provides the underlying functionality for handling econometric models and statistical series.
  - `pyqt` or `pyside6`: A set of Python bindings for the Qt framework, which is used to create the graphical user interface of `iode-gui`.
  - `qtconsole`: A rich Jupyter console widget that can be embedded in a Python Qt application, which is used to provide an interactive Python shell within `iode-gui`.
  - `ipython`: A powerful interactive shell for Python, which is used by `qtconsole` to provide the interactive Python shell within `iode-gui`.

Optionally, `iode-gui` can also use the following packages for additional functionalities:
  - `larray`: A library for handling N-dimensional labelled arrays in Python (developed by the Federal Planning Bureau in Belgium).

## IODE Documentation

The documentation for `IODE` is available online at:
https://iode.readthedocs.io/en/stable/index.html

## Support
"Alix Damman" <ald@plan.be>, "Yannick Van den Abbeel" <yvda@plan.be>
