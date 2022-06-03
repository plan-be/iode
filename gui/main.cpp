#include <QtWidgets/QApplication>

#include "utils.h"
#include "super.h"
#include "main_window.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
    QCoreApplication::setApplicationName("IODE");

    gui_assign_super_API();

    MainWindow main_window;
    main_window.show();
    return app.exec();
}
