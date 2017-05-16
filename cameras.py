from ctypes import *

CAMLIB = cdll.LoadLibrary("x64\Debug\libcamcont")

class Camera(object):
	def __init__(self, cam_handle):
		self._get_camera_name = CAMLIB.ccammanger_camera_get_name
		self._set_camera_brightness = CAMLIB.ccammanger_camera_set_brightness
		self._set_camera_brightness.argtypes = [c_void_p, c_float]

		self._cm_handle = cam_handle

	def get_name(self):
		bf = create_string_buffer(1024)
		res = self._get_camera_name(self._cm_handle, bf, 1024)
		buff = bytearray(bf)[:res].decode('utf-8')
		return buff

	def set_brightness(self, brightness):
		self._set_camera_brightness(self._cm_handle, brightness)

class Cameras(object):
	def __init__(self):
			
		self._get_cameras = CAMLIB.ccammanger_get_cameras
		self._get_cameras.res_type = c_void_p

		self._retrieved_cameras =  self._get_cameras()

		self._dispose_cameras = CAMLIB.ccammanger_dispose_cameras

		self._get_camera_names =  CAMLIB.ccammanger_get_camera_names

		self._get_camera = CAMLIB.ccammanger_get_camera
		self._get_camera.argtypes = [c_void_p, c_char_p]
		self._get_camera.res_type = c_void_p

		self._cameras = dict()
		bf = create_string_buffer(1024)
		res = self._get_camera_names(self._retrieved_cameras, bf, 1024)
		buff = bytearray(bf)[:res].decode('utf-8')
		for cm_name in buff.split(','):
			self._cameras[cm_name] = Camera(self._get_camera(self._retrieved_cameras, create_string_buffer(cm_name.encode('utf-8'))))
		
	def get_cameras(self):
		return self._cameras

	def __del__(self):
		self._dispose_cameras(self._retrieved_cameras)

if __name__ == "__main__":
	c  = Cameras();
	for cam in c._cameras.keys():
		print (cam + " : " + c._cameras[cam].get_name())

c._cameras["Lenovo EasyCamera"].set_brightness(0.0)