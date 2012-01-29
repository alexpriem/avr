import serial, sys
import serial, sys
from time import sleep

def check_output():
	for i in range (0,100):
		txt=s.readline()	
		if len(txt)==0:
			break
		if txt[-1]=='\n':
			print txt[:-1]
		else:
			print txt
		
		
s=serial.Serial('com3',baudrate=9600, timeout=0.5 )

for v in [0,0,0]:
	s.write("volt %.2f\n"  % float(v))
	check_output()
	if v!=0:
		sleep(5)
	
