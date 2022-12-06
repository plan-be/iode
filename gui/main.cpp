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

    gui_assign_super_API();

    // For the path given in pixmap, see "The Qt Resource System"
    // https://doc.qt.io/qt-6/resources.html#qt-resource-collection-file-qrc
    QPixmap pixmap(":/images/iode_splash_screen.png");
    // see QSplashScreen documentation:
    // https://doc.qt.io/qt-6/qsplashscreen.html#details
    QSplashScreen splash(pixmap);
    splash.show();
    app.processEvents();

    MainWindow main_window;
    main_window.show();
    splash.finish(&main_window);
    return app.exec();
}
