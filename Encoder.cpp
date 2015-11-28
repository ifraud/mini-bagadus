#include "defaults.h"
#include "Encoder.h"

#include <iostream>
#include <windows.h>
#include <stdio.h>

#include <mfxvideo++.h>
#include "common_utils.h"

class Encoder::Private
{
public:
	Private();
	mfxStatus sts = MFX_ERR_NONE;
	MFXVideoSession session;

	mfxIMPL impl = MFX_IMPL_HARDWARE;

	mfxFrameAllocator mfxAllocator;
	// Initialize encoder parameters
	mfxVideoParam mfxEncParams;

	// Initialize VPP parameters
	mfxVideoParam VPPParams;
	// Create Media SDK encoder
	MFXVideoENCODE *mfxENC;
	// Create Media SDK VPP component
	MFXVideoVPP *mfxVPP;

};
Encoder::Private::Private(){



	
}
Encoder::Encoder() : p_(new Private()){
	
}

Encoder::~Encoder() = default;

void Encoder::init(){
	mfxVersion ver = { { 0, 1 } };
	p_->sts= Initialize(p_->impl, ver, &(p_->session), &(p_->mfxAllocator));
	memset(&p_->mfxEncParams, 0, sizeof(p_->mfxEncParams));
	p_->mfxEncParams.mfx.CodecId = MFX_CODEC_AVC;
	p_->mfxEncParams.mfx.TargetUsage = MFX_TARGETUSAGE_BALANCED;
	p_->mfxEncParams.mfx.TargetKbps = 12000;
	p_->mfxEncParams.mfx.RateControlMethod = MFX_RATECONTROL_VBR;
	p_->mfxEncParams.mfx.FrameInfo.FrameRateExtN = 30;
	p_->mfxEncParams.mfx.FrameInfo.FrameRateExtD = 1;
	p_->mfxEncParams.mfx.FrameInfo.FourCC = MFX_FOURCC_NV12;
	p_->mfxEncParams.mfx.FrameInfo.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
	p_->mfxEncParams.mfx.FrameInfo.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
	p_->mfxEncParams.mfx.FrameInfo.CropX = 0;
	p_->mfxEncParams.mfx.FrameInfo.CropY = 0;
	p_->mfxEncParams.mfx.FrameInfo.CropW = 3840;
	p_->mfxEncParams.mfx.FrameInfo.CropH = 2160;
	// Width must be a multiple of 16
	// Height must be a multiple of 16 in case of frame picture and a multiple of 32 in case of field picture
	p_->mfxEncParams.mfx.FrameInfo.Width = 3840;
	p_->mfxEncParams.mfx.FrameInfo.Height = 2160;

	p_->mfxEncParams.IOPattern = MFX_IOPATTERN_IN_VIDEO_MEMORY;

	memset(&p_->VPPParams, 0, sizeof(p_->VPPParams));
	// Input data
	p_->VPPParams.vpp.In.FourCC = MFX_FOURCC_RGB4;
	p_->VPPParams.vpp.In.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
	p_->VPPParams.vpp.In.CropX = 0;
	p_->VPPParams.vpp.In.CropY = 0;
	p_->VPPParams.vpp.In.CropW = 3840;
	p_->VPPParams.vpp.In.CropH = 2160;
	p_->VPPParams.vpp.In.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
	p_->VPPParams.vpp.In.FrameRateExtN = 30;
	p_->VPPParams.vpp.In.FrameRateExtD = 1;
	// width must be a multiple of 16
	// height must be a multiple of 16 in case of frame picture and a multiple of 32 in case of field picture
	p_->VPPParams.vpp.In.Width = 3840;
	p_->VPPParams.vpp.In.Height = 2160;
	// Output data
	p_->VPPParams.vpp.Out.FourCC = MFX_FOURCC_NV12;
	p_->VPPParams.vpp.Out.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
	p_->VPPParams.vpp.Out.CropX = 0;
	p_->VPPParams.vpp.Out.CropY = 0;
	p_->VPPParams.vpp.Out.CropW = 3840;
	p_->VPPParams.vpp.Out.CropH = 2160;
	p_->VPPParams.vpp.Out.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
	p_->VPPParams.vpp.Out.FrameRateExtN = 30;
	p_->VPPParams.vpp.Out.FrameRateExtD = 1;
	// width must be a multiple of 16
	// height must be a multiple of 16 in case of frame picture and a multiple of 32 in case of field picture
	p_->VPPParams.vpp.Out.Width = p_->VPPParams.vpp.Out.CropW;
	p_->VPPParams.vpp.Out.Height = p_->VPPParams.vpp.Out.CropH;

	p_->VPPParams.IOPattern = MFX_IOPATTERN_IN_VIDEO_MEMORY | MFX_IOPATTERN_OUT_VIDEO_MEMORY;
	p_->mfxENC = new MFXVideoENCODE(p_->session);
	p_->mfxVPP = new MFXVideoVPP(p_->session);


}
void Encoder::run(){
	int flag = 1;
	FILE* pFile;
	fopen_s(&pFile, "file.bin", "wb");

	for (int i = 0; i < 100; i++){
		std::unique_lock<std::mutex> camLocker(camBufferMutex);
		while (cameraBusy>1)
			camBufferCond.wait(camLocker);
		//Now I can flip
		flag = flag ^ 1;
		//encoderBusy = true;
		camLocker.unlock();
	
		//Process
		std::cout << "Processing in Encoder "<<flag<<"\n";
		fwrite(buffer[flag],1, 3840 * 1600 * 4, pFile);
		

		std::unique_lock<std::mutex> encLocker(encBufferMutex);
		cameraBusy++;
		encoderBusy--;
		encLocker.unlock();
		encBufferCond.notify_one();
		

	}
	fclose(pFile);
}

std::thread Encoder::runThread(){
	return std::thread([=] {run(); });
}