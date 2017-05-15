#pragma once

#include "ICamera.hpp"

namespace CamCont {

	class __declspec(dllexport) CameraManager {
	public:
		void getCameras(ICamera::Cameras &ioCameras);
	};

}