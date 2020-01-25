#include <QApplication>
#include "widget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.addLibraryPath("plugins/");
    Widget w;
    w.show();
    return a.exec();
}
