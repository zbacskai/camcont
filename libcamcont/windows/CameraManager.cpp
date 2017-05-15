#include <camcont/CameraManager.hpp>

#include "WinCameraManager.hpp"

namespace CamCont {

	void CameraManager::getCameras(ICamera::Cameras &ioCameras)
	{
		WinCamCont::WinCameraManager winCamMgr;
		winCamMgr.getCameras(ioCameras);
	}
}