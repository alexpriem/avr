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

check_output()
s.write ("volt 125.00\n")
check_output ()
s.write("current 1.5\n")
check_output ()
check_output ()
s.write("ovp 500.00\n")
check_output ()
check_output ()
s.write("st 1\n")
check_output ()
check_output()

 

		
s.close()

