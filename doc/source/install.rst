Installation
============

PyPI
----

To install the latest stable version of the *iode* and *iode-gui* packages, use ::

    $ pip install iode iode-gui


To test the installation, type ::

    $ python

Then ::

    >>> import iode
    >>> iode.__version__
    '<iode version>'
    >>> import iode_gui
    >>> iode_gui.__version__
    '<iode-gui version>'


Required Dependencies
---------------------

iode
~~~~

- Python 3.10, 3.11 or 3.12
- `numpy <http://www.numpy.org/>`__
- `pandas <http://pandas.pydata.org/>`__
- `matplotlib <http://matplotlib.org/>`__

iode-gui
~~~~~~~~

- iode
- pyside6 
- qtconsole
- ipython


Optional Dependencies
---------------------

- `larray <https://github.com/larray-project/larray>`__:
  N-dimensional labelled arrays in Python (developed by the Federal Planning Bureau in Belgium) 
