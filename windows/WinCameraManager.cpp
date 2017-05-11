#include "WinCameraManager.hpp"

#include <new>
#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <Wmcodecdsp.h>
#include <assert.h>
#include <Dbt.h>
#include <shlwapi.h>
#include <mfplay.h>

#include <iostream>

#include <atlcomcli.h>

#include <memory>
#include <mutex>

#pragma comment(lib, "mf")
#pragma comment(lib, "mfplat")

namespace {

	struct WinMFStartup {
		WinMFStartup() {
			std::cout << "MF startup" << std::endl;
			MFStartup(MF_VERSION);
		}
	};

	// Change this to something safer
	WinMFStartup startWinMf;

	template <class T> void SafeRelease(T **ppT)
	{
		if (*ppT)
		{
			(*ppT)->Release();
			*ppT = NULL;
		}
	}

}

namespace WinCamCont {

	class WinCameraController;

	class WinCamera : public CamCont::ICamera {
	private:
		unsigned int index_;
		std::string name_;
	public:
		WinCamera(unsigned int index);
		const std::string& getName() const { return name_; };
		void setBrightness(const float percentage);
	};

	class WinCameraController {
		UINT32      numOfDevices_; 
		IMFActivate **ppDevices_;

	public:
		WinCameraController(); 
		~WinCameraController();
	
		UINT32  count() const { return numOfDevices_; }
		HRESULT getDeviceName(UINT32 index, WCHAR **ppszName);
		HRESULT setCameraBrightness(UINT32 index, float brightness);
	};

	WinCameraController::WinCameraController()
	{
		HRESULT hr = S_OK;
		IMFAttributes *pAttributes = NULL;

		hr = MFCreateAttributes(&pAttributes, 1);

		if (SUCCEEDED(hr))
			hr = pAttributes->SetGUID(
				MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
				MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);

		if (SUCCEEDED(hr))
			hr = MFEnumDeviceSources(pAttributes, &ppDevices_, &numOfDevices_);

		SafeRelease(&pAttributes);

		if (FAILED(hr))
			throw std::exception("FATAL: Failed to initialise camera controller");
	}

	WinCameraController::~WinCameraController() 
	{
		for (UINT32 i = 0; i < numOfDevices_; i++)
		{
			SafeRelease(&ppDevices_[i]);
		}
		CoTaskMemFree(ppDevices_);
		ppDevices_ = nullptr;

		numOfDevices_ = 0;
	}

	HRESULT WinCameraController::getDeviceName(UINT32 index, WCHAR **ppszName)
	{
		if (index >= count())
			return E_INVALIDARG;
		
		HRESULT hr = S_OK;

		hr = ppDevices_[index]->GetAllocatedString(
			MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME,
			ppszName,
			nullptr);

		return hr;
	}

	HRESULT WinCameraController::setCameraBrightness(UINT32 index, float brightness)
	{
		IMFMediaSource *camera = nullptr;
		HRESULT hr = ppDevices_[index]->ActivateObject(IID_PPV_ARGS(&camera));
		if (FAILED(hr))
			return hr;

		CComQIPtr<IAMVideoProcAmp> camControl(camera);
		if (camControl)
		{
			long min, max, step, def, control;
			camControl->GetRange(VideoProcAmp_Brightness, &min, &max, &step, &def, &control);
			float max_steps = (static_cast<float>(max - min) / static_cast<float>(step));
			long calcBrightness = static_cast<long>(max_steps * brightness / 100.00) + min;
			std::cout << min << " " << calcBrightness << " " << max << std::endl;
			camControl->Set(VideoProcAmp_Brightness, calcBrightness, VideoProcAmp_Flags_Auto);
		}
		return S_OK;
	}

	static thread_local WinCameraController winCamCont;

	WinCamera::WinCamera(unsigned int index) : index_(index)
	{
		WCHAR *deviceName;
		HRESULT hr = winCamCont.getDeviceName(index_, &deviceName);
		char nameBuff[256];
		char defChar = ' ';
		WideCharToMultiByte(CP_ACP, 0, deviceName, -1, nameBuff, sizeof(nameBuff), &defChar, nullptr);
		name_ = nameBuff;
		CoTaskMemFree(deviceName);
	}

	void WinCamera::setBrightness(float brightness)
	{
		winCamCont.setCameraBrightness(index_, brightness);
	}

	void WinCameraManager::getCameras(CamCont::ICamera::Cameras &ioCameras)
	{
		
		unsigned int numOfCams = winCamCont.count();
		std::cout << "Number of cameras found: " << numOfCams << std::endl;
		for (unsigned int i = 0; i < numOfCams; ++i)
		{
			std::unique_ptr<CamCont::ICamera> newCam(new WinCamera(i));
			const std::string &name= newCam->getName();
			std::cout << "Name: " << name << std::endl;
			ioCameras.insert(CamCont::ICamera::CameraMapPair(name, std::move(newCam)));
		}
	}

}