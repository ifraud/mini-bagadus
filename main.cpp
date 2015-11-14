#include <QCoreApplication>
#include <iostream>
#include <uEye.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    int numDevs;

    is_GetNumberOfCameras(&numDevs);
    std::cout<<"Hellow how are "<<numDevs<<std::endl;

    return a.exec();
}

