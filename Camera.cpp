#include "Camera.h"

#include <iostream>

#define MAX_SEQS 2
class Camera::Private
{
public:
	Private();
	HIDS m_Cam = 0;
	SENSORINFO m_sInfo;
};
Camera::Private::Private(){

}
Camera::Camera() : p_(new Private()){

}

Camera::~Camera() = default;

void Camera::init(){

	int nRet = is_InitCamera(&(p_->m_Cam), NULL);
	is_GetSensorInfo(p_->m_Cam, &(p_->m_sInfo));
	std::cout << "The sensor is " << p_->m_sInfo.strSensorName << std::endl;
	std::cout << "The max width is" << p_->m_sInfo.nMaxWidth << std::endl;
	std::cout << "The max height is" << p_->m_sInfo.nMaxHeight << std::endl;
	int m_PixFormat = IS_CM_RGB8_PACKED;
	int m_Bitspixel = 24;
	char* m_pcSeq[MAX_SEQS];
	int m_indSeq[MAX_SEQS];
	int m_seqs[MAX_SEQS];
	int fWidth = 3840;
	int fHeight = 2160;
	char *buffer = new char[3840*2160*3];
	for (int i = 0; i<MAX_SEQS; i++){
		is_AllocImageMem(p_->m_Cam, fWidth, fHeight, m_Bitspixel, &m_pcSeq[i], &m_indSeq[i]);
		is_AddToSequence(p_->m_Cam, m_pcSeq[i], m_indSeq[i]);
		m_seqs[i] = i + 1;
	}

	is_SetColorMode(p_->m_Cam, m_PixFormat);

	IS_SIZE_2D imageSize;
	imageSize.s32Width = fWidth;
	imageSize.s32Height = fHeight;

	is_AOI(p_->m_Cam, IS_AOI_IMAGE_SET_SIZE, (void*)&imageSize, sizeof(imageSize));
	nRet = is_CaptureVideo(p_->m_Cam, IS_WAIT);

	for (int j = 0; j<100; j++){

		int nNum, i;
		char *pcMem, *pcMemLast;
		is_GetActSeqBuf(p_->m_Cam, &nNum, &pcMem, &pcMemLast);
		double fps;
		is_GetFramesPerSecond(p_->m_Cam, &fps);
		UINT nPClock;
		nPClock = 344;
		nRet = is_PixelClock(p_->m_Cam, IS_PIXELCLOCK_CMD_SET, (void *)&nPClock, sizeof(nPClock));
		is_PixelClock(p_->m_Cam, IS_PIXELCLOCK_CMD_GET, (void *)&nPClock, sizeof(nPClock));


		is_SetFrameRate(p_->m_Cam, 30.0, &fps);
		for (i = 0; i<MAX_SEQS; i++)
		{
			if (pcMemLast == m_pcSeq[i])
				break;
		}


		// lock buffer for processing
		nRet = is_LockSeqBuf(p_->m_Cam, m_seqs[i], m_pcSeq[i]);

		//// start processing...................................
		double start = GetTickCount();
		memcpy(buffer, m_pcSeq[i], fWidth*fHeight * 3);
		double end = GetTickCount();
		std::cout << "The time for copy" << end - start << "\n";
		//Sleep(30);
		std::cout << "Getting frame rate" << fps << std::endl;
		std::cout << "Getting pixel clock" << i << std::endl;
		//// processing completed................................

		// unlock buffer
		is_UnlockSeqBuf(p_->m_Cam, m_seqs[i], m_pcSeq[i]);


	}


	is_StopLiveVideo(p_->m_Cam, IS_WAIT);






}