#pragma once

#include <string>
#include <map>
#include <memory>

namespace CamCont {

	class __declspec(dllexport) ICamera {
	public:
		typedef std::map <const std::string, std::unique_ptr<ICamera> > Cameras;
		typedef std::pair<const std::string, std::unique_ptr<ICamera> > CameraMapPair;

		virtual const std::string& getName() const = 0;
		virtual void setBrightness(const float percentage) = 0;

		virtual ~ICamera() {};
	};

}
