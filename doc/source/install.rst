Installation
============

Conda
-----
The preferred way to install the *iode* and *iode-gui* packages is through the `conda <https://docs.conda.io/en/latest/>`__ package manager.

To install the latest stable version of the two packages, use the following command in a terminal::

    $ conda install --channel plan-be iode iode-gui


Installing the *iode-gui* package through conda will also add two entries in your Windows Start Menu:

- **IODE > IODE Documentation**: opens the IODE documentation in your default web browser.
- **IODE > IODE Editor**: opens the IODE graphical user interface.


PyPI
----

To install the latest stable version of the *iode* and *iode-gui* packages, use ::

    $ pip install iode iode-gui

NOTE: Installing *iode-gui* through pip will not add any entries in your Windows Start Menu.


Testing the installation
------------------------

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
