#include <QCoreApplication>
#include <iostream>
#include <uEye.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    int numDevs;

    is_GetNumberOfCameras(&numDevs);
    std::cout<<"The number of cameras are "<<numDevs<<std::endl;

    HIDS m_Cam = 0;
    SENSORINFO m_sInfo;
    int nRet = is_InitCamera(&m_Cam, NULL);
    if(nRet == IS_SUCCESS){
        //Camera Opened succesfully
        is_GetSensorInfo(m_Cam,&m_sInfo);
        std::cout<<"The sensor is "<<m_sInfo.strSensorName<<std::endl;
    }


    return a.exec();
}

