#include "defaults.h"
#include "Encoder.h"

#include <iostream>
#include <windows.h>
#include <stdio.h>

class Encoder::Private
{
public:
	Private();

};
Encoder::Private::Private(){

}
Encoder::Encoder() : p_(new Private()){

}

Encoder::~Encoder() = default;

void Encoder::init(){


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