//Standard includes
#include <iostream>


//MFX includes
#include <mfxvideo++.h>

//IDS includes
#include <uEye.h>

int main(int argc, char *argv[])
{	
	int numDevs;

	is_GetNumberOfCameras(&numDevs);
	std::cout << "The number of cameras are " << numDevs << std::endl;
}