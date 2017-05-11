#pragma once

#include "../ICamera.hpp"

namespace WinCamCont {

	class WinCameraManager {
	public:
		void getCameras(CamCont::ICamera::Cameras &ioCameras);
	};

}