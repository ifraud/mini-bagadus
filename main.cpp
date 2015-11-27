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
#include "defaults.h"

std::mutex camBufferMutex;
std::condition_variable camBufferCond;
std::mutex encBufferMutex;
std::condition_variable encBufferCond;

int encoderBusy = 0;
int cameraBusy = 2;
char **buffer;

int main(int argc, char *argv[])
{	
	int numDevs;
	buffer = new char*[2];
	buffer[0] = new char[3840 * 1600 *4];
	buffer[1] = new char[3840 * 1600 * 4];


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

	camInstance.close();


}