#pragma once

#include <camcont/CameraManager.hpp>

#include <algorithm>
#include <iostream>
#include <string>

int print_help(const char* execName)
{
	std::cout << "Usage: ";
	std::cout << execName << " < list | set <name> >" << std::endl;

	return -1;
}

int main(int argc, char **argv)
{
	if (argc < 2 || argc > 4)
		return print_help(argv[0]);

	std::cout << argc << std::endl;

	CamCont::CameraManager camManager;
	CamCont::ICamera::Cameras cameras;
	const std::string &command = std::string(argv[1]);
	camManager.getCameras(cameras);
	if (2 == argc &&  command == "list")
	{
		std::cout << "Available Cameras:" << std::endl;
		std::for_each(cameras.begin(), cameras.end(), [](CamCont::ICamera::CameraMapPair &uPtr) { std::cout << uPtr.first << std::endl; });
	}
	else if (4 == argc && command == "set")
	{
		std::string camName(argv[2]);
		float brightness = std::stof(std::string(argv[3]));
		if (cameras.find(camName) != cameras.end())
			cameras[camName]->setBrightness(brightness);
		else
		{
			std::cout << "Invalid camera name: " << camName << std::endl;
			return -1;
		}
	}
	else
		return print_help(argv[0]);
	
	return 0;
}