import sys
from PyQt6.QtWidgets import QApplication, QSplashScreen
from PyQt6.QtGui import QPixmap
from PyQt6.QtCore import QLocale

from utils import ORGANIZATION_NAME
from main_window import MainWindow

def main():
    app = QApplication(sys.argv)
    app.setOrganizationName(ORGANIZATION_NAME)
    app.setApplicationName("IODE")

    files_to_load = sys.argv[1:]

    # force to use '.' as a decimal point
    loc = QLocale()
    loc.setNumberOptions(QLocale.c().numberOptions())
    QLocale.setDefault(loc)

    pixmap = QPixmap(":/images/iode_splash_screen.png")
    splash = QSplashScreen(pixmap)
    splash.show()
    app.processEvents()

    main_window = MainWindow(None, files_to_load)
    main_window.show()
    splash.finish(main_window)
    res = app.exec()
    return res

if __name__ == "__main__":
    sys.exit(main())
