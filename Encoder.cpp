#include "Encoder.h"

#include <iostream>
#include <windows.h>

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
	for (int i = 0; i < 100; i++){
		std::cout << "Just doing random shit in Encoder\n";
		Sleep(100);
	}
}

std::thread Encoder::runThread(){
	return std::thread([=] {run(); });
}