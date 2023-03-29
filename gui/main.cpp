#include <QtWidgets/QApplication>
#include <QPixmap>
#include <QSplashScreen>

#include "utils.h"
#include "super.h"
#include "main_window.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
    QCoreApplication::setApplicationName("IODE");

    // forces decimals separator to be dot 
    QLocale::setDefault(QLocale::English);

    // initialize IODE API
    IodeInit();
    gui_assign_super_API();

    // set executable path
    executable_dir = QDir::currentPath();
    url_manual = QUrl::fromLocalFile(executable_dir + "/doc/iode.chm");
    url_readme = QUrl::fromLocalFile(executable_dir + "/doc/readme.htm");
    url_shortcuts = QUrl::fromLocalFile(executable_dir + "/doc/keyboard_shortcuts.pdf");

    // For the path given in pixmap, see "The Qt Resource System"
    // https://doc.qt.io/qt-6/resources.html#qt-resource-collection-file-qrc
    QPixmap pixmap(":/images/iode_splash_screen.png");
    // see QSplashScreen documentation:
    // https://doc.qt.io/qt-6/qsplashscreen.html#details
    QSplashScreen splash(pixmap);
    splash.show();
    app.processEvents();

    MainWindow* main_window = new MainWindow();
    main_window->show();
    splash.finish(main_window);
    int res = app.exec();

    // WARNING: we need to call IodeEnd() AFTER the destructor of MainWindow 
    //          to avoid heap-use-after-free error
    delete main_window;
    IodeEnd();

    return res;
}
