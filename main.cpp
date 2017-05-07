#pragma once
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

#pragma comment(lib, "mf")
#pragma comment(lib, "mfplat")

const UINT WM_APP_PREVIEW_ERROR = WM_APP + 1;    // wparam = HRESULT

class DeviceList
{
	UINT32      m_cDevices; // contains the number of devices
	IMFActivate **m_ppDevices; // contains properties about each device

public:
	DeviceList() : m_ppDevices(NULL), m_cDevices(0)
	{

	}
	~DeviceList()
	{
		Clear();
	}

	UINT32  Count() const { return m_cDevices; }

	void    Clear();
	HRESULT EnumerateDevices();
	HRESULT GetDevice(UINT32 index, IMFActivate **ppActivate);
	HRESULT GetDeviceName(UINT32 index, WCHAR **ppszName);
	// Might be leaking. Double check
	IMFMediaSource* getCameraSource(UINT32 index);
};




//#include "DeviceList.h"

/*
* A templated Function SafeRelease releasing pointers memories
* @param ppT the pointer to release
*/

template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}



/*
* A function which copy attribute form source to a destination
* @ param pSrc is an Interface to store key/value pairs of an Object
* @ param pDest is an Interface to store key/value pairs of an Object
* @ param GUID is an unique identifier
* @ return HRESULT return errors warning condition on windows
*/

HRESULT CopyAttribute(IMFAttributes *pSrc, IMFAttributes *pDest, const GUID& key);




/*
* A Method form DeviceList which clear the list of Devices
*/

void DeviceList::Clear()
{
	for (UINT32 i = 0; i < m_cDevices; i++)
	{
		SafeRelease(&m_ppDevices[i]);
	}
	CoTaskMemFree(m_ppDevices);
	m_ppDevices = NULL;

	m_cDevices = 0;
}


/*
* A function which enumerate the list of Devices.
* @ return HRESULT return errors warning condition on windows
*/
HRESULT DeviceList::EnumerateDevices()
{
	HRESULT hr = S_OK;
	IMFAttributes *pAttributes = NULL;

	this->Clear();

	// Initialize an attribute store. We will use this to
	// specify the enumeration parameters.
	std::cout << "Enumerate devices" << std::endl;
	hr = MFCreateAttributes(&pAttributes, 1);

	// Ask for source type = video capture devices
	if (SUCCEEDED(hr))
	{
		std::cout << "Enumerate devices" << std::endl;
		hr = pAttributes->SetGUID(
			MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
			MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID
		);
	}
	// Enumerate devices.
	if (SUCCEEDED(hr))
	{
		std::cout << "Enumerate devices:" << m_cDevices << std::endl;
		hr = MFEnumDeviceSources(pAttributes, &m_ppDevices, &m_cDevices);
	}

	SafeRelease(&pAttributes);

	return hr;
}


/*
* A function which copy attribute form source to a destination
* @ param index the index in an array
* @ param ppActivate is an Interface to store key/value pairs of an Object
* @ return HRESULT return errors warning condition on windows
*/


HRESULT DeviceList::GetDevice(UINT32 index, IMFActivate **ppActivate)
{
	if (index >= Count())
	{
		return E_INVALIDARG;
	}

	*ppActivate = m_ppDevices[index];
	(*ppActivate)->AddRef();

	return S_OK;
}


/*
* A function which get the name of the devices
* @ param index the index in an array
* @ param ppszName Name of the device
*/


HRESULT DeviceList::GetDeviceName(UINT32 index, WCHAR **ppszName)
{
	std::cout << "Get Device name" << std::endl;
	if (index >= Count())
	{
		return E_INVALIDARG;
	}

	HRESULT hr = S_OK;

	hr = m_ppDevices[index]->GetAllocatedString(
		MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME,
		ppszName,
		NULL
	);

	return hr;
}

#include <iostream>
//#include "DeviceList.h"

HRESULT SetupCamera(DeviceList &devList, UINT32 index)
{
	IMFMediaSource *camera = devList.getCameraSource(index);

	CComQIPtr<IAMVideoProcAmp> camControl(camera);
	HRESULT hr = S_OK;
	if (camControl)
	{
		long min, max, step, def, control;
		camControl->GetRange(VideoProcAmp_Brightness, &min, &max, &step, &def, &control);
		std::cout << "Getting settings for camera brightness of cam: " << index << std::endl;
		std::cout << "--------------------------------------------------------" << std::endl;
		std::cout << "    Min: " << min << std::endl;
		std::cout << "    Max: " << max << std::endl;
		std::cout << "   Step: " << step << std::endl;
		std::cout << "Default: " << def << std::endl;
		std::cout << "Control: " << def << std::endl;

		camControl->Set(VideoProcAmp_Brightness, 100, VideoProcAmp_Flags_Auto);
	}
	return S_OK;
}

HRESULT UpdateDeviceList()
{
	HRESULT hr = S_OK;
	WCHAR *szFriendlyName = NULL;

	DeviceList g_devices;

	g_devices.Clear();

	hr = g_devices.EnumerateDevices();

	if (FAILED(hr)) { goto done; }
	std::cout << "Nb devices found:" << g_devices.Count() << std::endl;

	for (UINT32 iDevice = 0; iDevice < g_devices.Count(); iDevice++)
	{
		//std::cout << "" << std::endl;
		hr = g_devices.GetDeviceName(iDevice, &szFriendlyName);
		if (FAILED(hr)) { goto done; }
		std::cout << szFriendlyName << std::endl;
		// The list might be sorted, so the list index is not always the same as the
		// array index. Therefore, set the array index as item data.
		CoTaskMemFree(szFriendlyName);
		szFriendlyName = NULL;
	}
	std::cout << "End of EnumDeviceList" << std::endl;
	SetupCamera(g_devices, 0);
done:
	return hr;
}

IMFMediaSource* DeviceList::getCameraSource(UINT32 index)
{
	IMFMediaSource *pSource = NULL;
	m_ppDevices[index]->ActivateObject(IID_PPV_ARGS(&pSource));
	return pSource;
}

int main()
{
	std::cout << "Main" << std::endl;
	MFStartup(MF_VERSION);
	UpdateDeviceList();
	while (1);
	return 0;
}