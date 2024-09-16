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

    // check if files have been passed as argument to the application.
    // This happens when the user selects several files from a File Directory 
    // (Windows, Linux, Mac) and choose the option "Open With".
    QVector<QString> files_to_load;
    if(argc > 1)
        for(int i=1; i < argc; i++)
            files_to_load.append(QString(argv[i]));

    // forces decimals separator to be dot 
    QLocale::setDefault(QLocale::English);

    // initialize IODE API
    cpp_iode_init();
    gui_assign_super_API();
    kpause_continue = 1;

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

    MainWindow* main_window = new MainWindow(nullptr, files_to_load);
    main_window->show();
    splash.finish(main_window);
    int res = app.exec();

    // WARNING: we need to call IodeEnd() AFTER the destructor of MainWindow 
    //          to avoid heap-use-after-free error
    delete main_window;
    IodeEnd();

    return res;
}
