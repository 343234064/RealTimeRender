#include "CrashReportClient.h"
#include <QtWidgets/QApplication>


#include <iostream>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CrashReportClient w;
    w.show();


    return a.exec();
}
