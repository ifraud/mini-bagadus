#ifndef ENCODER_H
#define ENCODER_H

//std Includes
#include <memory>
#include <thread>

class Encoder{

public:
	void init();
	void run();

	std::thread runThread();

	Encoder();
	~Encoder();

protected:
	class Private;
	std::unique_ptr<Private> p_;
};

#endif