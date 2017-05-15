from ctypes import *
import pprint

def main():

	camlib = cdll.LoadLibrary("x64\Debug\libcamcont")

	get_cams = camlib.ccammanager_get_cameras
	get_cams.res_type = c_int

	p = create_string_buffer(1024)
	res = get_cams(p,1024)

	buff = bytearray(p)[:res].decode('utf-8')
		
	print (str(res))
	cameras = buff.split(",")
	for cam in cameras:
		print ("|" + cam + "|")

	
if __name__ == "__main__":
    main()