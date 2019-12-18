#include <QApplication>
#include "serverwindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // TODO: Seperate the graphical interface from the data
    // Server must Be a singleton
    ServerWindow fenetre;
    fenetre.show();

    return app.exec();
}
