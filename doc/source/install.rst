Installation
============

Wheel File
----------

Currently, the only supported way to install IODE is by using a *wheel* file. 
A *wheel* file is essentially a *zip* file that contains all the necessary files 
to install a Python package. 

To install the Python package for IODE, you have to first download the *wheel* file 
`iode-<iode version>-cp<python version>-<os>-<architecture>.whl` from the last release 
from the `release page <https://github.com/plan-be/iode/releases>`__. 

Then, open a terminal and go to the folder where you have downloaded the *wheel* file. 
To install the *iode* package, run the following command ::

    pip install iode-<iode version>-cp<python_version>-<os>-<architecture>.whl

To test the installation, type ::

    python

Then ::

    >>> import iode
    >>> iode.__version__
    '<iode version>'


Required Dependencies
---------------------

- Python 3.10, 3.11 or 3.12
- `numpy <http://www.numpy.org/>`__
- `pandas <http://pandas.pydata.org/>`__
- `matplotlib <http://matplotlib.org/>`__


Optional Dependencies
---------------------

- `larray <https://github.com/larray-project/larray>`__:
  N-dimensional labelled arrays in Python (developed by the Federal Planning Bureau in Belgium) 
