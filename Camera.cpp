#include "Camera.h"
#include "defaults.h"

#include <iostream>
#include <mutex>
#include <condition_variable>

#define MAX_SEQS 2
class Camera::Private
{
public:
	Private();
	HIDS m_Cam = 0;
	SENSORINFO m_sInfo;
	HANDLE hEvent;
	double fps=1.0;
	int m_PixFormat = IS_CM_RGBA8_PACKED;
	int m_Bitspixel = 32;
	char* m_pcSeq[MAX_SEQS];
	int m_indSeq[MAX_SEQS];
	int m_seqs[MAX_SEQS];
	int fWidth = 3840;
	int fHeight = 1600;
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
	
	

	for (int i = 0; i<MAX_SEQS; i++){
		is_AllocImageMem(p_->m_Cam, p_->fWidth, p_->fHeight, p_->m_Bitspixel, &(p_->m_pcSeq[i]), &(p_->m_indSeq[i]));
		is_AddToSequence(p_->m_Cam, p_->m_pcSeq[i], p_->m_indSeq[i]);
		p_->m_seqs[i] = i + 1;
	}

	is_SetColorMode(p_->m_Cam, p_->m_PixFormat);

	IS_SIZE_2D imageSize;
	imageSize.s32Width = p_->fWidth;
	imageSize.s32Height = p_->fHeight;

	is_AOI(p_->m_Cam, IS_AOI_IMAGE_SET_SIZE, (void*)&imageSize, sizeof(imageSize));
	nRet = is_CaptureVideo(p_->m_Cam, IS_WAIT);
	UINT nPClock;
	
	nPClock = 368;//Can be incremented by 8
	nRet = is_PixelClock(p_->m_Cam, IS_PIXELCLOCK_CMD_SET, (void *)&nPClock, sizeof(nPClock));
	is_PixelClock(p_->m_Cam, IS_PIXELCLOCK_CMD_GET, (void *)&nPClock, sizeof(nPClock));
	is_SetFrameRate(p_->m_Cam, 30.0, &p_->fps);
	//is_SetColorConverter(p_->m_Cam, IS_CM_UYVY_PACKED, IS_CONV_MODE_OPENCL_3X3);
	p_->hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	is_InitEvent(p_->m_Cam, p_->hEvent, IS_SET_EVENT_FRAME);
	is_EnableEvent(p_->m_Cam, IS_SET_EVENT_FRAME);



	

}
void Camera::run(){
	int flag = 1;
	for (int frameNum = 0; frameNum < 1000; frameNum++){
		std::unique_lock<std::mutex> encLocker(encBufferMutex);
		while (encoderBusy>1)
			encBufferCond.wait(encLocker);
		//Now I can flip
		flag = flag ^ 1;

		//cameraBusy = true;
		encLocker.unlock();
		//Processing
		DWORD dwRet = WaitForSingleObject(p_->hEvent, 1000);
		if (dwRet == WAIT_TIMEOUT){
			std::cout << "Waited too long for the frame\n";
			continue;
		}


		int nNum, i, nRet;
		char *pcMem, *pcMemLast;


		is_GetActSeqBuf(p_->m_Cam, &nNum, &pcMem, &pcMemLast);

		is_GetFramesPerSecond(p_->m_Cam, &p_->fps);

		for (i = 0; i<MAX_SEQS; i++)
		{
			if (pcMemLast == p_->m_pcSeq[i])
				break;
		}


		// lock buffer for processing
		nRet = is_LockSeqBuf(p_->m_Cam, p_->m_seqs[i], p_->m_pcSeq[i]);

		//// start processing...................................
		memcpy(buffer[flag], p_->m_pcSeq[i], p_->fWidth*p_->fHeight * 4);

		if (p_->fps < 30)
			std::cout << "Getting frame rate" << p_->fps << std::endl;
		//std::cout << "Getting pixel clock" << nNum << std::endl;
		//// processing completed................................

		// unlock buffer
		is_UnlockSeqBuf(p_->m_Cam, p_->m_seqs[i], p_->m_pcSeq[i]);
		

		

		std::unique_lock<std::mutex> camLocker(camBufferMutex);
		encoderBusy++;
		cameraBusy--;
		camLocker.unlock();
		camBufferCond.notify_one();
		
		
	}
}

std::thread Camera::runThread(){
	return std::thread([=] {run(); });
}

void Camera::close(){
	is_StopLiveVideo(p_->m_Cam, IS_WAIT);
}