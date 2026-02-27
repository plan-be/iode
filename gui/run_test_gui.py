from pathlib import Path
import qtpy
from iode_gui import start


if __name__ == "__main__":
    print("Starting IODE GUI...")
    print("qtpy version:", qtpy.__version__)
    print("Qt version:", qtpy.QT_VERSION)
    print("Qt API name:", qtpy.API_NAME)
    start()
