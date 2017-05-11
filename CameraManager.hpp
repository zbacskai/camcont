#pragma once

#include "ICamera.hpp"

namespace CamCont {

	class CameraManager {
	public:
		void getCameras(ICamera::Cameras &ioCameras);
	};

}