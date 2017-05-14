import json
import datetime
import time
import pprint
import os
import subprocess

import json

def load_config(filename):
	with open(filename, 'r') as fp:
		return json.load(fp)

class SunInfo(object):
	def __init__ (self, sunrise, sunset):
		self._sunrise = sunrise;
		self._sunset = sunset;

def process_sun_file(filename):
	stimes = dict()
	with open(filename, 'r') as fp:
		sun_times = fp.readlines()

		for stime in sun_times:
			columns = stime.rstrip().split(",")

			date_str = columns[0]
			sunrise_str = columns[1]
			sunset_str = columns[2]

			date = datetime.datetime.strptime(date_str, "%d/%m/%y").date()
			sunrise = datetime.datetime.strptime(sunrise_str, "%H:%M").time()
			sunset = datetime.datetime.strptime(sunset_str, "%H:%M").time()

#			pprint.pprint(date)
#			pprint.pprint(sunrise)
#			pprint.pprint(sunset)

			stimes[date] = SunInfo(sunrise, sunset)

	return stimes

def main():
	config = load_config("camcont.cfg")

	pprint.pprint(config)

	CAMCONTROLLER_PATH=os.path.dirname(os.path.realpath(__file__))
	brightness = 0;
	brightness_old =  -1;
	while (1):
		daylight_tm = process_sun_file('daylight-times.csv')

		now_date_time = datetime.datetime.now()
		now_date = now_date_time.date();
		now_time = now_date_time.time();

		if now_date in daylight_tm.keys():
			min_tm = daylight_tm[now_date]._sunrise
			max_tm = daylight_tm[now_date]._sunset

			if min_tm < now_time and now_time < max_tm:
				print ("Daylight!")
				brightness = 30
			else:
				print ("It is night!")
				brightness = 85

		print (CAMCONTROLLER_PATH)
		subprocess.call([config["camcont-exe"], 'set', config["camera-name"], str(brightness)])

		time.sleep (10)

if __name__ == "__main__":
    main()
