#ifndef CAMERA_H
#define CAMERA_H

//IDS Includes
#include <uEye.h>

//std Includes
#include <memory>
#include <thread>

class Camera{

public:
	void init();
	void run();
	void close();

	std::thread runThread();

	Camera();
	~Camera();

protected:
	class Private;
	std::unique_ptr<Private> p_;
};

#endif