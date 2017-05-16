#include <camcont/CCameraManager.h>
#include <camcont/CameraManager.hpp>

#include <string>
#include <sstream>
#include <cstring>
#include <algorithm>

#include <iostream>

void* ccammanger_get_cameras()
{
	CamCont::CameraManager camManager;
	CamCont::ICamera::Cameras *cameras = new CamCont::ICamera::Cameras();
	camManager.getCameras(*cameras);
	return cameras;
}

void ccammanger_dispose_cameras(void* cameras)
{
	CamCont::ICamera::Cameras* cmrs = static_cast<CamCont::ICamera::Cameras*>(cameras);
	delete cmrs;
}

void* ccammanger_get_camera(void* cameras, const char* cam_name)
{
	std::cout << "|" << cam_name << std::endl;
	CamCont::ICamera::Cameras* cmrs = static_cast<CamCont::ICamera::Cameras*>(cameras);
	CamCont::ICamera::Cameras::iterator cam_it = cmrs->find(std::string(cam_name));
	if (cam_it != cmrs->end())
		return cam_it->second.get();

	return nullptr;
}

unsigned int ccammanger_get_camera_names(void* cameras, char* cam_names, unsigned int cam_names_size)
{
	if (cam_names_size <= 1)
	{
		if (cam_names_size == 1)
			cam_names[0] = 0;

		return 0;
	}

	cam_names_size -= 1;

	CamCont::ICamera::Cameras* cmrs = static_cast<CamCont::ICamera::Cameras*>(cameras);
	std::stringstream ss;
	std::for_each(cmrs->begin(), cmrs->end(), [&ss](CamCont::ICamera::CameraMapPair &uPtr) { ss << uPtr.first << ','; });

	unsigned int result_size = ss.str().size();
	if (result_size < 1)
		return 0;

	result_size -= 1;

	result_size = (result_size < cam_names_size) ? result_size : cam_names_size;
	std::memcpy(cam_names, ss.str().c_str(), result_size);
	cam_names[result_size] = 0;

	return result_size;
}

unsigned int  ccammanger_camera_get_name(void* camera, char* name, unsigned int name_len)
{
	CamCont::ICamera* cam = static_cast<CamCont::ICamera*>(camera);
	const std::string &nm = cam->getName();

	unsigned int len = (nm.length() + 1 > name_len) ? name_len : nm.length() + 1;

	std::memcpy(name, nm.c_str(), len);

	name[len] = 0;

	return len;
}

void ccammanger_camera_set_brightness(void* camera, float brightness)
{
	CamCont::ICamera* cam = static_cast<CamCont::ICamera*>(camera);
	cam->setBrightness(brightness);
}

