#include <camcont/CCameraManager.h>
#include <camcont/CameraManager.hpp>

#include <string>
#include <sstream>
#include <cstring>
#include <algorithm>

int ccammanager_get_cameras(char **io_buffer, unsigned int buffer_size)
{
	char * buffer = *io_buffer;
	if (buffer_size < 1)
		return -1;

	if (buffer_size == 1)
	{
		buffer[0] = 0;
		return 0;
	}
	
	buffer_size -= 1;

	CamCont::CameraManager camManager;
	CamCont::ICamera::Cameras cameras;
	camManager.getCameras(cameras);

	std::stringstream ss;
	std::for_each(cameras.begin(), cameras.end(), [&ss](CamCont::ICamera::CameraMapPair &uPtr) { ss << uPtr.first << ','; });

	unsigned int result_size = ss.str().size();
	if ( result_size < 1)
		return -1;

	result_size -= 1;

	result_size = (result_size < buffer_size) ? result_size : buffer_size;
	std::memcpy(buffer, ss.str().c_str(), result_size);
	buffer[result_size] = 0;

	return result_size;
}

int ccammanger_set_brightness(const char* cameraName, float brightness)
{
	CamCont::CameraManager camManager;
	CamCont::ICamera::Cameras cameras;
	camManager.getCameras(cameras);

	std::string camName(cameraName);
	if (cameras.find(camName) != cameras.end())
		cameras[camName]->setBrightness(brightness);
	else
		return -1;

	return 0;
}

