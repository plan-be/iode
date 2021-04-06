#include <QApplication>
#include <QMainWindow>

#include <string>

#include "api/iode.h"
#include "utils.h"
#include "main_window.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
    QCoreApplication::setApplicationName(APPLICATION_NAME);

    MainWindow main_window;
    main_window.show();
    return app.exec();
}