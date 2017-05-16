#pragma once

extern "C" {

	__declspec(dllexport) void* ccammanger_get_cameras();
	__declspec(dllexport) void ccammanger_dispose_cameras(void* cameras);
	__declspec(dllexport) void* ccammanger_get_camera(void* cameras, const char* cam_name);
	__declspec(dllexport) unsigned int ccammanger_get_camera_names(void* cameras, char* cam_names, unsigned int cam_names_size);
	__declspec(dllexport) unsigned int ccammanger_camera_get_name(void* camera, char* name, unsigned int name_len);
	__declspec(dllexport) void ccammanger_camera_set_brightness(void* camera, float brightness);
}