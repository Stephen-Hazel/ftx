#include "ftx.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FTx w;
    w.show();
    return a.exec();
}
