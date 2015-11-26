#ifndef CAMERA_H
#define CAMERA_H

//IDS Includes
#include <uEye.h>

//std Includes
#include <memory>

class Camera{

public:
	void init();

	Camera();
	~Camera();

protected:
	class Private;
	std::unique_ptr<Private> p_;
};

#endif