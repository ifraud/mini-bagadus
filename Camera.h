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

	std::thread runThread();

	Camera();
	~Camera();

protected:
	class Private;
	std::unique_ptr<Private> p_;
};

#endif