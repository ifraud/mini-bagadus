//Standard includes
#include <iostream>


//MFX includes
#include <mfxvideo++.h>

//IDS includes
#include <uEye.h>

//Project includes
#include "Camera.h"

int main(int argc, char *argv[])
{	
	int numDevs;

	is_GetNumberOfCameras(&numDevs);
	std::cout << "The number of cameras connected are " << numDevs << "\n";
	Camera camInstance;
	camInstance.init();


}