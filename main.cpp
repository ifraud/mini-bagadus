#include <QCoreApplication>
#include <iostream>
#include <uEye.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#define MAX_SEQS 500

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    int numDevs;

    is_GetNumberOfCameras(&numDevs);
    std::cout<<"The number of cameras are "<<numDevs<<std::endl;

    HIDS m_Cam = 0;
    SENSORINFO m_sInfo;

    char* m_pcSeq[MAX_SEQS];
    int m_indSeq[MAX_SEQS];
    int m_seqs[MAX_SEQS];
    int fWidth = 3840;
    int fHeight = 1200;
    cv::Mat image(fHeight, fWidth, CV_8UC3, cv::Scalar(0,0,255));


    int nRet = is_InitCamera(&m_Cam, NULL);
    if(nRet == IS_SUCCESS){
        //Camera Opened succesfully
        is_GetSensorInfo(m_Cam,&m_sInfo);
        std::cout<<"The sensor is "<<m_sInfo.strSensorName<<std::endl;
        std::cout<<"The max width is"<<m_sInfo.nMaxWidth<<std::endl;
        std::cout<<"The max height is"<<m_sInfo.nMaxHeight<<std::endl;
        int m_PixFormat = IS_CM_RGB8_PACKED;
        int m_Bitspixel = 24;
        for(int i=0;i<MAX_SEQS;i++){
            is_AllocImageMem(m_Cam,fWidth,fHeight, m_Bitspixel,&m_pcSeq[i], &m_indSeq[i]);
            is_AddToSequence(m_Cam, m_pcSeq[i],m_indSeq[i]);
            m_seqs[i] = i+1;
        }

        is_SetColorMode(m_Cam, m_PixFormat);

        IS_SIZE_2D imageSize;
        imageSize.s32Width = fWidth;
        imageSize.s32Height = fHeight;

        is_AOI(m_Cam, IS_AOI_IMAGE_SET_SIZE, (void*)&imageSize, sizeof(imageSize));
        nRet = is_CaptureVideo(m_Cam, IS_WAIT);

        for(int j=0;j<1000;j++){

            std::cout<<"The image is captured succesfully "<<nRet<<std::endl;
            int nNum, i;
            char *pcMem, *pcMemLast;
            is_GetActSeqBuf(m_Cam, &nNum, &pcMem, &pcMemLast);
            double fps;
            is_GetFramesPerSecond(m_Cam, &fps);
            UINT nPClock;
            nPClock = 272;
            nRet = is_PixelClock(m_Cam, IS_PIXELCLOCK_CMD_SET, (void *)&nPClock, sizeof(nPClock));
            std::cout<<"Setting clock with"<<nRet<<std::endl;
            is_PixelClock(m_Cam, IS_PIXELCLOCK_CMD_GET, (void *)&nPClock, sizeof(nPClock));


            is_SetFrameRate(m_Cam, 40.0, &fps);
            for( i=0 ; i<MAX_SEQS ; i++)
            {
                if( pcMemLast == m_pcSeq[i] )
                break;
            }


            // lock buffer for processing
            nRet = is_LockSeqBuf( m_Cam,m_seqs[i], m_pcSeq[i] );

            //// start processing...................................

            // display buffer
            std::cout<<"Getting frame rate"<<fps<<std::endl;
            std::cout<<"Getting pixel clock"<<nPClock<<std::endl;
            memcpy(image.data, m_pcSeq[i], fWidth*fHeight*3);
            //cv::Mat dispImag;
            //cv::resize(image, dispImag,cvSize(0,0), 0.1, 0.1, cv::INTER_LINEAR);
            //imshow("Display Window", dispImag);
            //cv::waitKey(10);

            //// processing completed................................

            // unlock buffer
            is_UnlockSeqBuf( m_Cam, m_seqs[i], m_pcSeq[i] );


        }


        is_StopLiveVideo(m_Cam, IS_WAIT);






    }






    return a.exec();
}

