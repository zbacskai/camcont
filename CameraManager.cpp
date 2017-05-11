#include "CameraManager.hpp"

#include "windows/WinCameraManager.hpp"

namespace CamCont {

	void CameraManager::getCameras(ICamera::Cameras &ioCameras)
	{
		WinCamCont::WinCameraManager winCamMgr;
		winCamMgr.getCameras(ioCameras);
	}
}