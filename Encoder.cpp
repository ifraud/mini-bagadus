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

	mfxU16 nSurfNumVPPIn;
	mfxU16 nSurfNumVPPOutEnc;

	mfxFrameSurface1** pmfxSurfacesVPPIn;
	mfxFrameSurface1** pVPPSurfacesVPPOutEnc;

	mfxExtVPPDoNotUse extDoNotUse;
	mfxExtBuffer* extBuffers[1];

	mfxBitstream mfxBS;
	mfxTime tStart, tEnd;


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
	p_->mfxEncParams.mfx.TargetUsage = MFX_TARGETUSAGE_BEST_SPEED;
	p_->mfxEncParams.mfx.TargetKbps = 6000;
	p_->mfxEncParams.mfx.RateControlMethod = MFX_RATECONTROL_VBR;
	p_->mfxEncParams.mfx.FrameInfo.FrameRateExtN = 30;
	p_->mfxEncParams.mfx.FrameInfo.FrameRateExtD = 1;
	p_->mfxEncParams.mfx.FrameInfo.FourCC = MFX_FOURCC_NV12;
	p_->mfxEncParams.mfx.FrameInfo.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
	p_->mfxEncParams.mfx.FrameInfo.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
	p_->mfxEncParams.mfx.FrameInfo.CropX = 0;
	p_->mfxEncParams.mfx.FrameInfo.CropY = 0;
	p_->mfxEncParams.mfx.FrameInfo.CropW = 3840;
	p_->mfxEncParams.mfx.FrameInfo.CropH = 1600;
	// Width must be a multiple of 16
	// Height must be a multiple of 16 in case of frame picture and a multiple of 32 in case of field picture
	p_->mfxEncParams.mfx.FrameInfo.Width = 3840;
	p_->mfxEncParams.mfx.FrameInfo.Height = 1600;

	p_->mfxEncParams.IOPattern = MFX_IOPATTERN_IN_VIDEO_MEMORY;

	memset(&p_->VPPParams, 0, sizeof(p_->VPPParams));
	// Input data
	p_->VPPParams.vpp.In.FourCC = MFX_FOURCC_RGB4;
	p_->VPPParams.vpp.In.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
	p_->VPPParams.vpp.In.CropX = 0;
	p_->VPPParams.vpp.In.CropY = 0;
	p_->VPPParams.vpp.In.CropW = 3840;
	p_->VPPParams.vpp.In.CropH = 1600;
	p_->VPPParams.vpp.In.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
	p_->VPPParams.vpp.In.FrameRateExtN = 30;
	p_->VPPParams.vpp.In.FrameRateExtD = 1;
	// width must be a multiple of 16
	// height must be a multiple of 16 in case of frame picture and a multiple of 32 in case of field picture
	p_->VPPParams.vpp.In.Width = 3840;
	p_->VPPParams.vpp.In.Height = 1600;
	// Output data
	p_->VPPParams.vpp.Out.FourCC = MFX_FOURCC_NV12;
	p_->VPPParams.vpp.Out.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
	p_->VPPParams.vpp.Out.CropX = 0;
	p_->VPPParams.vpp.Out.CropY = 0;
	p_->VPPParams.vpp.Out.CropW = 3840;
	p_->VPPParams.vpp.Out.CropH = 1600;
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
	
	// Query number of required surfaces for encoder
	mfxFrameAllocRequest EncRequest;
	memset(&EncRequest, 0, sizeof(EncRequest));
	p_->sts = p_->mfxENC->QueryIOSurf(&p_->mfxEncParams, &EncRequest);


	// Query number of required surfaces for VPP
	mfxFrameAllocRequest VPPRequest[2];     // [0] - in, [1] - out
	memset(&VPPRequest, 0, sizeof(mfxFrameAllocRequest) * 2);
	p_->sts = p_->mfxVPP->QueryIOSurf(&p_->VPPParams, VPPRequest);

	EncRequest.Type |= MFX_MEMTYPE_FROM_VPPOUT;     // surfaces are shared between VPP output and encode input

	// Determine the required number of surfaces for VPP input and for VPP output (encoder input)
	p_->nSurfNumVPPIn = VPPRequest[0].NumFrameSuggested;
	p_->nSurfNumVPPOutEnc = EncRequest.NumFrameSuggested + VPPRequest[1].NumFrameSuggested;

	EncRequest.NumFrameSuggested = p_->nSurfNumVPPOutEnc;

	VPPRequest[0].Type |= WILL_WRITE; // This line is only required for Windows DirectX11 to ensure that surfaces can be written to by the application

	// Allocate required surfaces
	mfxFrameAllocResponse mfxResponseVPPIn;
	mfxFrameAllocResponse mfxResponseVPPOutEnc;
	p_->sts = p_->mfxAllocator.Alloc(p_->mfxAllocator.pthis, &VPPRequest[0], &mfxResponseVPPIn);
	p_->sts = p_->mfxAllocator.Alloc(p_->mfxAllocator.pthis, &EncRequest, &mfxResponseVPPOutEnc);


	// Allocate surface headers (mfxFrameSurface1) for VPPIn
	p_->pmfxSurfacesVPPIn = new mfxFrameSurface1 *[p_->nSurfNumVPPIn];
	for (int i = 0; i < p_->nSurfNumVPPIn; i++) {
		p_->pmfxSurfacesVPPIn[i] = new mfxFrameSurface1;
		memset(p_->pmfxSurfacesVPPIn[i], 0, sizeof(mfxFrameSurface1));
		memcpy(&(p_->pmfxSurfacesVPPIn[i]->Info), &(p_->VPPParams.vpp.In), sizeof(mfxFrameInfo));
		p_->pmfxSurfacesVPPIn[i]->Data.MemId = mfxResponseVPPIn.mids[i];
		ClearRGBSurfaceVMem(p_->pmfxSurfacesVPPIn[i]->Data.MemId);
	}

	//Allocate surface headers for VPPOut
	p_->pVPPSurfacesVPPOutEnc = new mfxFrameSurface1 *[p_->nSurfNumVPPOutEnc];
	for (int i = 0; i < p_->nSurfNumVPPOutEnc; i++) {
		p_->pVPPSurfacesVPPOutEnc[i] = new mfxFrameSurface1;
		memset(p_->pVPPSurfacesVPPOutEnc[i], 0, sizeof(mfxFrameSurface1));
		memcpy(&(p_->pVPPSurfacesVPPOutEnc[i]->Info), &(p_->VPPParams.vpp.Out), sizeof(mfxFrameInfo));
		p_->pVPPSurfacesVPPOutEnc[i]->Data.MemId = mfxResponseVPPOutEnc.mids[i];
	}
	
	// Disable default VPP operations
	memset(&p_->extDoNotUse, 0, sizeof(mfxExtVPPDoNotUse));
	p_->extDoNotUse.Header.BufferId = MFX_EXTBUFF_VPP_DONOTUSE;
	p_->extDoNotUse.Header.BufferSz = sizeof(mfxExtVPPDoNotUse);
	p_->extDoNotUse.NumAlg = 4;
	p_->extDoNotUse.AlgList = new mfxU32[p_->extDoNotUse.NumAlg];
	p_->extDoNotUse.AlgList[0] = MFX_EXTBUFF_VPP_DENOISE;       // turn off denoising (on by default)
	p_->extDoNotUse.AlgList[1] = MFX_EXTBUFF_VPP_SCENE_ANALYSIS;        // turn off scene analysis (on by default)
	p_->extDoNotUse.AlgList[2] = MFX_EXTBUFF_VPP_DETAIL;        // turn off detail enhancement (on by default)
	p_->extDoNotUse.AlgList[3] = MFX_EXTBUFF_VPP_PROCAMP;       // turn off processing amplified (on by default)
	// Add extended VPP buffers
	p_->extBuffers[0] = (mfxExtBuffer*)&p_->extDoNotUse;
	p_->VPPParams.ExtParam = p_->extBuffers;
	p_->VPPParams.NumExtParam = 1;
	// Initialize the Media SDK encoder
	p_->sts = p_->mfxENC->Init(&p_->mfxEncParams);
	// Initialize Media SDK VPP
	p_->sts = p_->mfxVPP->Init(&p_->VPPParams);

	// Retrieve video parameters selected by encoder.
	// - BufferSizeInKB parameter is required to set bit stream buffer size
	mfxVideoParam par;
	memset(&par, 0, sizeof(par));
	p_->sts = p_->mfxENC->GetVideoParam(&par);
	
	// Prepare Media SDK bit stream buffer
	
	memset(&p_->mfxBS, 0, sizeof(p_->mfxBS));
	p_->mfxBS.MaxLength = par.mfx.BufferSizeInKB * 1000;
	p_->mfxBS.Data = new mfxU8[p_->mfxBS.MaxLength];

	
	
	
	
}
void Encoder::run(){
	int flag = 1;
	FILE* pFile;
	fopen_s(&pFile, "file.bin", "wb");
	mfxGetTime(&p_->tStart);

	int nEncSurfIdx = 0;
	int nVPPSurfIdx = 0;
	mfxSyncPoint syncpVPP, syncpEnc;
	mfxU32 nFrame = 0;

	for (int i = 0; i < 1000; i++){
		std::unique_lock<std::mutex> camLocker(camBufferMutex);
		while (cameraBusy>1)
			camBufferCond.wait(camLocker);
		//Now I can flip
		flag = flag ^ 1;
		//encoderBusy = true;
		camLocker.unlock();
	
		//Process
		//std::cout << "Processing in Encoder "<<flag<<"\n";
		//fwrite(buffer[flag],1, 3840 * 1600 * 4, pFile);
		//Processing begins here
		nVPPSurfIdx = GetFreeSurfaceIndex(p_->pmfxSurfacesVPPIn, p_->nSurfNumVPPIn);    // Find free input frame surface
		//MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, nVPPSurfIdx, MFX_ERR_MEMORY_ALLOC);

		// Surface locking required when read/write video surfaces
		p_->sts = p_->mfxAllocator.Lock(p_->mfxAllocator.pthis, p_->pmfxSurfacesVPPIn[nVPPSurfIdx]->Data.MemId, &(p_->pmfxSurfacesVPPIn[nVPPSurfIdx]->Data));
		//MSDK_BREAK_ON_ERROR(sts);
		//std::cout << "The error is " << p_->sts << "\n";
		//p_->sts = LoadRawRGBFrame(pmfxSurfacesVPPIn[nVPPSurfIdx], fSource);  // Load frame from file into surface
		//MSDK_BREAK_ON_ERROR(sts);
		memcpy(p_->pmfxSurfacesVPPIn[nVPPSurfIdx]->Data.B, buffer[flag], 3840 * 1600 * 4);

		p_->sts = p_->mfxAllocator.Unlock(p_->mfxAllocator.pthis, p_->pmfxSurfacesVPPIn[nVPPSurfIdx]->Data.MemId, &(p_->pmfxSurfacesVPPIn[nVPPSurfIdx]->Data));
		//MSDK_BREAK_ON_ERROR(sts);

		nEncSurfIdx = GetFreeSurfaceIndex(p_->pVPPSurfacesVPPOutEnc, p_->nSurfNumVPPOutEnc);    // Find free output frame surface
		//MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, nEncSurfIdx, MFX_ERR_MEMORY_ALLOC);
		
		for (;;) {
			// Process a frame asychronously (returns immediately)
			p_->sts = p_->mfxVPP->RunFrameVPPAsync(p_->pmfxSurfacesVPPIn[nVPPSurfIdx], p_->pVPPSurfacesVPPOutEnc[nEncSurfIdx], NULL, &syncpVPP);
			if (MFX_WRN_DEVICE_BUSY == p_->sts) {
				MSDK_SLEEP(1);  // Wait if device is busy, then repeat the same call
			}
			else
				break;
		}

		if (MFX_ERR_MORE_DATA == p_->sts)
			continue;
		for (;;) {
			// Encode a frame asychronously (returns immediately)
			p_->sts = p_->mfxENC->EncodeFrameAsync(NULL, p_->pVPPSurfacesVPPOutEnc[nEncSurfIdx], &p_->mfxBS, &syncpEnc);

			if (MFX_ERR_NONE < p_->sts && !syncpEnc) {  // Repeat the call if warning and no output
				if (MFX_WRN_DEVICE_BUSY == p_->sts)
					MSDK_SLEEP(1);  // Wait if device is busy, then repeat the same call
			}
			else if (MFX_ERR_NONE < p_->sts && syncpEnc) {
				p_->sts = MFX_ERR_NONE;     // Ignore warnings if output is available
				break;
			}
			else if (MFX_ERR_NOT_ENOUGH_BUFFER == p_->sts) {
				// Allocate more bitstream buffer memory here if needed...
				break;
			}
			else
				break;
		}

		if (MFX_ERR_NONE == p_->sts) {
			p_->sts = p_->session.SyncOperation(syncpEnc, 60000);   // Synchronize. Wait until encoded frame is ready
			//MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

			p_->sts = WriteBitStreamFrame(&p_->mfxBS, pFile);
			//MSDK_BREAK_ON_ERROR(sts);

			++nFrame;
			
		
		}








		//Processing ends here



		std::unique_lock<std::mutex> encLocker(encBufferMutex);
		cameraBusy++;
		encoderBusy--;
		encLocker.unlock();
		encBufferCond.notify_one();
		

	}
	printf("Frames encoded: %d\r", nFrame);
	fclose(pFile);
}

std::thread Encoder::runThread(){
	return std::thread([=] {run(); });
}