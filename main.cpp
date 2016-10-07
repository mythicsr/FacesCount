#include "facescount.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FacesCount w;
    w.show();
    return a.exec();
}
