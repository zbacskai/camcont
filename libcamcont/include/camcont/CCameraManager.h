#pragma once

extern "C" {

	__declspec(dllexport)  int ccammanager_get_cameras(char **io_buffer, unsigned int buffer_size);
	__declspec(dllexport) int ccammanger_set_brightness(const char* cameraName, float brightness);
}