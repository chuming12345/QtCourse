#include "calculator.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Calculator w;
    w.resize(300, 360);
    w.show();
    return a.exec();
}
