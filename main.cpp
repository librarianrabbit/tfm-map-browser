#include <QtGui/QApplication>
#include "mapbrowser.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MapBrowser w;
    w.show();

    return a.exec();
}
