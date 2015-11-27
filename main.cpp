//Standard includes
#include <iostream>
#include <thread>


//MFX includes
#include <mfxvideo++.h>

//IDS includes
#include <uEye.h>

//Project includes
#include "Camera.h"
#include "Encoder.h"

int main(int argc, char *argv[])
{	
	int numDevs;

	is_GetNumberOfCameras(&numDevs);
	std::cout << "The number of cameras connected are " << numDevs << "\n";
	Camera camInstance;
	camInstance.init();

	Encoder encoderInstance;
	encoderInstance.init();

	std::thread camThread = camInstance.runThread();
	std::thread encThread = encoderInstance.runThread();

	camThread.join();
	encThread.join();


}