#include <QApplication>
#include "Program2Window.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Program2Window w;
    w.show();
    return app.exec();
}
